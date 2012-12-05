#include <vector>
#include <cstring>
#include <r2d2.hpp>
#include <r2d2/bluetooth_bridge.h>
#include <r2d2/bluetooth.hpp>

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
    NXT *nxt = new NXT(new Comm(transport));
    std::vector<NXT *> *v = static_cast< std::vector<NXT *> *>(arg);
    v->push_back(nxt);
}

bool BTTransport::open() {
    this->sock_ = r2d2_bt_create_socket();
    int status = r2d2_bt_connect_socket(this->sock_, this->addr_);
    return (status == 0);
}

void BTTransport::devWrite(uint8_t * buf, int buf_size) {
    uint8_t bf = buf_size;
    uint8_t header[] = {bf, 0x00};
    uint8_t outBuf[2 + buf_size];
    memcpy(outBuf, header, sizeof(header));
    memcpy(outBuf + 2, buf, buf_size);
    write(this->sock_, outBuf, sizeof(outBuf));
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

std::vector<NXT *>* BTNXTManager::list() {

    // List all the NXT devices
    std::vector<NXT*>* v = new std::vector<NXT*>();

    r2d2_bt_scan(addBTDeviceToList, v);

    return v;
}
