#pragma once

#ifndef GAME_H
#define GAME_H

#include "match.h"

using namespace pimoroni;

class Game
{
private:
public:

    bool debug = true;

    Match *current_match = nullptr;
    static int frames;

    static Hub75 hub75;
    static PicoGraphics_PenRGB888 graphics;

    // Callback for the dma interrupt (required)
    static void __isr dma_complete() 
    {
        hub75.dma_complete();
    }
    
    void initGame();
    void updateGame();
    void newMatch();
    void endMatch();
    void playerInput();

    Game();

};

#endif