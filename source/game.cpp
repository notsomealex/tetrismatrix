#include "game.h"
#include "match.h"
#include "menu.h"
#include "language.h"



bool tetrismatrix::debug = true;
Match *tetrismatrix::current_match = nullptr;
int tetrismatrix::frames = 0;
bool tetrismatrix::vertical = true;
Menu* tetrismatrix::main_menu = nullptr;
Language tetrismatrix::current_language = Language::EN;

Hub75 tetrismatrix::hub75(SCREEN_WIDTH, SCREEN_HEIGHT, nullptr, PANEL_GENERIC, false);
PicoGraphics_PenRGB888 tetrismatrix::graphics(hub75.width, hub75.height, nullptr);

void tetrismatrix::initGame()
{

    stdio_init_all();
    printf("starting\n");

    hub75.start(dma_complete);
    printf("hub75 started\n");


    std::vector<Menu::Option> main_menu_options = 
    {
        {
            Menu::Option(
            translate(current_language, Texts::START), 
            []() {if(current_match == nullptr) current_match = new Match();},
            0),
            
            Menu::Option(
                translate(current_language, Texts::OPTIONS),
                NULL, // TODO
                0),
            
            Menu::Option(
                translate(current_language, Texts::SCORES),
                NULL, // TODO
                0                
            )
        }
    };
    int pos_x = vertical ? SCREEN_HEIGHT / 2 : SCREEN_WIDTH / 2;
    int pos_y = vertical ? SCREEN_HEIGHT / 2 - 3 : SCREEN_HEIGHT / 2 - 5;
    main_menu = new Menu(main_menu_options, pos_x, pos_y);

    //newMatch();
        
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


void tetrismatrix::updateGame()
{
    //printf("updating game!!\n\n");

    // we clear the screan each frame.
    hub75.clear();

    if(current_match != nullptr)
    {
        //printf("updating match!!!\n\n");
        current_match->updateMatch();
    }

    if(main_menu != nullptr)
        main_menu->displayMenu(graphics, hub75);
    frames++;

    printf("frames: %i\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", frames);
}




