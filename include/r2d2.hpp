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

#define NXT_BLUETOOTH_ADDRESS "00:16:53"

class Comm {
public:
    virtual void devWrite(uint8_t *, int) = 0;
    virtual void devRead(unsigned char *, int) = 0;
    virtual bool open() = 0;
};

class Mode {
public:
    static const int RAW = 0x00;
    static const int BOOLEAN = 0x20;
    static const int PCT_FULL_SCALE = 0x80;
};

class Message {
private:
    std::stringstream sstream_;
    bool requiresResponse_;
    bool isDirect_;
    uint8_t type_;
    uint8_t opcode_;

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
    static const int RESET_ROTATION_COUNT = 0x0A;
    static const int GET_ROTATION_COUNT = 0x06;

    static const int PLAY_TONE = 0x03;
    static const int STOP_SOUND = 0x0C;

    static const int ACTIVE_LIGHT = 0x05;
    static const int PASSIVE_LIGHT = 0x06;

    static const int GET_FIRMWARE_VERSION = 0x88;

    Message(bool isDirect, bool requiresResponse);

    Message(const std::string &s);

    bool requiresResponse() const;

    bool isDirect() const;

    void add_u8(uint8_t v);

    void add_s8(int8_t v);

    void add_string(size_t n_bytes, const std::string& v);

    std::string get_value();

    uint32_t parse_u32();

    int32_t parse_s32();

    uint16_t parse_u16();

    int16_t parse_s16();

    uint8_t parse_u8();

    int8_t parse_s8();
};

class Sensor;

class SonarSensor;
class TouchSensor;
class LightSensor;

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

    void sendDirectCommand(bool, int8_t *, size_t, unsigned char *, size_t);

    bool open();

    std::string getName();

    double getFirmwareVersion();

    void getDeviceInfo(uint8_t*, size_t);

    TouchSensor *makeTouch(uint8_t);

    SonarSensor *makeSonar(uint8_t);

    LightSensor *makeLight(uint8_t, bool);

    Motor *makeMotor(uint8_t);

    void halt();

    bool isHalted() const;

    void playTone(int frequency, int duration);
    void stopSound();
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

    void resetRotationCount(bool relative);

    int getRotationCount();
};

class Sensor {
private:
    NXT *nxt_;
    uint8_t port_;

protected:
    NXT* getNXT();
    uint8_t getPort();

public:
    Sensor(NXT *nxt, uint8_t port);

    virtual int getValue() = 0;
};

class AnalogSensor : public Sensor {
public:
    AnalogSensor(NXT *nxt, uint8_t port) : Sensor(nxt, port) { };
    int getValue();
};

class TouchSensor : public AnalogSensor {
public:
    TouchSensor(NXT *nxt, uint8_t port) : AnalogSensor(nxt, port) { };
};

class LightSensor : public AnalogSensor {
public:
    LightSensor(NXT *nxt, uint8_t port) : AnalogSensor(nxt, port) { };
};

class DigitalSensor : public Sensor {
public:
    DigitalSensor(NXT *nxt, uint8_t port) : Sensor(nxt, port) { };
protected:
    int lsGetStatus(uint8_t *);
    void lsRead(uint8_t *);
    void lsWrite(const std::string&, uint8_t *, size_t);
};

class SonarSensor : public DigitalSensor {
public:
    SonarSensor(NXT *nxt, uint8_t port) : DigitalSensor(nxt, port) { };
    int getValue();
};

class NXTManager {
public:
    virtual std::vector<NXT *>* list() = 0;
};
#endif
