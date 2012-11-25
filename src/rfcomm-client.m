#import <Foundation/NSObject.h>
#import <IOBluetooth/objc/IOBluetoothDevice.h>
#import <IOBluetooth/objc/IOBluetoothDeviceInquiry.h>
#import <IOBluetooth/objc/IOBluetoothRFCOMMChannel.h>
#import <stdio.h>

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
