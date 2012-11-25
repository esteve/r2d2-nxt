#import <Foundation/NSObject.h>
#import <IOBluetooth/objc/IOBluetoothDevice.h>
#import <IOBluetooth/objc/IOBluetoothDeviceInquiry.h>
#import <stdio.h>

@interface Discoverer: NSObject {}
-(void) deviceInquiryComplete: (IOBluetoothDeviceInquiry*) sender 
                            error: (IOReturn) error
                            aborted: (BOOL) aborted;
-(void) deviceInquiryDeviceFound: (IOBluetoothDeviceInquiry*) sender
                            device: (IOBluetoothDevice*) device;
@end
@implementation Discoverer
-(void) deviceInquiryComplete: (IOBluetoothDeviceInquiry*) sender 
                            error: (IOReturn) error
                            aborted: (BOOL) aborted
{
    printf("inquiry complete\n");
    CFRunLoopStop( CFRunLoopGetCurrent() );
}
-(void) deviceInquiryDeviceFound: (IOBluetoothDeviceInquiry*) sender
                            device: (IOBluetoothDevice*) device
{
    printf("discovered %s\n", [[device getAddressString] cString]);
}
@end

int main( int argc, const char *argv[] ) 
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    Discoverer *d = [[Discoverer alloc] init];

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
