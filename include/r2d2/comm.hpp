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

#ifndef R2D2_COMM_HPP
#define R2D2_COMM_HPP

#include <cstdint>

class Transport {
public:
    virtual void devWrite(uint8_t *, int) = 0;
    virtual void devRead(unsigned char *, int) = 0;
    virtual bool open() = 0;
};

class Comm {
private:
    Transport *transport_;
public:
    Comm(Transport *transport) : transport_(transport) {}

    void sendSystemCommand(bool, int8_t *, size_t, uint8_t *, size_t);

    void sendDirectCommand(bool, int8_t *, size_t, unsigned char *, size_t);

    bool open();
};
#endif
