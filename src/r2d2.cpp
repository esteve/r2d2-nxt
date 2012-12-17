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

Motor::Motor(NXT *nxt, uint8_t port) {
    this->nxt_ = nxt;
    this->port_ = port;
}

void Motor::setForward(uint8_t power) {
    Message msg(true, false);
    msg.add_u8(Message::START_MOTOR);
    msg.add_u8(this->port_);
    msg.add_s8(power);

    msg.add_u8(0x01 | 0x04); // UNKNOWN
    msg.add_u8(0x01); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x20); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN

    std::string out = msg.get_value();

    this->nxt_->sendDirectCommand( false, (int8_t *)out.c_str(), out.size(), NULL, 0);
}

void Motor::setReverse(uint8_t power) {
    Message msg(true, false);
    msg.add_u8(Message::START_MOTOR);
    msg.add_u8(this->port_);
    msg.add_s8(-power);

    msg.add_u8(0x01 | 0x04); // UNKNOWN
    msg.add_u8(0x01); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x20); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN

    std::string out = msg.get_value();

    this->nxt_->sendDirectCommand( false, (int8_t *)out.c_str(), out.size(), NULL, 0);
}

void Motor::stop(bool brake) {
    Message msg(true, false);
    msg.add_u8(Message::STOP_MOTOR);
    msg.add_u8(this->port_);
    msg.add_s8(0); // power 0?

    if (brake) {
        msg.add_u8(0x01 | 0x02 | 0x04); // UNKNOWN
        msg.add_u8(0x01); // UNKNOWN
        msg.add_u8(0x00); // UNKNOWN
        msg.add_u8(0x20); // UNKNOWN
    } else {
        msg.add_u8(0x00); // UNKNOWN
        msg.add_u8(0x00); // UNKNOWN
        msg.add_u8(0x00); // UNKNOWN
        msg.add_u8(0x00); // UNKNOWN
    }

    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN

    std::string out = msg.get_value();

    this->nxt_->sendDirectCommand( false, (int8_t *)out.c_str(), out.size(), NULL, 0);
}

void Motor::resetRotationCount(bool relative) {
    Message msg(true, false);
    msg.add_u8(Message::RESET_ROTATION_COUNT);
    msg.add_u8(this->port_);
    msg.add_u8(relative);

    std::string out = msg.get_value();

    this->nxt_->sendDirectCommand( false, (int8_t *)out.c_str(), out.size(), NULL, 0);
}


Sensor::Sensor(NXT *nxt, uint8_t port) {
    this->nxt_ = nxt;
    this->port_ = port;
}

NXT* Sensor::getNXT() {
    return this->nxt_;
}

uint8_t Sensor::getPort() {
    return this->port_;
}

int AnalogSensor::getValue() {
    Message msg(true, true);
    msg.add_u8(Message::GET_VALUE);
    msg.add_u8(this->getPort());

    std::string out = msg.get_value();

    unsigned char responseBuffer[16];

    memset(responseBuffer, 1, sizeof(responseBuffer));

    this->getNXT()->sendDirectCommand(true, (int8_t *)out.c_str(), out.size(), responseBuffer, sizeof(responseBuffer));

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
    msg.add_u8(Message::LS_GET_STATUS);
    msg.add_u8(this->getPort());

    std::string out = msg.get_value();

    unsigned char responseBuffer[4];

    memset(responseBuffer, 1, sizeof(responseBuffer));

    this->getNXT()->sendDirectCommand(true, (int8_t *)out.c_str(), out.size(), responseBuffer, sizeof(responseBuffer));

    std::copy(responseBuffer, responseBuffer + sizeof(responseBuffer), outbuf);

    return static_cast<int>(responseBuffer[3]);
}

void DigitalSensor::lsRead(uint8_t *outbuf) {
    Message msg(true, true);
    msg.add_u8(Message::LS_READ);
    msg.add_u8(this->getPort());

    std::string out = msg.get_value();

    unsigned char responseBuffer[20];

    memset(responseBuffer, 1, sizeof(responseBuffer));

    this->getNXT()->sendDirectCommand(true, (int8_t *)out.c_str(), out.size(), responseBuffer, sizeof(responseBuffer));

    std::copy(responseBuffer, responseBuffer + sizeof(responseBuffer), outbuf);
}

