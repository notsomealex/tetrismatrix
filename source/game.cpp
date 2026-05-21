#include "game.h"
#include "match.h"
#include "menu.h"
#include "language.h"
#include "helpers.h"
#include "inputmanager.h"
#include "networkmanager.h"
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

    if (cyw43_arch_init())
    {
        printf("failed to initialise cyw43_arch\n");
    }

    // FOR NOW COMPLETELY BROKEN. better implement this in the cloud smh
    // DatabaseManager::init();

    if (!InputManager::init())
    {
        while (1)
        {
            printf("bluetooth input failed! cannot continue\n\n");
            sleep_ms(1000);
        }
    }
    printf("bluetooth input started!\n");

    while (InputManager::controllers.empty())
    {
        InputManager::searchForControllers();
    }


    Menu *main_menu = createMenu(Menus::MAIN_MENU);
    menus.push(main_menu);

    // DatabaseManager::close();
}

void tetrismatrix::updateGame()
{
    // printf("updating game!!\n\n");

    while (InputManager::controllers.empty())
    {
        InputManager::searchForControllers();
    }

    InputManager::scanKeys();

    if (!menus.empty())
    {
        if (menus.top()->menu_type.has_value())
        {
            if (past_language != (Language)std::get<EnumOption>(settings.current_language).value)
            {
                std::stack<Menus> menu_types;
                while (!menus.empty())
                {
                    menu_types.push(*menus.top()->menu_type);
                    popMenu();
                }
                while (!menu_types.empty())
                {
                    menus.push(createMenu(menu_types.top()));
                    menu_types.pop();
                }
                past_language = (Language)std::get<EnumOption>(settings.current_language).value;
            }
        }
        menus.top()->updateMenu(graphics, hub75);
    }
    if (current_match != nullptr)
        current_match->updateMatch();

    frames++;

    // printf("frames: %i\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", frames);
}

