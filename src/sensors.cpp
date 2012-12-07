#include <r2d2.hpp>
#include <r2d2/sensors.hpp>

#include <cstring>

#include <iostream>

Sensor::Sensor(Comm *comm, SensorPort port, SensorType type, Mode mode) {
    this->comm_ = comm;
    this->port_ = port;
    this->type_ = type;
    this->mode_ = mode;

    Message msg(true, false);
    msg.add_u8(uint8_t(Opcode::SET_INPUT_MODE));
    msg.add_u8(uint8_t(port));
    msg.add_u8(uint8_t(this->type_));
    msg.add_u8(uint8_t(this->mode_));

    std::string out = msg.get_value();

    if (nullptr != this->getComm()) { // TODO HACK
        this->getComm()->sendDirectCommand( false, (int8_t *)out.c_str(), out.size(), NULL, 0);
    }
}

Comm* Sensor::getComm() {
    return this->comm_;
}

SensorPort Sensor::getPort() {
    return this->port_;
}

int AnalogSensor::getValue() {
    Message msg(true, true);
    msg.add_u8(uint8_t(Opcode::GET_VALUE));
    msg.add_u8(uint8_t(this->getPort()));

    std::string out = msg.get_value();

    unsigned char responseBuffer[16];

    memset(responseBuffer, 1, sizeof(responseBuffer));

    this->getComm()->sendDirectCommand(true, (int8_t *)out.c_str(), out.size(), responseBuffer, sizeof(responseBuffer));

    std::string s(responseBuffer, responseBuffer + 16);

    Message response(s);

    response.parse_u8(); // 2 check status

    response.parse_u8(); // 3 port

    response.parse_u8(); // 4 valid

    response.parse_u8(); // 5 calibrated

    response.parse_u8(); // 6 sensor_type
    response.parse_u8(); // 7 sensor_mode
    response.parse_u16(); // 8,9 raw_ad_value
    response.parse_u16(); // 10,11 normalized_ad_value

    int result = response.parse_s16(); // 12,13 scaled value

    response.parse_s16(); // 14,15 calibrated value

    return result;
}

int DigitalSensor::lsGetStatus(uint8_t *outbuf) {
    Message msg(true, true);
    msg.add_u8(uint8_t(Opcode::LS_GET_STATUS));
    msg.add_u8(uint8_t(this->getPort()));

    std::string out = msg.get_value();

    unsigned char responseBuffer[4];

    memset(responseBuffer, 1, sizeof(responseBuffer));

    this->getComm()->sendDirectCommand(true, (int8_t *)out.c_str(), out.size(), responseBuffer, sizeof(responseBuffer));

    std::copy(responseBuffer, responseBuffer + sizeof(responseBuffer), outbuf);

    return static_cast<int>(responseBuffer[3]);
}

void DigitalSensor::lsRead(uint8_t *outbuf) {
    Message msg(true, true);
    msg.add_u8(uint8_t(Opcode::LS_READ));
    msg.add_u8(uint8_t(this->getPort()));

    std::string out = msg.get_value();

    unsigned char responseBuffer[20];

    memset(responseBuffer, 1, sizeof(responseBuffer));

    this->getComm()->sendDirectCommand(true, (int8_t *)out.c_str(), out.size(), responseBuffer, sizeof(responseBuffer));

    std::copy(responseBuffer, responseBuffer + sizeof(responseBuffer), outbuf);
}

void DigitalSensor::lsWrite(const std::string& indata, uint8_t *outBuf, size_t outSize) {
    Message msg(true, true);
    msg.add_u8(uint8_t(Opcode::LS_WRITE));
    msg.add_u8(uint8_t(this->getPort()));
    msg.add_u8(indata.size());
    msg.add_u8(outSize);
    msg.add_string(indata.size(), indata);

    std::string tosend = msg.get_value();

    unsigned char responseBuffer[3];

    memset(responseBuffer, 1, sizeof(responseBuffer));

    this->getComm()->sendDirectCommand(true, (int8_t *)(tosend.c_str()), tosend.size(), responseBuffer, sizeof(responseBuffer));

    std::memcpy(outBuf, responseBuffer, outSize * sizeof(uint8_t));
}

int SonarSensor::getValue() {
    Message msg(true, true);
    msg.add_u8(0x02); // I2C_DEV
    msg.add_u8(0x41); // COMMAND
    msg.add_u8(0x02); // CONTINUOUS

    std::string out = msg.get_value();

    uint8_t outbuf2[66];
    memset(outbuf2, 1, sizeof(outbuf2));
    this->lsWrite(out, outbuf2, 0x00);

    while (1) {
        Message msg(true, true);
        msg.add_u8(0x02); // I2C_DEV
        msg.add_u8(0x42); // READ VALUE FROM I2C
        std::string out2 = msg.get_value();

        uint8_t outbuf3[66];
        memset(outbuf3, 1, sizeof(outbuf3));
        this->lsWrite(out2, outbuf3, 1);

        uint8_t outbuf4[4];
        memset(outbuf4, 1, sizeof(outbuf4));
        this->lsGetStatus(outbuf4);
        if (outbuf4[3] >= 1) { // 1 bytes
            break;
        }
    }

    uint8_t outbuf5[20];
    memset(outbuf5, 1, sizeof(outbuf5));
    this->lsRead(outbuf5);
    if (outbuf5[2] == 0) {
        return outbuf5[4];
    } else {
        return -1;
    }
}

Sensor * SensorFactory::makeSensor(SensorType type, SensorPort port, Comm *comm) {
    Sensor *sensor;

    switch(type) {

        case SensorType::ACTIVE_LIGHT_SENSOR:
            sensor = new ActiveLightSensor(comm, port);
            break;
        case SensorType::PASSIVE_LIGHT_SENSOR:
            sensor = new PassiveLightSensor(comm, port);
            break;
        case SensorType::TOUCH_SENSOR:
            sensor = new TouchSensor(comm, port);
            break;
        case SensorType::SONAR_SENSOR:
            sensor = new SonarSensor(comm, port);
            break;
        default:
            sensor = new NullSensor(port);
    }

    return sensor;
}
