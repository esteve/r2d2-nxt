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
