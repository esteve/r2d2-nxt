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

namespace r2d2 {
class LSMessage {
private:
    std::stringstream sstream_;

public:
    void add_u8(uint8_t v);

    std::string get_value();
};

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

enum class SensorType : uint8_t {
    ACTIVE_LIGHT_SENSOR = 0x05,
    PASSIVE_LIGHT_SENSOR = 0x06,
    TOUCH_SENSOR = 0x01,
    SONAR_SENSOR = 0x0B,
    NULL_SENSOR = 0x00
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
    Sensor(Comm *comm, SensorPort port, SensorType, Mode);

    virtual int getValue() = 0;
};

class NullSensor : public Sensor {
public:
    NullSensor(SensorPort port) : Sensor(nullptr, port, SensorType::NULL_SENSOR, Mode::RAW) { };
    int getValue() { return 0; };
};

class AnalogSensor : public Sensor {
public:
    AnalogSensor(Comm *comm, SensorPort port, SensorType type, Mode mode) : Sensor(comm, port, type, mode) { };
    int getValue();
};

class TouchSensor : public AnalogSensor {
public:
    TouchSensor(Comm *comm, SensorPort port) : AnalogSensor(comm, port, SensorType::TOUCH_SENSOR, Mode::BOOLEAN) { };
};

class ActiveLightSensor : public AnalogSensor {
public:
    ActiveLightSensor(Comm *comm, SensorPort port) : AnalogSensor(comm, port, SensorType::ACTIVE_LIGHT_SENSOR, Mode::RAW) { };
};

class PassiveLightSensor : public AnalogSensor {
public:
    PassiveLightSensor(Comm *comm, SensorPort port) : AnalogSensor(comm, port, SensorType::PASSIVE_LIGHT_SENSOR, Mode::RAW) { };
};

class DigitalSensor : public Sensor {
public:
    DigitalSensor(Comm *comm, SensorPort port) : Sensor(comm, port, SensorType::SONAR_SENSOR, Mode::RAW) { };
protected:
    int lsGetStatus(uint8_t *);
    void lsRead(uint8_t *);
    void lsWrite(LSMessage&, uint8_t *, size_t);
};

class SonarSensor : public DigitalSensor {
public:
    SonarSensor(Comm *comm, SensorPort port) : DigitalSensor(comm, port) { };
    int getValue();
};
}
#endif
