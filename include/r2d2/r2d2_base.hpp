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

/**
 \mainpage R2D2-NXT

 \section intro_sec Introduction

 r2d2-nxt is a C++ library for programming Lego NXT robots using either the USB or the Bluetooth interfaces. It supports
 Linux, Microsoft Windows (tested on 7) and Mac OS X.

 \section requirements Requirements

 CMake (http://cmake.org) is required for building from sources. r2d2-nxt uses certain C++11 features, so you'll need a relatively recent C++ compiler.
 r2d2-nxt has been tested on the following platforms:

 - GCC 4.6 on Linux
 - Clang 3.2 on Linux
 - Clang 4.2 on Mac OSX
 - MSVC 10.0 on Microsoft Windows 7.

 You will also need to have libusb-1.0 installed (http://libusb.org). If you want to generate the documentation, you'll also have to install Doxygen
 (http://doxygen.org) and Graphviz (http://graphviz.org)

 \section install_sec Installation

 Run <tt>cmake .</tt> to generate the build rules for your platform (make on Linux and Mac OSX, nmake on Windows), afterwards you may use <tt>make install</tt>
 on Linux and Mac OSX, or <tt>nmake install</tt> on Microsoft Windows, to install r2d2-nxt to its proper location. You can configure the destination path
 (\c CMAKE_INSTALL_PREFIX) with <tt>ccmake .</tt> or <tt>cmake-gui</tt>

 \section examples Examples

 You may find several examples in the \c examples/ directory, that use both USB and Bluetooth.
*/
#ifndef R2D2_BASE_HPP
#define R2D2_BASE_HPP

#include <cstdint>
#include <sstream>
#include <vector>

#include <r2d2/comm.hpp>
#include <r2d2/motors.hpp>
#include <r2d2/sensors.hpp>

#define NXT_BLUETOOTH_ADDRESS "00:16:53"

//! R2D2-NXT namespace
namespace r2d2 {

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

    GET_FIRMWARE_VERSION = 0x88,

    GET_DEVICE_INFO = 0x9B
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

/**
A configured NXT brick, with the sensors and motors layout already set up.
*/
class NXT;

/**
An unconfigured NXT brick, without any sensors or motors attached.

An unconfigured NXT brick can issue system commands such as Brick::getFirmware
or Brick::getDeviceInfo, but it doesn't have any sensors or motors attached,
instead use Brick::configure to setup an NXT layout.

*/
class Brick {
private:
    Comm *comm_;
    bool halted;
    SensorFactory sensorFactory;
    MotorFactory motorFactory;

public:
    Brick(Comm *comm);

    /**
    Open the underlying transport.

    @return A bool whether the transport was successfully opened.
    */
    bool open();

    /**
    Obtain the NXT brick name.

    @return std::string A std::string with the name of the NXT brick.
    */
    std::string getName();

    /**
    Obtain the NXT firmware version.

    @return A double with the firmware version of the NXT brick.
    */
    double getFirmwareVersion();

    /**
    Obtain information about the NXT brick.

    @param uint8_t* A uint8_t* buffer to store the device information.
    @param size_t A size_t with the size of the buffer.
    */
    void getDeviceInfo(uint8_t*, size_t);

    /**
    Stop all motors, use it to guarantee that the brick will be in a safe
    state.
    */
    void halt();

    /**
    Whether the NXT brick is stopped.

    @return A bool with whether the NXT is stopped.
    */
    bool isHalted() const;

    /**
    Make the NXT brick play a sound.

    @param frequency The frequency in Hz to play in the NXT brick.
    @param duration The duration in milliseconds to play the tone.
    */
    void playTone(uint16_t frequency, uint16_t duration);

    /**
    Interrupt any sound currently playing in the NXT brick.
    */
    void stopSound();

    /**
    Configure a Brick with a Sensor and Motor layout.

    @param sensor1 The Sensor in the port 1.
    @param sensor2 The Sensor in the port 2.
    @param sensor3 The Sensor in the port 3.
    @param sensor4 The Sensor in the port 4.
    @param motorA The Motor in the port A.
    @param motorB The Motor in the port B.
    @param motorC The Motor in the port C.

    @return A fully configured NXT object.
    */
    NXT* configure(SensorType sensor1, SensorType sensor2,
                   SensorType sensor3, SensorType sensor4,
                   MotorType motorA, MotorType motorB, MotorType motorC);
};

class NXT {
private:
    Brick *brick_;
    Comm *comm_;
    bool halted;
    Sensor *sensorPort1;
    Sensor *sensorPort2;
    Sensor *sensorPort3;
    Sensor *sensorPort4;

    Motor *motorPortA;
    Motor *motorPortB;
    Motor *motorPortC;

public:

    NXT(Brick *, Comm *, Sensor *, Sensor *, Sensor *, Sensor *, Motor *, Motor *, Motor *);

    /**
    Return the Sensor object at the given port.

    @param port One of the SensorPort enum values.
    @return A Sensor instance.
    */
    Sensor * sensorPort(SensorPort port) {
        switch(port) {
            case SensorPort::IN_1:
                return sensorPort1;

            case SensorPort::IN_2:
                return sensorPort2;

            case SensorPort::IN_3:
                return sensorPort3;

            case SensorPort::IN_4:
                return sensorPort4;
            default:
                return nullptr;
        }
    };

    /**
    Return the Motor object at the given port.

    @param port One of the MotorPort enum values.
    @return A Motor instance.
    */
    Motor *motorPort(MotorPort port) {
        switch(port) {
            case MotorPort::OUT_A:
                return motorPortA;

            case MotorPort::OUT_B:
                return motorPortB;

            case MotorPort::OUT_C:
                return motorPortC;
            default:
                return nullptr;
        }
    };

    /**
    @see Brick::getName()
    */
    std::string getName();

    /**
    @see Brick::getName()
    */
    double getFirmwareVersion();

    /**
    @see Brick::getDeviceInfo()
    */
    void getDeviceInfo(uint8_t*, size_t);

    /**
    @see Brick::halt()
    */
    void halt();

    /**
    @see Brick::isHalted()
    */
    bool isHalted() const;

    /**
    @see Brick::playTone()
    */
    void playTone(uint16_t frequency, uint16_t duration);

    /**
    @see Brick::stopSound()
    */
    void stopSound();
};

class BrickManager {
public:
    /**
    Find all the NXT bricks in the underlying transport.

    @return A std::vector of Brick.
    */
    virtual std::vector<Brick *>* list() = 0;
};
}
#endif
