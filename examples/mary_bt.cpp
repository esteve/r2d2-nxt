#include <iostream>

#include <r2d2/r2d2_base.hpp>
#include <r2d2/bluetooth.hpp>

#include <thread>
#include <chrono>

int C = 523;
int D = 587;
int E = 659;
int G = 784;
int R = 0;

void play_tone_and_wait(r2d2::Brick *brick, int frequency, int duration) {
    brick->playTone(frequency, duration);
    std::chrono::milliseconds dura(duration);
    std::this_thread::sleep_for(dura);
}

void play(r2d2::Brick *brick, int note) {
    if (note > 0) {
        play_tone_and_wait(brick, note, 500);
    } else {
        std::chrono::milliseconds dura(500);
        std::this_thread::sleep_for(dura);
    }
}

int main()
{
    std::cout << "R2D2-NXT test project" << std::endl;

    r2d2::BTBrickManager btm;
    r2d2::Brick* brick = btm.list()->at(0);
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
