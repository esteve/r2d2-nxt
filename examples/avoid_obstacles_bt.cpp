#include <signal.h>

#include <iostream>
#include <ctime>
#include <thread>
#include <chrono>

#include <r2d2/r2d2_base.hpp>
#include <r2d2/usb.hpp>
#include <r2d2/bluetooth.hpp>

NXT *nxt = nullptr;

void r2d2_usleep(int milliseconds) {
    std::chrono::milliseconds duration(milliseconds);
    std::this_thread::sleep_for(duration);
};


class Task2 {
private:
    // How far should the sonar report
    static const int RANGE_SONAR = 40;

    // We define where the motors and the sensors are so that the code is more
    // readable later
    static const MotorPort RIGHT_MOTOR = MotorPort::OUT_B;
    static const MotorPort LEFT_MOTOR = MotorPort::OUT_C;

    static const SensorPort RIGHT_TOUCH_SENSOR = SensorPort::IN_1;
    static const SensorPort LEFT_TOUCH_SENSOR = SensorPort::IN_2;

    static const SensorPort SONAR_SENSOR = SensorPort::IN_3;

public:
    Task2() {
        BTBrickManager bm;
        Brick* brick = bm.list()->at(0);

        nxt = brick->configure(SensorType::TOUCH_SENSOR, SensorType::TOUCH_SENSOR,
        SensorType::SONAR_SENSOR, SensorType::NULL_SENSOR, MotorType::STANDARD_MOTOR, MotorType::STANDARD_MOTOR,
        MotorType::STANDARD_MOTOR);
    }

    void setLeftSpin() {
        // Move the right motor forwards while moving the left backwards one
        // makes a left turn
        nxt->motorPort(RIGHT_MOTOR)->setForward(50);
        nxt->motorPort(LEFT_MOTOR)->setReverse(50);
    }

    void setRightSpin() {
        // Move the right motor backwards while moving the left forwards one
        // makes a right turn
        nxt->motorPort(RIGHT_MOTOR)->setReverse(50);
        nxt->motorPort(LEFT_MOTOR)->setForward(50);
    }

    void setBothBack() {
        // Move both motors backwards
        nxt->motorPort(RIGHT_MOTOR)->setReverse(50);
        nxt->motorPort(LEFT_MOTOR)->setReverse(50);
    }

    void setBothForward() {
        // Move both motors forwards
        nxt->motorPort(RIGHT_MOTOR)->setForward(50);
        nxt->motorPort(LEFT_MOTOR)->setForward(50);
    }

    void run() {
        bool leftTouched = false;
        bool rightTouched = false;
        bool obstacleFound = false;

        bool direction = false; // false is to turn left, right to turn right

        this->setBothForward();

        while (true) {

            leftTouched = nxt->sensorPort(LEFT_TOUCH_SENSOR)->getValue();
            rightTouched = nxt->sensorPort(RIGHT_TOUCH_SENSOR)->getValue();

            obstacleFound = nxt->sensorPort(SONAR_SENSOR)->getValue() < RANGE_SONAR;

            if (leftTouched) {
                this->setBothBack();
                r2d2_usleep(1000);

                this->setRightSpin();
                // Sleeping for 0.5 seconds gives the NXT enough time to make
                // a 45º right spin
                r2d2_usleep(500);

                this->setBothForward();

            } else if (rightTouched) {
                this->setBothBack();
                r2d2_usleep(1000);

                this->setLeftSpin();
                // Sleeping for 0.5 seconds gives the NXT enough time to make
                // a 45º left spin
                r2d2_usleep(500);

                this->setBothForward();

            } else if (obstacleFound) {
                this->setBothBack();
                r2d2_usleep(1000);

                // Choose the direction the spin will take randomly
                direction = random() % 2; // between 0 and 1, i.e. false and true
                direction ? this->setRightSpin() : this->setLeftSpin();

                r2d2_usleep(500);

                this->setBothForward();

            }

        }
    }
};

void signal_callback_handler(int signum) {
    // Ensure that all motors are stopped
    if(nxt != nullptr) {
        nxt->motorPort(MotorPort::OUT_A)->stop(false);
        nxt->motorPort(MotorPort::OUT_B)->stop(false);
        nxt->motorPort(MotorPort::OUT_C)->stop(false);
    }

    std::cout << "EXITING NXT++ afraidOfProximity project" << std::endl;
    r2d2_usleep(1000);
    exit(signum);
}

int main(void) {
    if(nxt != nullptr) {
        std::cout << "Could not open the NXT robot" << std::endl;
        exit(1);
    }

    signal(SIGINT, signal_callback_handler);

    Task2 *task2 = new Task2();
    task2->run();
    delete task2;
    return 0;
}
