#include <iostream>

#include <r2d2.hpp>
#include <r2d2/usb.hpp>

int main()
{
    std::cout << "R2D2-NXT test project" << std::endl;

    USBNXTManager usbm;
    NXT* nxt = usbm.list()->at(0);
    if (nxt->open()) { //initialize the NXT and continue if it succeds
        Sensor* sensor = nxt->makeTouch(NXT::IN_1); //tell the NXT that the touch sensor is in port 1
        Motor* motor = nxt->makeMotor(NXT::OUT_A);
        while (1) { //main loop
            if (sensor->getValue() == true) //if the touch sensor is pressed down...
                motor->setForward(50); //turn the motor in port 1 on 50% power
            else
                motor->stop(false); //if the touch sensor is not pressed down turn the motor off
        }
    }
    return 0;
}
