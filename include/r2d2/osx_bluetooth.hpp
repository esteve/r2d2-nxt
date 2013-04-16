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

#ifndef R2D2_OSX_BLUETOOTH_HPP
#define R2D2_OSX_BLUETOOTH_HPP
#include <vector>
#include <r2d2/r2d2_base.hpp>
#include <CoreFoundation/CoreFoundation.h>
#include <IOBluetooth/Bluetooth.h>
#include <IOBluetooth/IOBluetoothUserLib.h>

namespace r2d2 {
class BTTransport : public Transport {
private:
    const BluetoothDeviceAddress *addr_;
    IOBluetoothRFCOMMChannelRef rfcommChannelRef_;
public:
    BTTransport(const BluetoothDeviceAddress *addr);
    ~BTTransport();

    bool open();

    void devWrite(bool requiresResponse, uint8_t *buf, int buf_size, uint8_t *re_buf, int re_buf_size);
};

void addBTDeviceToList(const BluetoothDeviceAddress *addr, void *arg);
}
#endif
