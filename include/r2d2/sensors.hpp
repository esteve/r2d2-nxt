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

#ifndef R2D2_SENSORS_HPP
#define R2D2_SENSORS_HPP

#include <cstdint>

#include <r2d2/comm.hpp>

#if !defined(nullptr)
// nullptr implementation taken from http://stackoverflow.com/questions/2419800/can-nullptr-be-emulated-in-gcc
const                        // this is a const object...
class {
public:
  template<class T>          // convertible to any type
    operator T*() const      // of null non-member
    { return 0; }            // pointer...
  template<class C, class T> // or any type of null
    operator T C::*() const  // member pointer...
    { return 0; }
private:
  void operator&() const;    // whose address can't be taken
} nullptr = {};              // and whose name is nullptr
#endif

enum class Mode : uint8_t {
    RAW = 0x00,
    BOOLEAN = 0x20,
    PCT_FULL_SCALE = 0x80
};

enum class SensorPort : uint8_t {
    IN_1 = 0,
    IN_2 = 1,
    IN_3 = 2,
    IN_4 = 3
};

enum class SensorType {
    ACTIVE_LIGHT_SENSOR,
    PASSIVE_LIGHT_SENSOR,
    TOUCH_SENSOR,
    SONAR_SENSOR,
    NULL_SENSOR
};

class Sensor;

class SonarSensor;
class TouchSensor;
class ActiveLightSensor;
class PassiveLightSensor;

class SensorFactory {
public:
    Sensor *makeSensor(SensorType, SensorPort, Comm *);
};

class Sensor {
private:
    Comm *comm_;
    SensorPort port_;
    SensorType type_;
    Mode mode_;

protected:
    Comm* getComm();
    SensorPort getPort();

public:
    Sensor(Comm *comm, SensorPort port);

    virtual int getValue() = 0;
};

class NullSensor : public Sensor {
public:
    NullSensor(SensorPort port) : Sensor(nullptr, port) { };
    int getValue() { return 0; };
};

class AnalogSensor : public Sensor {
public:
    AnalogSensor(Comm *comm, SensorPort port) : Sensor(comm, port) { };
    int getValue();
};

class TouchSensor : public AnalogSensor {
public:
    TouchSensor(Comm *comm, SensorPort port) : AnalogSensor(comm, port) { };
    static const SensorType type_ = SensorType::TOUCH_SENSOR;
    static const Mode mode_ = Mode::BOOLEAN;
};

class ActiveLightSensor : public AnalogSensor {
public:
    ActiveLightSensor(Comm *comm, SensorPort port) : AnalogSensor(comm, port) { };
    static const SensorType type_ = SensorType::ACTIVE_LIGHT_SENSOR;
    static const Mode mode_ = Mode::RAW;
};

class PassiveLightSensor : public AnalogSensor {
public:
    PassiveLightSensor(Comm *comm, SensorPort port) : AnalogSensor(comm, port) { };
    static const SensorType type_ = SensorType::PASSIVE_LIGHT_SENSOR;
    static const Mode mode_ = Mode::RAW;
};

class DigitalSensor : public Sensor {
public:
    DigitalSensor(Comm *comm, SensorPort port) : Sensor(comm, port) { };
protected:
    int lsGetStatus(uint8_t *);
    void lsRead(uint8_t *);
    void lsWrite(const std::string&, uint8_t *, size_t);
};

class SonarSensor : public DigitalSensor {
public:
    SonarSensor(Comm *comm, SensorPort port) : DigitalSensor(comm, port) { };
    int getValue();
    static const SensorType type_ = SensorType::SONAR_SENSOR;
    static const Mode mode_ = Mode::RAW;
};
#endif
