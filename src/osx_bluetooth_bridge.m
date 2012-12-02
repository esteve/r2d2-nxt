#import <Foundation/Foundation.h>
#import <Foundation/NSObject.h>
#import <IOBluetooth/objc/IOBluetoothDevice.h>
#import <IOBluetooth/objc/IOBluetoothDeviceInquiry.h>
#import <IOBluetooth/objc/IOBluetoothRFCOMMChannel.h>


@interface Discoverer: NSObject {
void (*func)(void *, void *);
void *argument;
}
-(Discoverer*) initWithCallback: (void (*)(void *, void*)) f context: (void *) ctx;

-(void) deviceInquiryComplete: (IOBluetoothDeviceInquiry*) sender 
                            error: (IOReturn) error
                            aborted: (BOOL) aborted;
-(void) deviceInquiryDeviceFound: (IOBluetoothDeviceInquiry*) sender
                            device: (IOBluetoothDevice*) device;
@end


@implementation Discoverer
-(Discoverer*) initWithCallback: (void (*)(void *, void*)) f context: (void *) ctx {
    self = [super init];

    if (self) {
        self->func = f;
        self->argument = ctx;
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
    if (addr->data[5] == 0x00 && addr->data[4] == 0x16 && addr->data[3] == 0x53) {
        // Lego NXT found
        func((void *)addr, self->argument);
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
@end


@implementation TrafficHandler
-(TrafficHandler*) initWithResponse: (BOOL)r {
    self = [super init];

    if (self) {
        self->response = r;
    }

    return self;
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
    // store data in a buffer

    printf("received: %s\n", dataPointer);
    //[channel closeChannel];
    CFRunLoopStop( CFRunLoopGetCurrent() );
}
@end


@interface ConnectionHandler : NSObject {
IOBluetoothRFCOMMChannel *channel;
}
- (void)rfcommChannelOpenComplete:(IOBluetoothRFCOMMChannel*)chan 
                           status:(IOReturn)status;
- (void)rfcommChannelData:(IOBluetoothRFCOMMChannel*)chan 
                     data:(void *)dataPointer 
                   length:(size_t)dataLength;
@property (nonatomic, assign) IOBluetoothRFCOMMChannel *channel;
@end


@implementation ConnectionHandler
- (void)rfcommChannelOpenComplete:(IOBluetoothRFCOMMChannel*)chan 
                           status:(IOReturn)status
{
    if( kIOReturnSuccess == status ) {
        [self setChannel: channel];
    }
    CFRunLoopStop( CFRunLoopGetCurrent() );
}
- (void)rfcommChannelData:(IOBluetoothRFCOMMChannel*)chan 
                     data:(void *)dataPointer 
                   length:(size_t)dataLength
{
    printf("received: %s\n", dataPointer);
    [channel closeChannel];
    CFRunLoopStop( CFRunLoopGetCurrent() );
}
@synthesize channel;
@end


void r2d2_bt_scan(void (*f)(void *, void *), void *arg) {
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

void *r2d2_bt_open_channel(void *addr) {
    BluetoothDeviceAddress *btaddr = addr;

    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    IOBluetoothDevice *remote_device = 
        [IOBluetoothDevice deviceWithAddress:btaddr];
    IOBluetoothRFCOMMChannel *chan;
    ConnectionHandler *handler = [[ConnectionHandler alloc] init];

    [remote_device openRFCOMMChannelAsync:&chan withChannelID:1 
                            delegate: handler];
    
    CFRunLoopRun();

    [handler release];
    [pool release];
    return chan;
}

void r2d2_bt_write(void *chan, char *data, size_t length, void *responseBuffer) {
    // this method should know whether we expect a a response
    // we can check the first byte in data
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    BOOL response = (data[0] == 0x00 || data[0] == 0x01);

    TrafficHandler *handler =
        [[TrafficHandler alloc] initWithResponse: response];

    IOBluetoothRFCOMMChannel *channel = (IOBluetoothRFCOMMChannel *)chan;

    [channel setDelegate: handler];

    [channel writeAsync: data length: length refcon: NULL];

    CFRunLoopRun();

    // store data in the responseBuffer?
    // responseBuffer = handler.responseBuffer
    [handler release];
    [pool release];
}


void r2d2_bt_read(void *chan, char *data, size_t length, void *responseBuffer) {
    // this method should know whether we expect a a response
    // we can check the first byte in data
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    BOOL response = (data[0] == 0x00 || data[0] == 0x01);

    TrafficHandler *handler =
        [[TrafficHandler alloc] initWithResponse: response];

    IOBluetoothRFCOMMChannel *channel = (IOBluetoothRFCOMMChannel *)chan;

    [channel setDelegate: handler];

    [channel writeAsync: data length: length refcon: NULL];

    CFRunLoopRun();

    // store data in the responseBuffer?
    // responseBuffer = handler.responseBuffer
    [handler release];
    [pool release];
}


/*




int main( int argc, const char *argv[] ) 
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    Discoverer *d = [[Discoverer alloc] init];
    ConnectionHandler *handler = [[ConnectionHandler alloc] init];

    IOBluetoothDeviceInquiry *bdi = 
        [[IOBluetoothDeviceInquiry alloc] init];
    [bdi setDelegate: d];
    
    [bdi start];

    CFRunLoopRun();

    [bdi release];
    [d release];
    [pool release];
    return 0;
}

int main( int argc, const char *argv[] ) 
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    NSString *addr_str = @"00:10:60:A8:95:49";
    BluetoothDeviceAddress addr;
    IOBluetoothNSStringToDeviceAddress( addr_str, &addr );
    IOBluetoothDevice *remote_device = 
        [IOBluetoothDevice withAddress:&addr];
    IOBluetoothRFCOMMChannel *chan;
    ConnectionHandler *handler = [[ConnectionHandler alloc] init];

    [remote_device openRFCOMMChannelAsync:&chan withChannelID:1 
                            delegate: handler];
    
    CFRunLoopRun();

    [handler release];
    [pool release];
    return 0;
}

*/
