#import <Foundation/Foundation.h>
#import <Foundation/NSObject.h>
#import <IOBluetooth/objc/IOBluetoothDevice.h>
#import <IOBluetooth/objc/IOBluetoothDeviceInquiry.h>
#import <IOBluetooth/objc/IOBluetoothRFCOMMChannel.h>

@interface Discoverer: NSObject {
NSString *addressString;
BluetoothDeviceAddress *addr;
}
-(void) deviceInquiryComplete: (IOBluetoothDeviceInquiry*) sender 
                            error: (IOReturn) error
                            aborted: (BOOL) aborted;
-(void) deviceInquiryDeviceFound: (IOBluetoothDeviceInquiry*) sender
                            device: (IOBluetoothDevice*) device;
@property (atomic, assign) NSString *addressString;
@property (atomic, assign) BluetoothDeviceAddress *addr;
@end
@implementation Discoverer
-(void) deviceInquiryComplete: (IOBluetoothDeviceInquiry*) sender 
                            error: (IOReturn) error
                            aborted: (BOOL) aborted
{
    CFRunLoopStop( CFRunLoopGetCurrent() );
}
-(void) deviceInquiryDeviceFound: (IOBluetoothDeviceInquiry*) sender
                            device: (IOBluetoothDevice*) device
{
    NSString *addrStr = [device getAddressString];
    BluetoothDeviceAddress *addr = [device getAddress];
    if (addr->addr[5] == 0x00 && addr->addr[4] == 0x16 && addr->addr[3] == 0x53) {
        // Lego NXT found
        [self setAddr:addr];
    }

    [self setAddressString:addrStr];
}
@synthesize addressString;
@synthesize addr;
@end


@interface ConnectionHandler : NSObject {}
- (void)rfcommChannelOpenComplete:(IOBluetoothRFCOMMChannel*)channel 
                           status:(IOReturn)status;
- (void)rfcommChannelData:(IOBluetoothRFCOMMChannel*)channel 
                     data:(void *)dataPointer 
                   length:(size_t)dataLength;
@end
@implementation ConnectionHandler
- (void)rfcommChannelOpenComplete:(IOBluetoothRFCOMMChannel*)channel 
                           status:(IOReturn)status
{
    if( kIOReturnSuccess == status ) {
        printf("connection established\n");
        [channel writeSync: "Hello!" length: 6];
    } else {
        printf("Connection error!\n");
        CFRunLoopStop( CFRunLoopGetCurrent() );
    }
}
- (void)rfcommChannelData:(IOBluetoothRFCOMMChannel*)channel 
                     data:(void *)dataPointer 
                   length:(size_t)dataLength
{
    printf("received: %s\n", dataPointer);
    [channel closeChannel];
    CFRunLoopStop( CFRunLoopGetCurrent() );
}
@end


void r2d2_bt_scan(void (*f)(void *, void *), void *arg) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    Discoverer *d = [[Discoverer alloc] init];
    ConnectionHandler *handler = [[ConnectionHandler alloc] init];

    IOBluetoothDeviceInquiry *bdi = 
        [[IOBluetoothDeviceInquiry alloc] init];
    [bdi setDelegate: d];
    
    [bdi start];

    CFRunLoopRun();

    NSString *addrStr = d.addressString;
    f([addrStr cString], [addrStr cString]);

    [bdi release];
    [d release];
    [pool release];
}











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
