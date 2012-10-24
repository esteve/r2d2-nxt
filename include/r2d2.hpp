#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <cstring>

#include <libusb-1.0/libusb.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <stdlib.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <sstream>
#include <boost/format.hpp>
#include <iomanip>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#define NXT_BLUETOOTH_ADDRESS "00:16:53"

class Mode {
public:
    static const int RAW = 0x00;
    static const int BOOLEAN = 0x20;
};

class Message {
private:
    std::stringstream sstream_;
    bool requiresResponse_;
    bool isDirect_;
    uint8_t type_;

public:
    static const int SET_INPUT_MODE = 0x05;
    static const int LOWSPEED_9V = 0x0B;


    static const int LS_GET_STATUS = 0x0E;
    static const int LS_WRITE = 0x0F;
    static const int LS_READ = 0x10;

    static const int GET_VALUE = 0x07;

    static const int TOUCH = 0x01;

    static const int START_MOTOR = 0x04;
    static const int STOP_MOTOR = 0x04;

    Message(bool isDirect, bool requiresResponse);

    bool requiresResponse() const;

    bool isDirect() const;

    void add_u8(uint8_t v);

    void add_s8(int8_t v);

    void add_string(size_t n_bytes, const std::string& v);

    std::string get_value();
};

class Comm {
public:
    virtual void devWrite(uint8_t *, int) = 0;
    virtual void devRead(uint8_t *, int) = 0;
    virtual bool open() = 0;
};

class Command {
    virtual uint8_t * serialize() = 0;
};

class SetTouchSensor : public Command {
private:
    uint8_t port_;

public:
    SetTouchSensor(uint8_t port) {
        this->port_ = port;
    }

    uint8_t * serialize() {
        uint8_t *buffer = new uint8_t[5] {0x00, 0x05, this->port_, 0x01, 0x20};
        return buffer;
    }
};

class Sensor;

class SonarSensor;

class Motor;

class NXT {
private:
    Comm *comm_;
    bool halted;

public:
    static const int OUT_A = 0;
    static const int OUT_B = 1;
    static const int OUT_C = 2;

    static const int IN_1 = 0;
    static const int IN_2 = 1;
    static const int IN_3 = 2;
    static const int IN_4 = 3;

    NXT(Comm *comm);
    void sendSystemCommand(bool, int8_t *, size_t, uint8_t *, size_t);

    void sendDirectCommand(bool, int8_t *, size_t, uint8_t *, size_t);

    bool open();

    std::string getName();

    double getFirmwareVersion();

    void getDeviceInfo(uint8_t*, size_t);

    Sensor *makeTouch(uint8_t);

    SonarSensor *makeSonar(uint8_t);

    Motor *makeMotor(uint8_t);

    void submitDirectCommand(Command *);

    void halt();

    bool isHalted() const;
};

class Motor {
private:
    NXT *nxt_;
    uint8_t port_;

public:
    Motor(NXT *nxt, uint8_t port);

    void setForward(uint8_t power);

    void setReverse(uint8_t power);

    void stop(bool brake);
};

class Sensor {
private:
    NXT *nxt_;
    uint8_t port_;

public:
    Sensor(NXT *nxt, uint8_t port);

    int getValue();
};

class SonarSensor : Sensor {
private:
    NXT *nxt_;
    uint8_t port_;

public:
    SonarSensor(NXT *nxt, uint8_t port);

    int lsGetStatus(uint8_t *);
    void lsRead(uint8_t *);
    void lsWrite(const std::string&, uint8_t *, size_t);
    int getSonarValue();
};


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

class USBComm : public Comm {
private:
    libusb_device *usb_dev_;
    libusb_device_handle *pUSBHandle_;
    int ucEpOut_;
    int ucEpIn_;
    libusb_context *ctx_;
    static const int TIMEOUT = 500;

    boost::mutex io_mutex;
//0x0BB8;

public:

    bool open();

    void devWrite(uint8_t * buf, int buf_size);

    void devRead(uint8_t * buf, int buf_size);

    USBComm(libusb_context *ctx, libusb_device *usb_dev);

    ~USBComm();
};

class USBNXTManager {
    static const int NXT_VENDOR_ID = 0x0694;
    static const int NXT_PRODUCT_ID = 0x0002;

public:
    std::vector<NXT *>* list();
};

class BTNXTManager {
    static const int NXT_VENDOR_ID = 0x0694;
    static const int NXT_PRODUCT_ID = 0x0002;

public:
    std::vector<NXT *>* list();
};
