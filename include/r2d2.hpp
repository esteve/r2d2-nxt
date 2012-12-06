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

#ifndef R2D2_HPP
#define R2D2_HPP

#include <cstdint>
#include <sstream>
#include <vector>

#include <r2d2/comm.hpp>
#include <r2d2/motors.hpp>
#include <r2d2/sensors.hpp>

#define NXT_BLUETOOTH_ADDRESS "00:16:53"

enum class Opcode : uint8_t {
    SET_INPUT_MODE = 0x05,
    LOWSPEED_9V = 0x0B,


    LS_GET_STATUS = 0x0E,
    LS_WRITE = 0x0F,
    LS_READ = 0x10,

    GET_VALUE = 0x07,

    TOUCH = 0x01,

    START_MOTOR = 0x04,
    STOP_MOTOR = 0x04,
    RESET_ROTATION_COUNT = 0x0A,
    GET_ROTATION_COUNT = 0x06,

    PLAY_TONE = 0x03,
    STOP_SOUND = 0x0C,

    ACTIVE_LIGHT = 0x05,
    PASSIVE_LIGHT = 0x06,

    GET_FIRMWARE_VERSION = 0x88
};

class Message {
private:
    std::stringstream sstream_;
    bool requiresResponse_;
    bool isDirect_;
    uint8_t type_;
    uint8_t opcode_;

public:
    Message(bool isDirect, bool requiresResponse);

    Message(const std::string &s);

    bool requiresResponse() const;

    bool isDirect() const;

    void add_u8(uint8_t v);

    void add_s8(int8_t v);

    void add_u16(uint16_t v);

    void add_s16(int16_t v);

    void add_u32(uint32_t v);

    void add_s32(int32_t v);

    void add_string(size_t n_bytes, const std::string& v);

    std::string get_value();

    uint32_t parse_u32();

    int32_t parse_s32();

    uint16_t parse_u16();

    int16_t parse_s16();

    uint8_t parse_u8();

    int8_t parse_s8();
};

class ConfiguredNXT;

class NXT {
private:
    Comm *comm_;
    bool halted;
    SensorFactory sensorFactory;
    MotorFactory motorFactory;

public:
    NXT(Comm *comm);

    bool open();

    std::string getName();

    double getFirmwareVersion();

    void getDeviceInfo(uint8_t*, size_t);

    void halt();

    bool isHalted() const;

    void playTone(uint16_t frequency, uint16_t duration);
    void stopSound();

    ConfiguredNXT* configure(SensorType sensor1, SensorType sensor2,
        SensorType sensor3, SensorType sensor4,
        MotorType motorA, MotorType motorB, MotorType motorC);
};

class ConfiguredNXT {
private:
    NXT *nxt_;
    Comm *comm_;
    bool halted;
    std::vector<Sensor *> sensorPorts;
    std::vector<Motor *> motorPorts;

public:

    ConfiguredNXT(NXT *, Comm *, Sensor *, Sensor *, Sensor *, Sensor *, Motor *, Motor *, Motor *);

    Sensor * sensorPort(SensorPort port) {
        return this->sensorPorts.at(uint8_t(port));
    };

    Motor *motorPort(MotorPort port) {
        return this->motorPorts.at(uint8_t(port));
    };

    std::string getName();

    double getFirmwareVersion();

    void getDeviceInfo(uint8_t*, size_t);

    void halt();

    bool isHalted() const;

    void playTone(uint16_t frequency, uint16_t duration);
    void stopSound();
};

class NXTManager {
public:
    virtual std::vector<NXT *>* list() = 0;
};
#endif
