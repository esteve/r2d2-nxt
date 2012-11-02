#ifndef R2D2_USB_HPP
#define R2D2_USB_HPP

#include <vector>
#include <cstdint>

#include <libusb-1.0/libusb.h>

#include <mutex>

#include <r2d2.hpp>

#define NXT_BLUETOOTH_ADDRESS "00:16:53"

class USBComm : public Comm {
private:
    libusb_device *usb_dev_;
    libusb_device_handle *pUSBHandle_;
    int ucEpOut_;
    int ucEpIn_;
    libusb_context *ctx_;
    static const int TIMEOUT = 500;

    std::mutex io_mutex;
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
#endif
