#include "game.h"
#include "match.h"
bool tetrismatrix::debug = true;
Match *tetrismatrix::current_match = nullptr;
int tetrismatrix::frames = 0;

    Hub75 tetrismatrix::hub75(64, 32, nullptr, PANEL_GENERIC, false);
    PicoGraphics_PenRGB888 tetrismatrix::graphics(hub75.width, hub75.height, nullptr);

void tetrismatrix::initGame()
{

    stdio_init_all();
    while (!stdio_usb_connected()) tight_loop_contents();
    printf("starting\n");

    hub75.start(dma_complete);
    printf("hub75 started\n");



     // TODO: show menu and stuff.

    if(current_match == nullptr)
        current_match = new Match();
        
}


void tetrismatrix::endMatch()
{
    if (current_match != nullptr)
    {
        delete current_match;
        current_match = nullptr;
    }
}

void tetrismatrix::newMatch()
{
    if (current_match == nullptr)
        current_match = new Match();
}

void tetrismatrix::playerInput()
{
}

void tetrismatrix::updateGame()
{
    printf("updating game!!\n\n");

    // we clear the screan each frame.
    hub75.clear();

    if(current_match != nullptr)
        current_match->updateMatch();

    frames++;
}


