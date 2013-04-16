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

#ifndef R2D2_MOTORS_HPP
#define R2D2_MOTORS_HPP

#include <cstdint>

#include <r2d2/comm.hpp>

namespace r2d2 {
enum class MotorPort : uint8_t {
    OUT_A = 0,
    OUT_B = 1,
    OUT_C = 2
};

enum class MotorType {
     STANDARD_MOTOR,
     NULL_MOTOR
};

class Motor {
public:
    virtual void setForward(uint8_t power) = 0;

    virtual void setReverse(uint8_t power) = 0;

    virtual void stop(bool brake) = 0;

    virtual void resetRotationCount(bool relative) = 0;

    virtual int getRotationCount() = 0;
};

class MotorFactory {
public:
    Motor *makeMotor(MotorType, MotorPort, Comm *);
};

class NullMotor : public Motor {
    void setForward(uint8_t power) { }

    void setReverse(uint8_t power) { }

    void stop(bool brake) { }

    void resetRotationCount(bool relative) { }

    int getRotationCount() { return 0; }
};

class StandardMotor : public Motor {
private:
    Comm *comm_;
    MotorPort port_;

public:
    StandardMotor(Comm *comm, MotorPort port);

    void setForward(uint8_t power);

    void setReverse(uint8_t power);

    void stop(bool brake);

    void resetRotationCount(bool relative);

    int getRotationCount();
};
}
#endif
