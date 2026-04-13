#include "game.h"
#include "match.h"
#include "menu.h"
#include "language.h"
#include "helpers.h"
#include "inputmanager.h"



bool tetrismatrix::debug = true;
Match *tetrismatrix::current_match = nullptr;
int tetrismatrix::frames = 0;
bool tetrismatrix::vertical = true;
Menu* tetrismatrix::main_menu = nullptr;
Language tetrismatrix::current_language = Language::ES;

Hub75 tetrismatrix::hub75(SCREEN_WIDTH, SCREEN_HEIGHT, nullptr, PANEL_GENERIC, false);
PicoGraphics_PenRGB888 tetrismatrix::graphics(hub75.width, hub75.height, nullptr);

void tetrismatrix::initGame()
{

    stdio_init_all();
    printf("starting\n");

    hub75.start(dma_complete);
    printf("hub75 started\n");

    if(!InputManager::init())
    {
        while(1) 
        {
            printf("bluetooth input failed! cannot continue\n\n");
            sleep_ms(1000);
        }
    }
    printf("bluetooth input started!\n");


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
    int pos_x = vertical ? SCREEN_HEIGHT / 2 : SCREEN_WIDTH / 2 - 2;
    int pos_y = vertical ? SCREEN_HEIGHT / 2 - 3 : SCREEN_HEIGHT / 2 - 3;
    std::string title = vertical ? "TETRIS MATRIX" : "TETRISMATRIX";
    main_menu = new Menu(title, main_menu_options, pos_x, pos_y);

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

    while(InputManager::controllers_connected == 0)
    {
        InputManager::searchForControllers();
    }

    // we clear the screan each frame.
    hub75.clear();


    if(current_match != nullptr)
    {
        //printf("updating match!!!\n\n");
        current_match->updateMatch();
    }

    if(main_menu != nullptr)
        main_menu->updateMenu(graphics, hub75);

    static std::array<int, 3> title_rgb_increments = {2, 3, 5};
    //static std::array<int, 3> arrow_rgb_increments = {4, 6, 10};

    rainbowEffect(main_menu->title_color, title_rgb_increments);
    //rainbowEffect(main_menu->arrow_color, arrow_rgb_increments);

    frames++;

    //printf("frames: %i\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", frames);
}



