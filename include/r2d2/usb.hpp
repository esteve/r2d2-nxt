/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef R2D2_USB_HPP
#define R2D2_USB_HPP

#include <vector>
#include <cstdint>

#include <libusb-1.0/libusb.h>

#include <boost/thread/mutex.hpp>

#include <r2d2.hpp>

#define NXT_BLUETOOTH_ADDRESS "00:16:53"

class USBTransport : public Transport {
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

    void devRead(unsigned char * buf, int buf_size);

    USBTransport(libusb_context *ctx, libusb_device *usb_dev);

    ~USBTransport();
};

class USBNXTManager : public NXTManager {
    static const int NXT_VENDOR_ID = 0x0694;
    static const int NXT_PRODUCT_ID = 0x0002;

public:
    std::vector<NXT *>* list();
};
#endif
