#include <iostream>

#include <r2d2/r2d2_base.hpp>
#include <r2d2/usb.hpp>

int main()
{
    std::cout << "R2D2-NXT test project" << std::endl;

    r2d2::USBBrickManager usbm;
    r2d2::Brick* brick = usbm.list()->at(0);

    int turnCount = 0;

    r2d2::NXT* nxt = brick->configure(r2d2::SensorType::NULL_SENSOR, r2d2::SensorType::TOUCH_SENSOR,
        r2d2::SensorType::NULL_SENSOR, r2d2::SensorType::NULL_SENSOR, r2d2::MotorType::STANDARD_MOTOR,
        r2d2::MotorType::STANDARD_MOTOR, r2d2::MotorType::STANDARD_MOTOR);


    if (nxt != nullptr) { //initialize the NXT and continue if it succeds

        std::cout << brick->getName() << std::endl;
        r2d2::Sensor* sensor = nxt->sensorPort(r2d2::SensorPort::IN_2); //tell the NXT that the touch sensor is in port 1
        r2d2::Motor* motor = nxt->motorPort(r2d2::MotorPort::OUT_A);
        int oldCount = motor->getRotationCount();
        while (1) { //main loop
            if (sensor->getValue() == true) //if the touch sensor is pressed down...
                motor->setForward(50); //turn the motor in port 1 on 50% power
            else
                motor->stop(false); //if the touch sensor is not pressed down turn the motor off
            int newCount = motor->getRotationCount();
            if (newCount > oldCount) {
                turnCount += newCount - oldCount;
                oldCount = newCount;
                std::cout << "Number of turns so far is..." << turnCount << std::endl;
            }
        }
    }
    return 0;
}
