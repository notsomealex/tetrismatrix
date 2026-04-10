#include "libs.h"
#include "game.h"


int main()
{

    tetrismatrix::initGame();

    while (1) 
    {
        tetrismatrix::updateGame();
        sleep_ms(1000 / 60); // 60 fps
    }
}
