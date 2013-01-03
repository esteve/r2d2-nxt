#include <vector>
#include <cstring>
#include <r2d2.hpp>
#include <r2d2/bluetooth.hpp>
#include <r2d2/osx_bluetooth.hpp>
#include <r2d2/osx_bluetooth_bridge.h>

BTTransport::BTTransport(const BluetoothDeviceAddress *addr) {
    this->addr_ = addr;
}

BTTransport::~BTTransport() {
//    free(this->addr_);
}

void addBTDeviceToList(const BluetoothDeviceAddress *addr, void *arg) {
    BTTransport *transport = new BTTransport(addr);
    Brick *brick = new Brick(new Comm(transport));
    std::vector<Brick *> *v = static_cast< std::vector<Brick *> *>(arg);
    v->push_back(brick);
}

bool BTTransport::open() {
    this->rfcommChannelRef_ = r2d2_bt_open_channel(this->addr_);
    return (this->rfcommChannelRef_ != NULL);
}

void BTTransport::devWrite(bool requiresResponse, uint8_t * buf, int buf_size, uint8_t * re_buf, int re_buf_size) {
    uint8_t bf = buf_size;
    uint8_t header[] = {bf, 0x00};
    uint8_t outBuf[2 + buf_size];
    memcpy(outBuf, header, sizeof(header));
    memcpy(outBuf + 2, buf, buf_size);


    size_t actual_re_buf_size;

    r2d2_bt_write(this->rfcommChannelRef_, outBuf, sizeof(outBuf), re_buf, &actual_re_buf_size);
}

void BTTransport::devRead(uint8_t * buf, int buf_size) {
}

std::vector<Brick *>* BTBrickManager::list() {

    // List all the NXT devices
    std::vector<Brick*>* v = new std::vector<Brick*>();

    r2d2_bt_scan(addBTDeviceToList, v);

    return v;
}
