#import <Foundation/Foundation.h>
#import <Foundation/NSObject.h>
#import <IOBluetooth/objc/IOBluetoothDevice.h>
#import <IOBluetooth/objc/IOBluetoothDeviceInquiry.h>
#import <IOBluetooth/objc/IOBluetoothRFCOMMChannel.h>


@interface Discoverer: NSObject {
void (*func)(const BluetoothDeviceAddress *, void *);
void *context;
}
-(Discoverer*) initWithCallback: (void (*)(const BluetoothDeviceAddress *, void*)) f context: (void *) ctx;

-(void) deviceInquiryComplete: (IOBluetoothDeviceInquiry*) sender 
                            error: (IOReturn) error
                            aborted: (BOOL) aborted;
-(void) deviceInquiryDeviceFound: (IOBluetoothDeviceInquiry*) sender
                            device: (IOBluetoothDevice*) device;
@end


@implementation Discoverer
-(Discoverer*) initWithCallback: (void (*)(const BluetoothDeviceAddress *, void*)) f context: (void *) ctx {
    self = [super init];

    if (self) {
        self->func = f;
        self->context = ctx;
    }

    return self;
}

-(void) deviceInquiryComplete: (IOBluetoothDeviceInquiry*) sender 
                            error: (IOReturn) error
                            aborted: (BOOL) aborted
{
    CFRunLoopStop( CFRunLoopGetCurrent() );
}
-(void) deviceInquiryDeviceFound: (IOBluetoothDeviceInquiry*) sender
                            device: (IOBluetoothDevice*) device
{
    const BluetoothDeviceAddress *addr = [device getAddress];

    if (addr->data[0] == 0x00 && addr->data[1] == 0x16 && addr->data[2] == 0x53) {
        // Lego NXT found
        func(addr, self->context);
    }
}

@end


@interface TrafficHandler : NSObject {
BOOL response;
}
- (TrafficHandler*) initWithResponse: (BOOL)r;
- (void)rfcommChannelData:(IOBluetoothRFCOMMChannel*)channel 
                     data:(void *)dataPointer 
                   length:(size_t)dataLength;
- (void)rfcommChannelWriteComplete:(IOBluetoothRFCOMMChannel*) channel
                            refcon:(void*)refcon
                            status:(IOReturn) error;
@property (readonly, nonatomic) NSData *message;
@end


@implementation TrafficHandler {
NSMutableData *receivedData;
BOOL processingMessage;
size_t messageLength;
}

-(TrafficHandler*) initWithResponse: (BOOL)r {
    self = [super init];

    if (self) {
        self->response = r;
        self->processingMessage = FALSE;
        self->receivedData = [[NSMutableData alloc] init];
    }
    return self;
}

- (void)saveBuffer:(char *)dataPointer
            length:(size_t)dataLength  {
    if (!processingMessage) {
        messageLength = dataPointer[0];
        processingMessage = TRUE;

        NSData *data = [NSData dataWithBytes: dataPointer length: dataLength];
        [receivedData appendData:[data subdataWithRange:NSMakeRange(2, [data length] - 2)]];
    } else {
        NSData *data = [NSData dataWithBytes: dataPointer length: dataLength];
        [receivedData appendData:data];
    }

    if (messageLength == [receivedData length]) {

        //_message = [NSData dataWithData:receivedData];
        _message = [receivedData copy];
        [receivedData release];
        CFRunLoopStop( CFRunLoopGetCurrent() );
    }
}

- (void)rfcommChannelWriteComplete:(IOBluetoothRFCOMMChannel*) channel
                            refcon:(void*)refcon
                            status:(IOReturn) error {
    if(!(self->response)) {
        // Not waiting for a response, stop loop
        CFRunLoopStop( CFRunLoopGetCurrent() );
    }
}

- (void)rfcommChannelData:(IOBluetoothRFCOMMChannel*)channel 
                     data:(void *)dataPointer 
                   length:(size_t)dataLength
{
    [self saveBuffer:dataPointer length:dataLength];
}
@end


@interface ConnectionHandler : NSObject {
}
- (void)rfcommChannelOpenComplete:(IOBluetoothRFCOMMChannel*)chan 
                           status:(IOReturn)status;
@end


@implementation ConnectionHandler
- (void)rfcommChannelOpenComplete:(IOBluetoothRFCOMMChannel*)chan 
                           status:(IOReturn)status
{
/*
    if( kIOReturnSuccess == status ) {
        [self setChannel: channel];
    }
*/
    CFRunLoopStop( CFRunLoopGetCurrent() );
}
@end


void r2d2_bt_scan(void (*f)(const BluetoothDeviceAddress *, void *), void *arg) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    Discoverer *d = [[Discoverer alloc] initWithCallback: f context: arg];

    IOBluetoothDeviceInquiry *bdi = 
        [[IOBluetoothDeviceInquiry alloc] init];
    [bdi setDelegate: d];
    
    [bdi start];

    CFRunLoopRun();

    [bdi release];
    [d release];
    [pool release];
}

IOBluetoothRFCOMMChannelRef r2d2_bt_open_channel(const BluetoothDeviceAddress *addr) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    IOBluetoothDevice *remote_device = 
        [IOBluetoothDevice deviceWithAddress:addr];
    IOBluetoothRFCOMMChannel *chan;
    ConnectionHandler *handler = [[ConnectionHandler alloc] init];
    
    [remote_device openRFCOMMChannelAsync:&chan withChannelID:1 
                            delegate: handler];
    
    CFRunLoopRun();

    IOBluetoothRFCOMMChannelRef refchan = [chan getRFCOMMChannelRef];
//    [handler release];
    [pool release];
    return refchan;
}

void r2d2_bt_write(IOBluetoothRFCOMMChannelRef refchan, char *data, size_t length, void *responseBuffer, size_t *responseLength) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    BOOL response = (data[2] == 0x00 || data[2] == 0x01);

    TrafficHandler *handler =
        [[TrafficHandler alloc] initWithResponse: response];

    IOBluetoothRFCOMMChannel *channel = [IOBluetoothRFCOMMChannel withRFCOMMChannelRef: refchan];

    [channel setDelegate: handler];

    [channel writeAsync: data length: length refcon: NULL];

    CFRunLoopRun();

    size_t messageLength = [handler.message length];
    unsigned char str[messageLength];

    [handler.message getBytes:responseBuffer length:messageLength];

    *responseLength = messageLength;

    [handler.message release];
    [handler release];
    [pool release];
}
