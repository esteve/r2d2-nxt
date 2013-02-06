#include <iostream>
#include <unistd.h>

#include <r2d2.hpp>
#include <r2d2/bluetooth.hpp>

#include <ctime>

int C = 523;
int D = 587;
int E = 659;
int G = 784;
int R = 0;

void play_tone_and_wait(Brick *brick, int frequency, int duration) {
    brick->playTone(frequency, duration);
    usleep(duration * 1000);
}

void play(Brick *brick, int note) {
    if (note > 0) {
        play_tone_and_wait(brick, note, 500);
    } else {
        usleep(500000);
    }
}

int main()
{
    std::cout << "R2D2-NXT test project" << std::endl;

    BTBrickManager btm;
    Brick* brick = btm.list()->at(0);
    if (brick->open()) { //initialize the NXT and continue if it succeds
        std::cout << brick->getName() << std::endl;

        int song[] = {E, D, C, D, E, E, E, R,
             D, D, D, R,
             E, G, G, R, E, D, C, D, E, E, E, E, D, D, E, D, C};

        for(int i=0;i < 29; i++) {
            play(brick, song[i]);
        }
    }
    return 0;
}