void DigitalSensor::lsWrite(const std::string& indata, uint8_t *outBuf, size_t outSize) {
    Message msg(true, true);
    msg.add_u8(Message::LS_WRITE);
    msg.add_u8(this->getPort());
    msg.add_u8(indata.size());
    msg.add_u8(outSize);
    msg.add_string(indata.size(), indata);

    std::string tosend = msg.get_value();

    unsigned char responseBuffer[3];

    memset(responseBuffer, 1, sizeof(responseBuffer));

    this->getNXT()->sendDirectCommand(true, (int8_t *)(tosend.c_str()), tosend.size(), responseBuffer, sizeof(responseBuffer));

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

TouchSensor* NXT::makeTouch(uint8_t port) {
    Message msg(true, false);
    msg.add_u8(Message::SET_INPUT_MODE);
    msg.add_u8(port);
    msg.add_u8(Message::TOUCH);
    msg.add_u8(Mode::BOOLEAN);

    std::string out = msg.get_value();
    this->sendDirectCommand( false, (int8_t *)out.c_str(), out.size(), NULL, 0);
    return new TouchSensor(this, port);
}

LightSensor* NXT::makeLight(uint8_t port, bool active)
{
    Message msg(true, false);
    msg.add_u8(Message::SET_INPUT_MODE);
    msg.add_u8(port);

    if(active) {
        msg.add_u8(Message::ACTIVE_LIGHT);
    } else {
        msg.add_u8(Message::PASSIVE_LIGHT);
    }
    msg.add_u8(Mode::RAW);

    std::string out = msg.get_value();
    this->sendDirectCommand( false, (int8_t *)out.c_str(), out.size(), NULL, 0);
    return new LightSensor(this, port);
}


SonarSensor* NXT::makeSonar(uint8_t port) {
    // 0x05 -> setInputMode
    // 0x0B -> LOWSPEED_9V
    // 0x00 -> RAW
    Message msg(true, false);
    msg.add_u8(Message::SET_INPUT_MODE);
    msg.add_u8(port);
    msg.add_u8(Message::LOWSPEED_9V);
    msg.add_u8(Mode::RAW);

    std::string out = msg.get_value();
    this->sendDirectCommand( false, (int8_t *)out.c_str(), out.size(), NULL, 0);
    return new SonarSensor(this, port);
}

Motor* NXT::makeMotor(uint8_t port) {
    return new Motor(this, port);
}

bool NXT::open() {
    return this->comm_->open();
}

bool NXT::isHalted() const {
    return this->halted;
}

void NXT::halt() {
    for (uint8_t port = 0; port < 3; ++port) {
        this->makeMotor(port)->stop(false);
    }
    this->halted = true;
}

double NXT::getFirmwareVersion() {
    uint8_t outBuf[7];
    const int min = 4, maj = 5;

    Message msg(false, true);
    msg.add_u8(Message::GET_FIRMWARE_VERSION);
    std::string data = msg.get_value();

    // Send the system command to the NXT.
    this->sendSystemCommand(true, (int8_t *)data.c_str(), data.size(), outBuf, sizeof(outBuf));

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
    this->sendSystemCommand(true, inBuf, sizeof(inBuf), outBuf, size);
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

void NXT::sendSystemCommand(bool response, int8_t * dc_buf,
                            size_t dc_buf_size, uint8_t * re_buf, size_t re_buf_size) {
    uint8_t buf[dc_buf_size + 1];

    std::copy(dc_buf, dc_buf + dc_buf_size, buf + 1);

    buf[0] = response ? 0x01 : 0x81;

    this->comm_->devWrite(buf, dc_buf_size + 1);

    if (response) {
        unsigned char tempreadbuf[re_buf_size];
        this->comm_->devRead(tempreadbuf, re_buf_size);

        std::copy(tempreadbuf + 1, tempreadbuf + re_buf_size, re_buf);
    }
}

void NXT::sendDirectCommand(bool response, int8_t * dc_buf,
                            size_t dc_buf_size, unsigned char * re_buf, size_t re_buf_size) {
    uint8_t buf[dc_buf_size + 1];

    std::copy(dc_buf, dc_buf + dc_buf_size, buf + 1);

    buf[0] = response ? 0x00 : 0x80;

    this->comm_->devWrite(buf, dc_buf_size + 1);

    if (response) {
        this->comm_->devRead(re_buf, re_buf_size);
    }
}

void NXT::playTone(uint16_t frequency, uint16_t duration) {
    Message msg(true, false);
    msg.add_u8(Message::PLAY_TONE);

    msg.add_u16(frequency);
    msg.add_u16(duration);

    std::string out = msg.get_value();

    this->sendDirectCommand(false, (int8_t *)out.c_str(), out.size(), NULL, 0);  
}

void NXT::stopSound() {
    Message msg(true, false);
    msg.add_u8(Message::STOP_SOUND);

    std::string out = msg.get_value();

    this->sendDirectCommand(false, (int8_t *)out.c_str(), out.size(), NULL, 0);  
}

int Motor::getRotationCount() {
    Message msg(true, true);
    msg.add_u8(Message::GET_ROTATION_COUNT);
    msg.add_u8(this->port_);

    std::string out = msg.get_value();

    unsigned char responseBuffer[25];

    memset(responseBuffer, 1, sizeof(responseBuffer));

    this->nxt_->sendDirectCommand(true, (int8_t *)out.c_str(), out.size(), responseBuffer, sizeof(responseBuffer));

    std::string s(responseBuffer, responseBuffer + 25);

    Message response(s);

/*
    port = tgram.parse_u8() 3
    power = tgram.parse_s8() 4

    mode = tgram.parse_u8() 5
    regulation = tgram.parse_u8() 6
    turn_ratio = tgram.parse_s8() 7
    run_state = tgram.parse_u8() 8

    tacho_limit = tgram.parse_u32() 9,10,11,12
    tacho_count = tgram.parse_s32() 13,14,15,16
    block_tacho_count = tgram.parse_s32() 17,18,19,20
    rotation_count = tgram.parse_s32() 21,22,23,24,25
*/

    response.parse_u8(); // 2 check status

    response.parse_u8(); // 3 port

    response.parse_s8(); // 4 power

    response.parse_u8(); // 5 mode
    response.parse_u8(); // 6 regulation
    response.parse_s8(); // 7 turn ratio
    response.parse_u8(); // 8 run_state

    response.parse_u32(); // 9,10,11,12 tacho limit

    response.parse_s32(); // 13,14,15,16 tacho count

    response.parse_s32(); // 17,18,19,20 block tacho count

    int tacho2 = response.parse_s32(); // 21,22,23,24 calibrated value

    int i = responseBuffer[21];
    if(i < 0)
            i = 256 + i;
    if(responseBuffer[23] == -1)
            responseBuffer[23] = 0;
    if(responseBuffer[24] == -1)
            responseBuffer[24] = 0;

    int tacho = (responseBuffer[24] << 24) + (responseBuffer[23] << 16) + (responseBuffer[22] << 8) + i;

//    std::cout << "TACHO: " << (tacho == tacho2) << std::endl;

    return tacho;
}
