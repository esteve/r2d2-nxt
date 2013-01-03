#ifndef R2D2_OSX_BLUETOOTH_BRIDGE_H
#define R2D2_OSX_BLUETOOTH_BRIDGE_H

#include <IOBluetooth/Bluetooth.h>
#include <IOBluetooth/IOBluetoothUserLib.h>

#ifdef __cplusplus
extern "C" {
#endif
void r2d2_bt_scan(void (*f)(const BluetoothDeviceAddress *, void *), void *arg);

IOBluetoothRFCOMMChannelRef r2d2_bt_open_channel(const BluetoothDeviceAddress *addr);

void r2d2_bt_write(IOBluetoothRFCOMMChannelRef refchan, void *data, size_t length, void *responseBuffer, size_t *responseLength);
#ifdef __cplusplus
}
#endif

#endif
