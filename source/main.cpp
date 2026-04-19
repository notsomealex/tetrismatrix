#include "libs.h"
#include "game.h"


int main()
{

    tetrismatrix::initGame();

    while (1) 
    {
        extern char __StackLimit, __bss_end__;
        //printf("Free RAM: %d bytes\n", &__StackLimit - &__bss_end__);
        printf("MISC_BUTTON_START: 0x%04X\n", MISC_BUTTON_START);
        printf("BUTTON_X: 0x%04X\n", BUTTON_X);
        tetrismatrix::updateGame();
        sleep_ms(1000 / 60); // 60 fps
        printf("this hasnt crashed yet!\n\n");
    }
}
