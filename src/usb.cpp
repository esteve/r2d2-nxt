#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>

#include <r2d2/usb.hpp>

namespace r2d2 {

bool USBTransport::open() {

    int nEp = 0;

    if (this->usb_dev_ == 0) {
        std::cerr << "Device not found!" << std::endl;
        return false;
    }

    int r = libusb_open(this->usb_dev_, &(this->pUSBHandle_));

    if (r != 0) {
        std::cerr << "Not able to claim the USB device" << std::endl;
        return false;
    } else {
        libusb_config_descriptor *config;
        libusb_get_config_descriptor(this->usb_dev_, 0, &config);
        if (config->interface) {
            if (config->interface->altsetting) {
                r = libusb_claim_interface(this->pUSBHandle_, config->interface->altsetting->bInterfaceNumber);
                if ((nEp = config->interface->altsetting->bNumEndpoints)) {
                    if (config->interface->altsetting->endpoint) {
                        this->ucEpIn_ = (config->interface->altsetting->endpoint[0].bEndpointAddress);
                        if (nEp > 1) {
                            this->ucEpOut_ = (config->interface->altsetting->endpoint[1].bEndpointAddress);
                        }

                    }
                }
            }
        }
    }
    return true;
}

void USBTransport::devWrite(bool requiresResponse, uint8_t * buf, int buf_size, unsigned char * re_buf, int re_buf_size) {
    std::lock_guard<std::mutex> lock(this->io_mutex);
    if (this->pUSBHandle_) {
        int actual_length;
        int r = libusb_bulk_transfer(this->pUSBHandle_, this->ucEpIn_, buf, buf_size, &actual_length, TIMEOUT);
        if (r == 0 && actual_length == buf_size) {
        } else {
            std::cerr << "ERROR WRITING" << std::endl;
            std::cerr << "READ: " << actual_length << std::endl;
            std::cerr << "BUF LEN: " << buf_size << std::endl;
            std::cerr << "RES: " << r << std::endl;
        }

        if(requiresResponse) {
            int re_actual_length;
            int r2 = libusb_bulk_transfer(this->pUSBHandle_, this->ucEpOut_, re_buf, re_buf_size, &re_actual_length, TIMEOUT);
            if (r2 == 0 && re_actual_length <= re_buf_size) {
            } else {
                std::cerr << "ERROR READING" << std::endl;
                std::cerr << "READ: " << re_actual_length << std::endl;
                std::cerr << "BUF LEN: " << re_buf_size << std::endl;
                std::cerr << "RES: " << r2 << std::endl;
            }
        }
    }
}

USBTransport::USBTransport(libusb_context *ctx, libusb_device *usb_dev) {
    this->ctx_ = ctx;
    this->usb_dev_ = usb_dev;
    this->pUSBHandle_ = nullptr;
}

USBTransport::~USBTransport() {
    libusb_unref_device(this->usb_dev_);
    libusb_close(this->pUSBHandle_);
}

std::vector<Brick *>* USBBrickManager::list() {

    // List all the NXT devices
    std::vector<Brick*>* v = new std::vector<Brick*>();

    libusb_device **devs;
    libusb_context *ctx = NULL;
    libusb_device *dev;

    int r;
    ssize_t cnt; //holding number of devices in list

    r = libusb_init(&ctx);

    if (r < 0) {
        std::cout<<"Init Error "<<r<<std::endl; //there was an error
        return v;
    }

    libusb_set_debug(ctx, 1); //set verbosity level to 1

    cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
    if (cnt < 0) {
        std::cout<<"Get Device Error"<<std::endl; //there was an error
        return v;
    }

    std::cout<<cnt<<" Devices in list."<<std::endl; //print total number of usb devices
    ssize_t i; //for iterating through the list
    for (i = 0; i < cnt; i++) {
        libusb_device_descriptor desc;
        int r = libusb_get_device_descriptor(devs[i], &desc);
        if (r < 0) {
            std::cout<<"failed to get device descriptor"<<std::endl;
            continue;
        }


        if (desc.idVendor == NXT_VENDOR_ID && desc.idProduct == NXT_PRODUCT_ID) {
            libusb_device_handle *devh = NULL;
            libusb_open(devs[i], &devh);
            libusb_reset_device(devh);
            libusb_close(devh);
        }
    }


    // Get the list of all the USB devices
    cnt = libusb_get_device_list(ctx, &devs);
    if (cnt < 0) {
        std::cout << "Error getting device" <<std::endl;
        return v;
    }

    std::cout<<cnt<<" Devices in list."<<std::endl; //print total number of usb devices
    for (i = 0; i < cnt; i++) {
        libusb_device_descriptor desc;
        int r = libusb_get_device_descriptor(devs[i], &desc);
        if (r < 0) {
            std::cout<<"failed to get device descriptor"<<std::endl;
            continue;
        }

        if (desc.idVendor == NXT_VENDOR_ID && desc.idProduct == NXT_PRODUCT_ID) {
            dev = libusb_ref_device(devs[i]);
            USBTransport *transport = new USBTransport(ctx, dev);
            Brick *brick = new Brick(new Comm(transport));
            v->push_back(brick);
        }
    }
    libusb_free_device_list(devs, 1); //free the list, unref the devices in it
//        libusb_exit(ctx); //close the session
    return v;
}
}
