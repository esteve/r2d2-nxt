#ifndef R2D2_BLUETOOTH_HPP
#define R2D2_BLUETOOTH_HPP
#include <vector>
#include <cstdint>

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <stdlib.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include <r2d2.hpp>

#define NXT_BLUETOOTH_ADDRESS "00:16:53"

class BTComm : public Comm {
private:
    struct sockaddr_rc addr_;
    int sock_;
public:
    BTComm(struct sockaddr_rc addr);

    bool open();

    void devWrite(uint8_t * buf, int buf_size);

    void devRead(uint8_t * buf, int buf_size);
};

class BTNXTManager {
    static const int NXT_VENDOR_ID = 0x0694;
    static const int NXT_PRODUCT_ID = 0x0002;

public:
    std::vector<NXT *>* list();
};
#endif
