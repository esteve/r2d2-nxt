#include <r2d2.hpp>
#include <r2d2/motors.hpp>

StandardMotor::StandardMotor(Comm *comm, MotorPort port) {
    this->comm_ = comm;
    this->port_ = port;
}

void StandardMotor::setForward(uint8_t power) {
    Message msg(true, false);
    msg.add_u8(uint8_t(Opcode::START_MOTOR));
    msg.add_u8(uint8_t(this->port_));
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

    this->comm_->sendDirectCommand( false, (int8_t *)out.c_str(), out.size(), NULL, 0);
}

void StandardMotor::setReverse(uint8_t power) {
    Message msg(true, false);
    msg.add_u8(uint8_t(Opcode::START_MOTOR));
    msg.add_u8(uint8_t(this->port_));
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

    this->comm_->sendDirectCommand( false, (int8_t *)out.c_str(), out.size(), NULL, 0);
}

void StandardMotor::stop(bool brake) {
    Message msg(true, false);
    msg.add_u8(uint8_t(Opcode::STOP_MOTOR));
    msg.add_u8(uint8_t(this->port_));
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

    this->comm_->sendDirectCommand( false, (int8_t *)out.c_str(), out.size(), NULL, 0);
}

void StandardMotor::resetRotationCount(bool relative) {
    Message msg(true, false);
    msg.add_u8(uint8_t(Opcode::RESET_ROTATION_COUNT));
    msg.add_u8(uint8_t(this->port_));
    msg.add_u8(relative);

    std::string out = msg.get_value();

    this->comm_->sendDirectCommand( false, (int8_t *)out.c_str(), out.size(), NULL, 0);
}

Motor * MotorFactory::makeMotor(MotorType type, MotorPort port, Comm *comm) {
    Motor *motor;

    switch(type) {
        case MotorType::STANDARD_MOTOR:
            motor = new StandardMotor(comm, port);
            break;
        default:
            motor = new NullMotor;
    }

    return motor;
}

int StandardMotor::getRotationCount() {
    Message msg(true, true);
    msg.add_u8(uint8_t(Opcode::GET_ROTATION_COUNT));
    msg.add_u8(uint8_t(this->port_));

    std::string out = msg.get_value();

    unsigned char responseBuffer[25];

    memset(responseBuffer, 1, sizeof(responseBuffer));

    this->comm_->sendDirectCommand(true, (int8_t *)out.c_str(), out.size(), responseBuffer, sizeof(responseBuffer));

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
