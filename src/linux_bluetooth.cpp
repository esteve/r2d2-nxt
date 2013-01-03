#include <vector>
#include <cstring>
#include <r2d2.hpp>
#include <r2d2/bluetooth.hpp>
#include <r2d2/linux_bluetooth.hpp>
#include <r2d2/linux_bluetooth_bridge.h>

BTTransport::BTTransport(void *addr) {
    this->addr_ = addr;
    // this is actually a pointer to a struct sockaddr_rc
    // but bluetooth.h can't be included if using std=c++0x
}

BTTransport::~BTTransport() {
    free(this->addr_);
}

void addBTDeviceToList(void *addr, void *arg) {
    BTTransport *transport = new BTTransport(addr);
    Brick *brick = new Brick(new Comm(transport));
    std::vector<Brick *> *v = static_cast< std::vector<Brick *> *>(arg);
    v->push_back(brick);
}

bool BTTransport::open() {
    this->sock_ = r2d2_bt_create_socket();
    int status = r2d2_bt_connect_socket(this->sock_, this->addr_);
    return (status == 0);
}

void BTTransport::devWrite(bool requiresResponse, uint8_t* buf, int buf_size, uint8_t* re_buf, int re_buf_size) {
    uint8_t bf = buf_size;
    uint8_t header[] = {bf, 0x00};
    uint8_t outBuf[2 + buf_size];
    memcpy(outBuf, header, sizeof(header));
    memcpy(outBuf + 2, buf, buf_size);
    write(this->sock_, outBuf, sizeof(outBuf));

    if(requiresResponse) {
        char reply[64];
        memset(reply, 0, sizeof(reply));
 
        // read data from the client
        int bytes_read = read(this->sock_, reply, 2);
 
        if ( bytes_read > 0 ) {
            int replylength = reply[0] + (reply[1] * 256);
            bytes_read = read(this->sock_, reply, replylength);
            if (bytes_read == replylength) {
                memcpy(re_buf, reply, re_buf_size);
            }
        }
    }
}

void BTTransport::devRead(uint8_t * buf, int buf_size) {
    char reply[64];
    memset(reply, 0, sizeof(reply));

    // read data from the client
    int bytes_read = read(this->sock_, reply, 2);

    if ( bytes_read > 0 ) {
        int replylength = reply[0] + (reply[1] * 256);
        bytes_read = read(this->sock_, reply, replylength);
        if (bytes_read == replylength) {
            memcpy(buf, reply, buf_size);
        }
    }
}

std::vector<Brick *>* BTBrickManager::list() {

    // List all the NXT devices
    std::vector<Brick*>* v = new std::vector<Brick*>();

    r2d2_bt_scan(addBTDeviceToList, v);

    return v;
}
