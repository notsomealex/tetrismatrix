#include "game.h"
#include "match.h"
#include "menu.h"
#include "language.h"
#include "helpers.h"
#include "inputmanager.h"
#include "databasemanager.h"

bool tetrismatrix::debug = true;
Match *tetrismatrix::current_match = nullptr;
int tetrismatrix::frames = 0;
bool tetrismatrix::vertical = true;
std::stack<Menu *> tetrismatrix::menus;

tetrismatrix::GameSettings tetrismatrix::settings;
GameModes tetrismatrix::selected_game_mode = GameModes::ZEN;
MatchSettings tetrismatrix::selected_match_settings;
Hub75 tetrismatrix::hub75(SCREEN_WIDTH, SCREEN_HEIGHT, nullptr, PANEL_GENERIC, false);
PicoGraphics_PenRGB888 tetrismatrix::graphics(hub75.width, hub75.height, nullptr);
Language tetrismatrix::past_language;

void tetrismatrix::initGame()
{
    EnumOption default_language = {(int)Language::ES, {"ENGLISH", "ESPANOL"}};
    settings.current_language = default_language;
    past_language = (Language)std::get<EnumOption>(settings.current_language).value;
    stdio_init_all();
    printf("starting\n");

    hub75.start(dma_complete);
    printf("hub75 started\n");

    // FOR NOW COMPLETELY BROKEN. better implement this in the cloud smh
    //DatabaseManager::init();
        
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

    //DatabaseManager::close();

    
}

