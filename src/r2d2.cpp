#include <r2d2.hpp>

#include <sstream>
#include <boost/format.hpp>
#include <iostream>
#include <iomanip>
#include <cstring>

Message::Message(bool isDirect, bool requiresResponse) {
    this->isDirect_ = isDirect;
    this->requiresResponse_ = requiresResponse;
}

Message::Message(const std::string &s) {
    this->sstream_.str(s);
    this->type_ = this->parse_u8(); // check that it's 0x02
    this->opcode_ = this->parse_u8(); // check that the opcode corresponds to the generator message
}

bool Message::requiresResponse() const {
    return this->requiresResponse_;
}

bool Message::isDirect() const {
    return this->isDirect_;
}

void Message::add_u8(uint8_t v) {
    this->sstream_ << v;
}

void Message::add_s8(int8_t v) {
    this->sstream_ << v;
}

void Message::add_u16(uint16_t v) {
    uint8_t v0 = v & 255;
    uint8_t v1 = (v >> 8) & 255;

    this->sstream_ << v0;
    this->sstream_ << v1;
}

void Message::add_s16(int16_t v) {
    int8_t v0 = v & 255;
    int8_t v1 = (v >> 8) & 255;

    this->sstream_ << v0;
    this->sstream_ << v1;
}

void Message::add_u32(uint32_t v) {
    uint8_t v0 = v & 255;
    uint8_t v1 = (v >> 8) & 255;
    uint8_t v2 = (v >> 16) & 255;
    uint8_t v3 = (v >> 24) & 255;

    this->sstream_ << v0;
    this->sstream_ << v1;
    this->sstream_ << v2;
    this->sstream_ << v3;
}

void Message::add_s32(int32_t v) {
    int8_t v0 = v & 255;
    int8_t v1 = (v >> 8) & 255;
    int8_t v2 = (v >> 16) & 255;
    int8_t v3 = (v >> 24) & 255;

    this->sstream_ << v0;
    this->sstream_ << v1;
    this->sstream_ << v2;
    this->sstream_ << v3;
}

uint8_t Message::parse_u8() {
    char v[1];
    this->sstream_.read(v, 1);
    return v[0];
}

int8_t Message::parse_s8() {
    char v[1];
    this->sstream_.read(v, 1);
    return v[0];
}

void Message::add_string(size_t n_bytes, const std::string& v) {
    this->sstream_ << std::setfill(static_cast<char>(0x00)) << std::left << std::setw(n_bytes) << v;
}

uint32_t Message::parse_u32() {
    char v[4];
    this->sstream_.read(v, 4);
    return (v[3] << 24) + (v[2] << 16) + (v[1] << 8) + v[0];
}

int32_t Message::parse_s32() {
    char v[4];
    this->sstream_.read(v, 4);
    return (v[3] << 24) + (v[2] << 16) + (v[1] << 8) + v[0];
}

uint16_t Message::parse_u16() {
    char v[2];
    this->sstream_.read(v, 2);
    return (v[1] << 8) + v[0];
}

int16_t Message::parse_s16() {
    char v[2];
    this->sstream_.read(v, 2);
    return (v[1] << 8) + v[0];
}


std::string Message::get_value() {
    return this->sstream_.str();
}

bool NXT::open() {
    return this->comm_->open();
}

bool NXT::isHalted() const {
    return this->halted;
}

void NXT::halt() {
    // TODO: halt brick cleanly
    /*
    for (uint8_t port = 0; port < 3; ++port) {
        this->makeMotor(port)->stop(false);
    }
    */
    this->halted = true;
}

double NXT::getFirmwareVersion() {
    uint8_t outBuf[7];
    const int min = 4, maj = 5;

    Message msg(false, true);
    msg.add_u8(uint8_t(Opcode::GET_FIRMWARE_VERSION));
    std::string data = msg.get_value();

    // Send the system command to the NXT.
    this->comm_->sendSystemCommand(true, (int8_t *)data.c_str(), data.size(), outBuf, sizeof(outBuf));

    double version = outBuf[min];

    while (version >= 1)
        version /= 10;
    version += outBuf[maj];

    return version;
}

void NXT::getDeviceInfo(uint8_t * outBuf, size_t size) {
    //uint8_t inBuf[] = { 0x01, 0x9B };
    int8_t inBuf[] = { static_cast<int8_t>(0x9B) };

    // Send the system command to the NXT.
    this->comm_->sendSystemCommand(true, inBuf, sizeof(inBuf), outBuf, size);
}

std::string NXT::getName() {
    uint8_t outBuf[33];
    char name[16];

    name[0] = '\0';

    this->getDeviceInfo(outBuf, sizeof(outBuf));

    std::memcpy(name, outBuf + 2, 15 * sizeof(uint8_t));

    name[15] = '\0';

    return std::string(name);
}

NXT::NXT(Comm *comm) {
    this->comm_ = comm;
    this->halted = false;
}

ConfiguredNXT* NXT::configure(SensorType sensor1, SensorType sensor2,
        SensorType sensor3, SensorType sensor4,
        MotorType motorA, MotorType motorB, MotorType motorC) {

     Sensor* sensorObject1 = this->sensorFactory.makeSensor(sensor1, SensorPort::IN_1, this->comm_);
     Sensor* sensorObject2 = this->sensorFactory.makeSensor(sensor2, SensorPort::IN_2, this->comm_);
     Sensor* sensorObject3 = this->sensorFactory.makeSensor(sensor3, SensorPort::IN_3, this->comm_);
     Sensor* sensorObject4 = this->sensorFactory.makeSensor(sensor4, SensorPort::IN_4, this->comm_);

     Motor* motorObjectA = this->motorFactory.makeMotor(motorA, MotorPort::OUT_A, this->comm_);
     Motor* motorObjectB = this->motorFactory.makeMotor(motorB, MotorPort::OUT_B, this->comm_);
     Motor* motorObjectC = this->motorFactory.makeMotor(motorC, MotorPort::OUT_C, this->comm_);

     ConfiguredNXT *configuredNXT = new ConfiguredNXT(this, this->comm_,
         sensorObject1, sensorObject2, sensorObject3, sensorObject4,
         motorObjectA, motorObjectB, motorObjectC);

     return configuredNXT;
}

ConfiguredNXT::ConfiguredNXT(NXT *nxt, Comm *comm, Sensor *sensor1, Sensor *sensor2,
        Sensor *sensor3, Sensor *sensor4,
        Motor *motorA, Motor *motorB, Motor *motorC) {
     this->nxt_ = nxt;

     this->comm_ = comm;

     this->sensorPort1 = sensor1;
     this->sensorPort2 = sensor2;
     this->sensorPort3 = sensor3;
     this->sensorPort4 = sensor4;

     this->motorPortA = motorA;
     this->motorPortB = motorB;
     this->motorPortC = motorC;
}

void NXT::playTone(uint16_t frequency, uint16_t duration) {
    Message msg(true, false);
    msg.add_u8(uint8_t(Opcode::PLAY_TONE));

    msg.add_u16(frequency);
    msg.add_u16(duration);

    std::string out = msg.get_value();

    this->comm_->sendDirectCommand(false, (int8_t *)out.c_str(), out.size(), NULL, 0);  
}

void NXT::stopSound() {
    Message msg(true, false);
    msg.add_u8(uint8_t(Opcode::STOP_SOUND));

    std::string out = msg.get_value();

    this->comm_->sendDirectCommand(false, (int8_t *)out.c_str(), out.size(), NULL, 0);  
}
