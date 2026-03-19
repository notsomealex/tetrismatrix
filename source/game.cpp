#include "game.h"


int Game::frames = 0;

    Hub75 Game::hub75(64, 32, nullptr, PANEL_GENERIC, false);
    PicoGraphics_PenRGB888 Game::graphics(hub75.width, hub75.height, nullptr);

void Game::initGame()
{

    stdio_init_all();
    while (!stdio_usb_connected()) tight_loop_contents();
    printf("starting\n");

    hub75.start(dma_complete);
    printf("hub75 started\n");

    Pen BG = graphics.create_pen(0, 0, 0);
    Pen WHITE = graphics.create_pen(200, 200, 200);

    if(current_match == nullptr)
        current_match = new Match();
        
}


void Game::endMatch()
{
    if (current_match != nullptr)
    {
        delete current_match;
        current_match = nullptr;
    }
}

void Game::newMatch()
{
    if (current_match == nullptr)
        current_match = new Match();
}

void Game::playerInput()
{
}

void Game::updateGame()
{
    if(current_match != nullptr)
        current_match->updateMatch();

        frames++;
}

Game::Game()
{
    
}


