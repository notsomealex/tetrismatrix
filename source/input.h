#include "libs.h"

enum  INPUT
{
    KEY_LEFT = 1 << 0,
    KEY_RIGHT = 1 << 1,
    KEY_UP = 1 << 2,
    KEY_DOWN = 1 << 3,
    KEY_A = 1 << 4,
    KEY_B = 1 << 5,
    KEY_X = 1 << 6,
    KEY_Y = 1 << 7,
    KEY_L = 1 << 8,
    KEY_R = 1 << 9
};

uint16_t getInput();

extern uint16_t keys_down;

    
