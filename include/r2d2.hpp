#ifndef R2D2_HPP
#define R2D2_HPP

#include <cstdint>
#include <sstream>

#include <r2d2/comm.hpp>

#define NXT_BLUETOOTH_ADDRESS "00:16:53"

class Mode {
public:
    static const int RAW = 0x00;
    static const int BOOLEAN = 0x20;
};

class Message {
private:
    std::stringstream sstream_;
    bool requiresResponse_;
    bool isDirect_;
    uint8_t type_;

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

    Message(bool isDirect, bool requiresResponse);

    bool requiresResponse() const;

    bool isDirect() const;

    void add_u8(uint8_t v);

    void add_s8(int8_t v);

    void add_string(size_t n_bytes, const std::string& v);

    std::string get_value();
};

class Sensor;

class SonarSensor;

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

    void sendDirectCommand(bool, int8_t *, size_t, uint8_t *, size_t);

    bool open();

    std::string getName();

    double getFirmwareVersion();

    void getDeviceInfo(uint8_t*, size_t);

    Sensor *makeTouch(uint8_t);

    SonarSensor *makeSonar(uint8_t);

    Motor *makeMotor(uint8_t);

    void halt();

    bool isHalted() const;
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
};

class TouchSensor : public AnalogSensor {
public:
    TouchSensor(NXT *nxt, uint8_t port) : AnalogSensor(nxt, port) { };
    int getValue();
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
#endif