void tetrismatrix::updateGame()
{
    // printf("updating game!!\n\n");
    
  

    while (InputManager::controllers.empty())
    {
        InputManager::searchForControllers();
    }

    InputManager::scanKeys(); 

    if(!menus.empty())
    {
        if(menus.top()->menu_type.has_value())
        {
            if(past_language != (Language)std::get<EnumOption>(settings.current_language).value)
            {
                std::stack<Menus> menu_types;
                while(!menus.empty())
                {
                    menu_types.push(*menus.top()->menu_type);
                    menus.pop();
                }
                while(!menu_types.empty())
                {
                    menus.push(createMenu(menu_types.top()));
                    menu_types.pop();
                }
                past_language = (Language)std::get<EnumOption>(settings.current_language).value;
            }
        }
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
    Language current_language = (Language)std::get<EnumOption>(settings.current_language).value;
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
                    true),

                Menu::Option(
                    translate(current_language, Texts::OPTIONS),
                    [](){menus.push(createMenu(Menus::OPTIONS)); InputManager::clearAllInputs();},
                    true),

                Menu::Option(
                    translate(current_language, Texts::SCORES),
                    [](){menus.push(createMenu(Menus::SCORES)); InputManager::clearAllInputs();},
                    true),
            }
        };

            title = vertical ? "TETRIS MATRIX" : "TETRISMATRIX";
            break;
        case Menus::OPTIONS:

         options = {
            {
                Menu::Option(
                    translate(current_language, Texts::VOLUME_SFX),
                    []()
                    {InputManager::clearAllInputs();},
                    (OptionValue*)&settings.volume_sfx,
                    0,
                    100,
                    true),

                Menu::Option(
                    translate(current_language, Texts::VOLUME_MUSIC),
                    []()
                    {InputManager::clearAllInputs();},
                    (OptionValue*)&settings.volume_music,
                    0,
                    100,
                    true),

                Menu::Option(
                    translate(current_language, Texts::LANGUAGE),
                    []()
                    {InputManager::clearAllInputs();},
                    (OptionValue*)&settings.current_language,
                    0,
                    (int)Language::SIZE - 1,
                    true),

                    Menu::Option(
                    translate(current_language, Texts::SAVE),
                    [current_language]()
                    {
                        hub75.clear();
                        std::string saved_text = translate(current_language, Texts::SAVED) + "!";
                        draw4x3Text(hub75, graphics, saved_text, 1, 10, 255, 255, 255, tetrismatrix::vertical);
                        /*
                        hub75.stop(dma_complete);
                        DatabaseManager::saveOrUpdateGameSettings(
                            std::get<int>(settings.volume_music),
                            std::get<int>(settings.volume_sfx),
                            current_language
                        );
                        sqlite3_exec(DatabaseManager::database, "PRAGMA wal_checkpoint(FULL);", nullptr, nullptr, nullptr);
                        hub75.start(dma_complete);
                        */
                        sleep_ms(1000);
                        InputManager::clearAllInputs();
                    },
                    true),
            }
        };
            title = translate(current_language, Texts::OPTIONS);
            break;
        case Menus::SCORES:
            title = translate(current_language, Texts::SCORES);
            break;

        case Menus::PAUSE:
            options = {
                // APPARENTLY it is only needed to clear the screen here. dk why
                Menu::Option(
                translate(current_language, Texts::RESUME),
                []()
                { menus.pop(); current_match->paused = false; hub75.clear(); InputManager::clearAllInputs();},
                true),

                Menu::Option(
                translate(current_language, Texts::EXIT),
                []()
                { if(current_match != nullptr) delete current_match; current_match = nullptr; menus.pop(); menus.top()->hide_menu = false; InputManager::clearAllInputs(); if(!tetrismatrix::vertical) tetrismatrix::vertical = true;},
                true)
            };
            title = translate(current_language, PAUSE);
            break;

        case Menus::GAME_MODE_SELECTION:
        options = {
            {
                Menu::Option(
                    translate(current_language, Texts::ZEN),
                    []()
                    {selected_game_mode = GameModes::ZEN; selected_match_settings = default_settings[GameModes::ZEN]; menus.push(createMenu(Menus::MATCH_SETTINGS)); InputManager::clearAllInputs();},
                    true),

                Menu::Option(
                    translate(current_language, Texts::MARATHON),
                    []()
                    {selected_game_mode = GameModes::MARATHON; selected_match_settings = default_settings[GameModes::MARATHON]; menus.push(createMenu(Menus::MATCH_SETTINGS)); InputManager::clearAllInputs();}, 
                    true),

                Menu::Option(
                    translate(current_language, Texts::VERSUS),
                    []()
                    {selected_game_mode = GameModes::VERSUS; selected_match_settings = default_settings[GameModes::VERSUS]; menus.push(createMenu(Menus::MATCH_SETTINGS)); InputManager::clearAllInputs();},
                    true),

                Menu::Option(
                    translate(current_language, Texts::COOP),
                    []()
                    {selected_game_mode = GameModes::COOP; selected_match_settings = default_settings[GameModes::COOP]; menus.push(createMenu(Menus::MATCH_SETTINGS)); InputManager::clearAllInputs();},
                    true),

                Menu::Option(
                    translate(current_language, Texts::DUAL_BOARD),
                    []()
                    {selected_game_mode = GameModes::DUAL_BOARD; selected_match_settings = default_settings[GameModes::DUAL_BOARD]; menus.push(createMenu(Menus::MATCH_SETTINGS)); InputManager::clearAllInputs();}, 
                    true),
            }
        };
            title = translate(current_language, Texts::GAME_MODE);
            break;

        case Menus::MATCH_SETTINGS:
            options = {
            {
                    Menu::Option(
                    translate(current_language, Texts::VELOCITY),
                    []()
                    { InputManager::clearAllInputs();},
                    (OptionValue*)&selected_match_settings.velocity,
                    1,
                    10,
                    true)
            }
        };
            if(selected_game_mode == GameModes::ZEN)
            {
                options.push_back(
                Menu::Option(
                    translate(current_language, Texts::UPCOMING_PIECES),
                    []()
                    { InputManager::clearAllInputs();},
                    (OptionValue*)&selected_match_settings.max_amount_upcoming_pieces,
                    1,
                    10,
                    true));

                options.push_back(
                Menu::Option(
                    translate(current_language, Texts::GAME_SCALE),
                    []()
                    { InputManager::clearAllInputs();},
                    (OptionValue*)&selected_match_settings.game_scale,
                    1,
                    5,
                    true)
                );

                options.push_back(
                Menu::Option(
                    translate(current_language, Texts::BOARD_WIDTH),
                    []()
                    { InputManager::clearAllInputs();},
                    (OptionValue*)&selected_match_settings.board_width,
                    5,
                    16,
                    true)
                );

                options.push_back(
                Menu::Option(
                    translate(current_language, Texts::BOARD_HEIGHT),
                    []()
                    { InputManager::clearAllInputs();},
                    (OptionValue*)&selected_match_settings.board_height,
                    10,
                    32,
                    true)
                );
                
                options.push_back(
                Menu::Option(
                    translate(current_language, Texts::LOCK_DELAY),
                    []()
                    { InputManager::clearAllInputs();},
                    (OptionValue*)&selected_match_settings.lock_delay,
                    0,
                    60,
                    true));
            }
                    

            if(selected_game_mode == GameModes::MARATHON)
                options.push_back(Menu::Option(
                    translate(current_language, Texts::MAX_VELOCITY),
                    []()
                    { InputManager::clearAllInputs();},
                    (OptionValue*)&selected_match_settings.max_velocity,
                    2,
                    10,
                    true));

            if(selected_game_mode == GameModes::VERSUS)
                {
                    options.push_back(Menu::Option(
                    translate(current_language, Texts::GARBAGE_MULTIPLIER),
                    []()
                    { InputManager::clearAllInputs();}, 
                    (OptionValue*)&selected_match_settings.garbage_multiplier,
                    1,
                    100,
                    true));

                    options.push_back(Menu::Option(
                    translate(current_language, Texts::GARBAGE_BLOCKING),
                    []()
                    { InputManager::clearAllInputs();},
                    (OptionValue*)&selected_match_settings.garbage_blocking,
                    1,
                    100,
                    true));
                }

            options.push_back(Menu::Option(
                    translate(current_language, Texts::START),
                    []()
                    { menus.pop(); menus.top()->hide_menu = true; current_match = new Match(selected_match_settings, selected_game_mode); InputManager::clearAllInputs();},
                    true));
            title = translate(current_language, Texts::SETTINGS);
    
            break;
            case Menus::GAME_OVER:
                options = {
            {
                Menu::Option(
                    translate(current_language, Texts::RETRY),
                    []()
                    {menus.push(createMenu(Menus::MATCH_SETTINGS)); InputManager::clearAllInputs();},
                    true),

                Menu::Option(
                    translate(current_language, Texts::EXIT),
                    []()
                    {if(current_match != nullptr) delete current_match; current_match = nullptr; menus.pop(); menus.top()->hide_menu = false; InputManager::clearAllInputs(); if(!tetrismatrix::vertical) tetrismatrix::vertical = true; InputManager::clearAllInputs();}, 
                    true)
            }
        };
            title = translate(current_language, Texts::GAME_OVER);
                break;

    }


    menu = new Menu(title, options, nullptr);


    // assign callback function and rest of the stuff.
    switch(menu_type)
    {
        case Menus::MAIN_MENU:
            menu->callback = [menu]() {
            static std::array<int, 3> title_rgb_increments = {2, 3, 5};
            rainbowEffect(menu->title_color, title_rgb_increments);
            };
            break;
        case Menus::OPTIONS:
            menu->options[2].on_click = [](){past_language = (Language)std::get<EnumOption>(settings.current_language).value;};
            break;
        case Menus::SCORES:
            break;
        case Menus::PAUSE:
            menu->callback = [menu](){
                if(InputManager::keys_pressed[InputManager::controllers[0]] & KEY_START)
                {
                    tetrismatrix::menus.pop();
                    InputManager::clearAllInputs();
                    current_match->paused = false;
                    hub75.clear();
                }
                if(InputManager::keys_pressed[InputManager::controllers[0]] & KEY_B)
                {
                    if(current_match != nullptr) 
                        delete current_match; 
                    current_match = nullptr; 
                    menus.pop(); 
                    menus.top()->hide_menu = false; 
                    InputManager::clearAllInputs();
                }
            };
            break;
            case Menus::GAME_MODE_SELECTION:
                menu->callback = [](){};
                break;
            case Menus::GAME_OVER:
                menu->callback = [menu](){
                if(InputManager::keys_pressed[InputManager::controllers[0]] & KEY_START)
                {
                    menu->selected_index = 0;
                    menu->executeSelectedOption();
                }
                if(InputManager::keys_pressed[InputManager::controllers[0]] & KEY_B)
                {
                    menu->selected_index = 1;
                    menu->executeSelectedOption();
                }
            };
                break;
    }
    menu->menu_type = menu_type;
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
