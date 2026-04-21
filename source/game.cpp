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
std::stack<Menu *> tetrismatrix::menus;
Language tetrismatrix::current_language = Language::ES;

tetrismatrix::Settings tetrismatrix::settings;

Hub75 tetrismatrix::hub75(SCREEN_WIDTH, SCREEN_HEIGHT, nullptr, PANEL_GENERIC, false);
PicoGraphics_PenRGB888 tetrismatrix::graphics(hub75.width, hub75.height, nullptr);

void tetrismatrix::initGame()
{

    stdio_init_all();
    printf("starting\n");

    hub75.start(dma_complete);
    printf("hub75 started\n");

    if (!InputManager::init())
    {
        while (1)
        {
            printf("bluetooth input failed! cannot continue\n\n");
            sleep_ms(1000);
        }
    }
    printf("bluetooth input started!\n");


    Menu* main_menu = createMenu(Menus::MAIN_MENU);
    menus.push(main_menu);

    

    // newMatch();
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
    // printf("updating game!!\n\n");

    while (InputManager::controllers.empty())
    {
        InputManager::searchForControllers();
    }

    // we clear the screan each frame.
    hub75.clear();

    InputManager::scanKeys(); 

    if(!menus.empty())
    {
        menus.top()->updateMenu(graphics, hub75);
    }

    if(current_match != nullptr)
        current_match->updateMatch();

    frames++;

    // printf("frames: %i\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", frames);
}

Menu* tetrismatrix::createMenu(Menus menu_type)
{
    std::vector<Menu::Option> options;
    std::string title;
    int pos_x;
    int pos_y;
    Menu* menu = nullptr;
    switch(menu_type)
    {
        case Menus::MAIN_MENU:
            options = {
            {
                Menu::Option(
                    translate(current_language, Texts::START),
                    []()
                    {menus.push(createMenu(Menus::GAME_MODE_SELECTION)); InputManager::clearAllInputs();},
                    0,
                    true),

                Menu::Option(
                    translate(current_language, Texts::OPTIONS),
                    NULL, // TODO
                    0,
                    true),

                Menu::Option(
                    translate(current_language, Texts::SCORES),
                    NULL, // TODO
                    0,
                    true),
            }
        };

            title = vertical ? "TETRIS MATRIX" : "TETRISMATRIX";
            pos_x = vertical ? SCREEN_HEIGHT / 2 : SCREEN_WIDTH / 2 - 2;
            pos_y = vertical ? SCREEN_HEIGHT / 2 - 3 : SCREEN_HEIGHT / 2 - 3;
            break;
        case Menus::OPTIONS:

            break;
        case Menus::SCORES:
            
            break;

        case Menus::PAUSE:
            options = {
                Menu::Option(
                translate(current_language, Texts::RESUME),
                []()
                { menus.pop(); InputManager::clearAllInputs();},
                0,
                true),
                Menu::Option(
                translate(current_language, Texts::EXIT),
                []()
                { if(current_match != nullptr) delete current_match; current_match = nullptr; menus.pop(); menus.top()->hide_menu = false; InputManager::clearAllInputs();},
                0,
                true)
            };
            title = translate(current_language, PAUSE);
            pos_x = vertical ? SCREEN_HEIGHT / 2 : SCREEN_WIDTH / 2 - 2;
            pos_y = vertical ? SCREEN_HEIGHT / 2 - 3 : SCREEN_HEIGHT / 2 - 3;
            break;

        case Menus::GAME_MODE_SELECTION:
        options = {
            {
                Menu::Option(
                    translate(current_language, Texts::ZEN),
                    []()
                    {if(current_match == nullptr) current_match = new Match(settings.match_settings, GameModes::ZEN); InputManager::clearAllInputs();},
                    0,
                    true),

                Menu::Option(
                    translate(current_language, Texts::MARATHON),
                    []()
                    {if(current_match == nullptr) current_match = new Match(settings.match_settings, GameModes::MARATHON); InputManager::clearAllInputs();}, 
                    0,
                    true),

                Menu::Option(
                    translate(current_language, Texts::VERSUS),
                    []()
                    {if(current_match == nullptr) current_match = new Match(settings.match_settings, GameModes::VERSUS); InputManager::clearAllInputs();},
                    0,
                    true),

                Menu::Option(
                    translate(current_language, Texts::COOP),
                    []()
                    {if(current_match == nullptr) current_match = new Match(settings.match_settings, GameModes::COOP); InputManager::clearAllInputs();},
                    0,
                    true),

                Menu::Option(
                    translate(current_language, Texts::DUAL_BOARD),
                    []()
                    {if(current_match == nullptr) current_match = new Match(settings.match_settings, GameModes::DUAL_BOARD); InputManager::clearAllInputs();}, 
                    0,
                    true),
            }
        };
            title = translate(current_language, Texts::GAME_MODE);
            pos_x = vertical ? SCREEN_HEIGHT / 2 : SCREEN_WIDTH / 2 - 2;
            pos_y = vertical ? SCREEN_HEIGHT / 2 - 3 : SCREEN_HEIGHT / 2 - 3;
            break;
    }


    menu = new Menu(title, options, pos_x, pos_y, nullptr);


    // assign callback function and rest of the stuff.
    switch(menu_type)
    {
        case Menus::MAIN_MENU:
            menu->callback = [menu]() {
            static std::array<int, 3> title_rgb_increments = {2, 3, 5};
            rainbowEffect(menu->title_color, title_rgb_increments);
            };
            menu->menu_type = Menus::MAIN_MENU;
            break;
        case Menus::OPTIONS:
            menu->menu_type = Menus::OPTIONS;
            break;
        case Menus::SCORES:
            menu->menu_type = Menus::SCORES;
            break;
        case Menus::PAUSE:
            menu->menu_type = Menus::PAUSE;
            menu->callback = [menu](){
                if(InputManager::keys_pressed[InputManager::controllers[0]] & KEY_START)
                {
                    tetrismatrix::menus.pop();
                    InputManager::clearAllInputs();
                }
            };
            break;
    }
    return menu;
}

int tetrismatrix::screenWidth()
{
    return tetrismatrix::vertical ? SCREEN_HEIGHT : SCREEN_WIDTH;
}

int tetrismatrix::screenHeight()
{
    return tetrismatrix::vertical ? SCREEN_WIDTH : SCREEN_HEIGHT;
}
