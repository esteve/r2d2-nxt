#include <iostream>

#include <r2d2.hpp>
#include <r2d2/usb.hpp>

#include <ctime>

int C = 523;
int D = 587;
int E = 659;
int G = 784;
int R = 0;

void play_tone_and_wait(NXT *nxt, int frequency, int duration) {
    nxt->playTone(frequency, duration);
    usleep(duration * 1000);
}

void play(NXT *nxt, int note) {
    if (note > 0) {
        play_tone_and_wait(nxt, note, 500);
    } else {
        usleep(500000);
    }
}

int main()
{
    std::cout << "R2D2-NXT test project" << std::endl;

    USBNXTManager usbm;
    NXT* nxt = usbm.list()->at(0);
    if (nxt->open()) { //initialize the NXT and continue if it succeds
        std::cout << nxt->getName() << std::endl;

        int song[] = {E, D, C, D, E, E, E, R,
             D, D, D, R,
             E, G, G, R, E, D, C, D, E, E, E, E, D, D, E, D, C};

        for(int i=0;i < 29; i++) {
            play(nxt, song[i]);
        }
    }
    return 0;
}