Menu *tetrismatrix::createMenu(Menus menu_type)
{
    std::vector<Menu::Option> options;
    std::string title;
    int pos_x;
    int pos_y;
    Language current_language = (Language)std::get<EnumOption>(settings.current_language).value;
    Menu *menu = nullptr;
    switch (menu_type)
    {
        case Menus::MAIN_MENU:
            options = {
                {
                    Menu::Option(
                            translate(current_language, Texts::START),
                            []()
                            {menus.push(createMenu(Menus::GAME_MODE_SELECTION)); InputManager::clearAllInputs(); },
                            true),

                    Menu::Option(
                            translate(current_language, Texts::OPTIONS),
                            []()
                            {menus.push(createMenu(Menus::OPTIONS)); InputManager::clearAllInputs(); },
                            true),

                    Menu::Option(
                            translate(current_language, Texts::SCORES),
                            []()
                            {menus.push(createMenu(Menus::SCORES)); InputManager::clearAllInputs(); },
                            true),
                }};

            title = vertical ? "TETRIS MATRIX" : "TETRISMATRIX";
            break;
        case Menus::OPTIONS:

            options = {
                {
                    Menu::Option(
                            translate(current_language, Texts::VOLUME_SFX),
                            []()
                            { InputManager::clearAllInputs(); },
                            (OptionValue *)&settings.volume_sfx,
                            0,
                            100,
                            true),

                    Menu::Option(
                            translate(current_language, Texts::VOLUME_MUSIC),
                            []()
                            { InputManager::clearAllInputs(); },
                            (OptionValue *)&settings.volume_music,
                            0,
                            100,
                            true),

                    Menu::Option(
                            translate(current_language, Texts::LANGUAGE),
                            []()
                            { InputManager::clearAllInputs(); },
                            (OptionValue *)&settings.current_language,
                            0,
                            (int)Language::SIZE - 1,
                            true),
                    Menu::Option(
                            translate(current_language, Texts::CONNECT_TO_NETWORK),
                            []()
                            { 
                            NetworkManager::enable();
                            NetworkManager::getNearNetworks();
                            NetworkManager::disable();

                            menus.push(createMenu(Menus::CONNECT_TO_NETWORK));
                            InputManager::clearAllInputs(); 
                            },
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
                }};
            title = translate(current_language, Texts::OPTIONS);
            break;
            case Menus::SCORES:
            {
                
                title = translate(current_language, Texts::SCORES);
                if(!NetworkManager::enable())
                {
                    std::string text = translate(current_language, Texts::CONNECT_TO_NETWORK) + " " + translate(current_language, Texts::FIRST);
                    showMessage(tetrismatrix::hub75, text, 2000, tetrismatrix::vertical);
                    text = translate(current_language, Texts::GO) + " " + translate(current_language, Texts::TO) + " " + translate(current_language, Texts::OPTIONS);
                    showMessage(tetrismatrix::hub75, text, 2000, tetrismatrix::vertical);
                    break;
                }
                hub75.clear();
                std::string message = translate(current_language, Texts::GETTING) + " " + translate(current_language, Texts::SCORES);
                showMessage(hub75, message, 0, vertical);
                printf("getting entries...\n\n");
                auto entries = DatabaseManager::getHigherScoreEntries(10);
                for(auto& entry : entries)
                {
                    auto player = DatabaseManager::getPlayer(entry.player_id);
                    if(!player.has_value()) 
                    {
                        printf("player %d not found, skipping\n", entry.player_id);
                        continue;
                    }
                    auto match = DatabaseManager::getMatch(entry.match_id);
                    std::string option_text = player.value().name + ": " + std::to_string(entry.score);
                    printf("option_text: %s", option_text.c_str());

                    auto show_results = std::make_shared<std::function<void()>>(
                            [=]()
                            {
                            Language current_language = (Language)std::get<EnumOption>(tetrismatrix::settings.current_language).value;

                            std::vector<Menu::Option> options =
                            {
                            Menu::Option(translate(current_language, Texts::NAME) + ": " + player.value().name, 
                                    [](){},
                                    true
                                    ),
                            Menu::Option(translate(current_language, Texts::SCORE) + ": " + std::to_string(entry.score),
                                    [](){},
                                    true
                                    ),
                            Menu::Option(translate(current_language, Texts::LINES_CLEARED) + ": " + std::to_string(entry.lines_cleared),
                                    [](){},
                                    true
                                    ),
                            Menu::Option(translate(current_language, Texts::SINGLE) + "S: " + std::to_string(entry.singles),
                                    [](){},
                                    true
                                    ),
                            Menu::Option(translate(current_language, Texts::DOUBLE) + "S: " + std::to_string(entry.doubles),
                                    [](){},
                                    true
                                    ),
                            Menu::Option(translate(current_language, Texts::TRIPLE) + "S: " + std::to_string(entry.triples),
                                    [](){},
                                    true
                                    ),
                            Menu::Option(translate(current_language, Texts::TETRIS) + ": " + std::to_string(entry.tetris),
                                    [](){},
                                    true
                                    ),
                            };
                            menus.push(new Menu(translate(current_language, Texts::RESULTS), options, [](){}));
                            });
                    options.push_back(Menu::Option(option_text,
                                *show_results,
                                true));
                }
                break;
            }

        case Menus::PAUSE:
            options = {
                Menu::Option(
                        translate(current_language, Texts::RESUME),
                        []()
                        { popMenu(); current_match->paused = false; hub75.clear(); InputManager::clearAllInputs(); },
                        true),

                Menu::Option(
                        translate(current_language, Texts::EXIT),
                        []()
                        {
                        if (current_match != nullptr)
                        delete current_match;
                        current_match = nullptr;
                        popMenu();
                        menus.top()->hide_menu = false;
                        InputManager::clearAllInputs();
                        if (!tetrismatrix::vertical)
                        tetrismatrix::vertical = true;
                        },
                        true)};
            title = translate(current_language, PAUSE);
            break;

        case Menus::GAME_MODE_SELECTION:
            options = {
                {
                    Menu::Option(
                            translate(current_language, Texts::ZEN),
                            []()
                            {selected_game_mode = GameModes::ZEN; selected_match_settings = default_settings[GameModes::ZEN]; menus.push(createMenu(Menus::MATCH_SETTINGS)); InputManager::clearAllInputs(); },
                            true),

                    Menu::Option(
                            translate(current_language, Texts::MARATHON),
                            []()
                            {selected_game_mode = GameModes::MARATHON; selected_match_settings = default_settings[GameModes::MARATHON]; menus.push(createMenu(Menus::MATCH_SETTINGS)); InputManager::clearAllInputs(); },
                            true),

                    Menu::Option(
                            translate(current_language, Texts::VERSUS),
                            []()
                            {selected_game_mode = GameModes::VERSUS; selected_match_settings = default_settings[GameModes::VERSUS]; menus.push(createMenu(Menus::MATCH_SETTINGS)); InputManager::clearAllInputs(); },
                            true),

                    Menu::Option(
                            translate(current_language, Texts::COOP),
                            []()
                            {selected_game_mode = GameModes::COOP; selected_match_settings = default_settings[GameModes::COOP]; menus.push(createMenu(Menus::MATCH_SETTINGS)); InputManager::clearAllInputs(); },
                            true),

                }};
            title = translate(current_language, Texts::GAME_MODE);
            break;

        case Menus::MATCH_SETTINGS:
            options = {
                {Menu::Option(
                        translate(current_language, Texts::VELOCITY),
                        []()
                        { InputManager::clearAllInputs(); },
                        (OptionValue *)&selected_match_settings.velocity,
                        1,
                        15,
                        true)}};
            if (selected_game_mode == GameModes::ZEN)
            {
                options.push_back(
                        Menu::Option(
                            translate(current_language, Texts::UPCOMING_PIECES),
                            []()
                            { InputManager::clearAllInputs(); },
                            (OptionValue *)&selected_match_settings.max_amount_upcoming_pieces,
                            1,
                            10,
                            true));

                options.push_back(
                        Menu::Option(
                            translate(current_language, Texts::GAME_SCALE),
                            []()
                            { InputManager::clearAllInputs(); },
                            (OptionValue *)&selected_match_settings.game_scale,
                            1,
                            5,
                            true));

                options.push_back(
                        Menu::Option(
                            translate(current_language, Texts::BOARD_WIDTH),
                            []()
                            { InputManager::clearAllInputs(); },
                            (OptionValue *)&selected_match_settings.board_width,
                            5,
                            16,
                            true));

                options.push_back(
                        Menu::Option(
                            translate(current_language, Texts::BOARD_HEIGHT),
                            []()
                            { InputManager::clearAllInputs(); },
                            (OptionValue *)&selected_match_settings.board_height,
                            10,
                            32,
                            true));

                options.push_back(
                        Menu::Option(
                            translate(current_language, Texts::LOCK_DELAY),
                            []()
                            { InputManager::clearAllInputs(); },
                            (OptionValue *)&selected_match_settings.lock_delay,
                            0,
                            60,
                            true));
            }

            /*
               if(selected_game_mode == GameModes::MARATHON)
               options.push_back(Menu::Option(
               translate(current_language, Texts::MAX_VELOCITY),
               []()
               { InputManager::clearAllInputs();},
               (OptionValue*)&selected_match_settings.max_velocity,
               2,
               15,
               true));
               */

            if (selected_game_mode == GameModes::VERSUS)
            {
                options.push_back(Menu::Option(
                            translate(current_language, Texts::GARBAGE_MULTIPLIER),
                            []()
                            { InputManager::clearAllInputs(); },
                            (OptionValue *)&selected_match_settings.garbage_multiplier,
                            0,
                            10,
                            true));

                options.push_back(Menu::Option(
                            translate(current_language, Texts::GARBAGE_BLOCKING),
                            []()
                            { InputManager::clearAllInputs(); },
                            (OptionValue *)&selected_match_settings.garbage_blocking,
                            0,
                            10,
                            true));
            }

            options.push_back(Menu::Option(
                        translate(current_language, Texts::START),
                        []()
                        {current_match = new Match(selected_match_settings, selected_game_mode); InputManager::clearAllInputs(); popMenu(); menus.top()->hide_menu = true; },
                        true));
            title = translate(current_language, Texts::SETTINGS);

            break;
        case Menus::GAME_OVER:
            options = {
                {Menu::Option(
                        translate(current_language, Texts::RETRY),
                        []()
                        {if(current_match != nullptr) delete current_match; current_match = nullptr; menus.push(createMenu(Menus::MATCH_SETTINGS)); InputManager::clearAllInputs(); },
                        true),

                Menu::Option(
                        translate(current_language, Texts::EXIT),
                        // IDEALLY i'd like this to not make it go back to the main menu, but meh for now it will stick.
                        []()
                        {
                        if (current_match != nullptr)
                        delete current_match;
                        current_match = nullptr;
                        while (menus.size() > 1)
                        popMenu();
                        menus.top()->hide_menu = false;
                        InputManager::clearAllInputs();
                        if (!tetrismatrix::vertical)
                        tetrismatrix::vertical = true;
                        },
                        true)}};
            title = translate(current_language, Texts::GAME_OVER);
            break;
        case Menus::CONNECT_TO_NETWORK:
            {
                const size_t password_size = 63;
                for(auto& network : NetworkManager::found_networks)
                {

                    printf("creating option for: %s\n", network.ssid.c_str());
                    std::string upper_name = network.ssid;
                    std::transform(upper_name.begin(), upper_name.end(), upper_name.begin(), ::toupper);
                    if(network.auth_mode == 0)
                    {
                        printf("public network!!\n\n");
                        options.push_back(
                                Menu::Option(upper_name,
                                    [network]()
                                    {
                                    NetworkManager::enable();
                                    NetworkManager::connectToNetwork(network.ssid, "");
                                    NetworkManager::disable();
                                    }
                                    , true)
                                );
                    }
                    else
                    {
                        printf("private network!!\n\n");
                        auto password = std::make_shared<OptionValue>(std::string(""));
                        options.push_back(
                                Menu::Option(upper_name,
                                    [network, current_language, password, password_size]()
                                    {
                                    std::string& s = std::get<std::string>(*password);

                                    menus.top()->title = translate(current_language, Texts::PASSWORD);
                                    menus.top()->on_close =
                                    [password, network, current_language]()
                                    {
                                    trimTrailingSpaces(std::get<std::string>(*password));
 
                                    NetworkManager::enable();
                                    // FOR TESTING. REMOVE
                                    if(network.ssid == "Totalplay-21AA")
                                    *password = "21AA1A802RdsAgMZ";

                                    int result = NetworkManager::connectToNetwork(network.ssid, std::get<std::string>(*password));
                                    NetworkManager::disable();

                                    popMenu();
                                    InputManager::clearAllInputs();
                                    };
                                    }, 
                            password.get(), 
                            63,
                            63,
                            true)
                                );
                    }
                }
                title = translate(current_language, Texts::NETWORKS);
                break;
            }
    }

    menu = new Menu(title, options, nullptr);

    // assign callback function and rest of the stuff.
    switch (menu_type)
    {
        case Menus::MAIN_MENU:
            menu->callback = [menu]()
            {
                static std::array<int, 3> title_rgb_increments = {2, 3, 5};
                rainbowEffect(menu->title_color, title_rgb_increments);
            };
            break;
        case Menus::OPTIONS:
            menu->options[2].on_click = []()
            { past_language = (Language)std::get<EnumOption>(settings.current_language).value; };
            break;
        case Menus::SCORES:
        /*
            menu->callback =
            [menu, current_language]()
            {
                static bool fetched = false;
                if(!fetched) 
                {
                printf("enabling network...\n\n");
                NetworkManager::enable();
                printf("getting entries...\n\n");
                auto entries = DatabaseManager::getHigherScoreEntries(1);
                for(auto& entry : entries)
                {
                    printf("processing entry...\n\n");
                    auto player = DatabaseManager::getPlayer(entry.player_id);
                    printf("player gotten\n\n");
                    auto match = DatabaseManager::getMatch(entry.match_id);
                    printf("match gotten\n\n");
                    std::string option_text = ""
                    if(player.has_value())
                        option_text += player.value().name + ": ";
                    else
                        option_text += "player: ";
                    
                    option_text += std::to_string(entry.score);
                    printf("declaring lambda...\n\n");
                    auto show_results = std::make_shared<std::function<void()>>(
                            [=]()
                            {
                            Language current_language = (Language)std::get<EnumOption>(tetrismatrix::settings.current_language).value;

                            std::vector<Menu::Option> options =
                            {
                            Menu::Option(translate(current_language, Texts::NAME) + ": " + player.value().name, 
                                    [](){},
                                    true
                                    ),
                            Menu::Option(translate(current_language, Texts::SCORE) + ": " + std::to_string(entry.score),
                                    [](){},
                                    true
                                    ),
                            Menu::Option(translate(current_language, Texts::LINES_CLEARED) + ": " + std::to_string(entry.lines_cleared),
                                    [](){},
                                    true
                                    ),
                            Menu::Option(translate(current_language, Texts::SINGLE) + "S: " + std::to_string(entry.singles),
                                    [](){},
                                    true
                                    ),
                            Menu::Option(translate(current_language, Texts::DOUBLE) + "S: " + std::to_string(entry.doubles),
                                    [](){},
                                    true
                                    ),
                            Menu::Option(translate(current_language, Texts::TRIPLE) + "S: " + std::to_string(entry.triples),
                                    [](){},
                                    true
                                    ),
                            Menu::Option(translate(current_language, Texts::TETRIS) + ": " + std::to_string(entry.tetris),
                                    [](){},
                                    true
                                    ),
                            };
                            });
                            printf("pushing option...\n\n");
                    menu->options.push_back(Menu::Option(translate(current_language, Texts::SCORE),
                                *show_results,
                                true));
                                
                }
                printf("finished!\n\n");
            }
            fetched = true;
        };
        */
        break;
        case Menus::PAUSE:
            menu->callback = [menu]()
            {
                if (InputManager::keys_pressed[InputManager::controllers[0]] & KEY_START)
                {
                    tetrismatrix::popMenu();
                    InputManager::clearAllInputs();
                    current_match->paused = false;
                    hub75.clear();
                }
                if (InputManager::keys_pressed[InputManager::controllers[0]] & KEY_B)
                {
                    if (current_match != nullptr)
                        delete current_match;
                    current_match = nullptr;
                    popMenu();
                    menus.top()->hide_menu = false;
                    InputManager::clearAllInputs();
                }
            };
            break;
        case Menus::GAME_MODE_SELECTION:
            menu->callback = []() {};
            break;
        case Menus::GAME_OVER:
            menu->callback = [menu]()
            {
                if (InputManager::keys_pressed[InputManager::controllers[0]] & KEY_START)
                {
                    menu->selected_index = 0;
                    menu->executeSelectedOption();
                }
                if (InputManager::keys_pressed[InputManager::controllers[0]] & KEY_B)
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

inline void tetrismatrix::popMenu()
{
    if (menus.empty())
        return;
    if (menus.top() != nullptr)
        delete menus.top();

    printf("size before pop: %i\n", menus.size());
    menus.pop();
    printf("size after pop: %i\n", menus.size());
}

int tetrismatrix::screenWidth()
{
    return tetrismatrix::vertical ? SCREEN_HEIGHT : SCREEN_WIDTH;
}

int tetrismatrix::screenHeight()
{
    return tetrismatrix::vertical ? SCREEN_WIDTH : SCREEN_HEIGHT;
}
