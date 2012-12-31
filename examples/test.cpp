#include <iostream>

#include <r2d2.hpp>
#include <r2d2/usb.hpp>

int main()
{
    std::cout << "R2D2-NXT test project" << std::endl;

    USBBrickManager usbm;
    Brick* brick = usbm.list()->at(0);

    int turnCount = 0;

    NXT* nxt = brick->configure(SensorType::NULL_SENSOR, SensorType::TOUCH_SENSOR,
        SensorType::NULL_SENSOR, SensorType::NULL_SENSOR, MotorType::STANDARD_MOTOR, MotorType::STANDARD_MOTOR,
        MotorType::STANDARD_MOTOR);


    if (nxt != nullptr) { //initialize the NXT and continue if it succeds

        std::cout << brick->getName() << std::endl;
        Sensor* sensor = nxt->sensorPort(SensorPort::IN_2); //tell the NXT that the touch sensor is in port 1
        Motor* motor = nxt->motorPort(MotorPort::OUT_A);
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
