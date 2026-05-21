#include "match.h"
#include "board.h"
#include "piece.h"
#include "menu.h"
#include "helpers.h"
#include "language.h"
#include "inputmanager.h"
#include "databasemanager.h"
#include "networkmanager.h"
#include "types.h"

void Match::initMatch()
{
    srand(to_ms_since_boot(get_absolute_time()));



    // main boards and pieces.
    boards.push_back(new Board(std::get<int>(settings.board_width), std::get<int>(settings.board_height), std::get<int>(settings.game_scale)));
    pieces.push_back(new Piece(boards[0], std::get<int>(settings.game_scale), 30 / std::get<int>(settings.velocity), std::get<int>(settings.lock_delay), InputManager::controllers[0]));

    switch(game_mode)
    {
        case GameModes::VERSUS:
            tetrismatrix::vertical = false;
            while(InputManager::controllers.size() != 2)
                InputManager::searchForControllers();
            boards.push_back(new Board(std::get<int>(settings.board_width), std::get<int>(settings.board_height), std::get<int>(settings.game_scale)));
            pieces.push_back(new Piece(boards[1], std::get<int>(settings.game_scale), 30 / std::get<int>(settings.velocity), std::get<int>(settings.lock_delay), InputManager::controllers[1]));

            boards[0]->board_pos_x += -1 + (boards[0]->board_pos_x * std::get<int>(settings.game_scale)) / 2;
            boards[0]->board_pos_y = 0;
            boards[1]->board_pos_x = 3 * boards[0]->board_pos_x + 2;
            boards[1]->board_pos_y = 0;
            break;
        case GameModes::COOP:
            tetrismatrix::vertical = false;
            while(InputManager::controllers.size() != 2)
                InputManager::searchForControllers();
            pieces.push_back(new Piece(boards[0], std::get<int>(settings.game_scale), 30 / std::get<int>(settings.velocity), std::get<int>(settings.lock_delay), InputManager::controllers[1]));
            break;
        case GameModes::MARATHON:

            break;
    }

    for(int i = 0; i < pieces.size(); i++)
    {
        setUpcomingPieces(pieces[i]);
    }

    tetrismatrix::hub75.clear();

    for(auto piece : pieces)
    {
        names[piece] = "";
    }

}

void Match::endMatch()
{
    for(int i = 0; i < boards.size(); i++)
    {
        delete boards[i];
        delete pieces[i];
    }
    tetrismatrix::menus.top()->hide_menu = false;
}

void Match::updateMatch()
{
    if(paused || ended)
        return;

    int16_t& keys_pressed = InputManager::keys_pressed[InputManager::controllers[0]];


    if(keys_pressed & KEY_START)
    {
        menus.push(createMenu(Menus::PAUSE));
        paused = true;
    }

    //printf("updating match!");

    for(int i = 0; i < boards.size(); i++)
    {
        boards[i]->updateBoard();

    }

    for(int i = 0; i < pieces.size(); i++)
    {
        if(pieces[i]->just_solidified)
        {
            updateUpcomingPieces(pieces[i]);
            pieces[i]->just_solidified = false;
            scores[pieces[i]] += 5 + ((std::get<int>(settings.velocity) - 1));
            scores[pieces[i]] += pieces[i]->board->cleared_lines * 10 * std::get<int>(settings.velocity);
            total_lines_cleared[pieces[i]] += pieces[i]->board->cleared_lines;
            current_velocity_lines_cleared[pieces[i]] += pieces[i]->board->cleared_lines;

            if(pieces[i]->board->cleared_lines > 0)
            {
                switch (pieces[i]->board->cleared_lines)
                {
                    case 1:
                        singles_amount[pieces[i]]++;
                        break;
                    case 2:
                        doubles_amount[pieces[i]]++;
                        break;
                    case 3:
                        triples_amount[pieces[i]]++;
                        break;
                    case 4:
                        tetris_amount[pieces[i]]++;
                        break;
                }
            }
        }
        pieces[i]->updatePiece();
    }



    // DIFFERENT GAME MODES //

    switch(game_mode)
    {
        case GameModes::ZEN:
            if (pieces[0]->checkCollision() & COLLISION_DIRECTION::UP)
            {
                boards[0]->resetBoard();
                pieces[0]->restartPiece();
            }
            break;
        case GameModes::MARATHON:
            {
                showUpcomingPieces(pieces[0], pieces[0]->board->board_pos_x, pieces[0]->board->board_pos_y - 10, false, false);
                showScore(pieces[0], pieces[0]->board->board_pos_x - 2, pieces[0]->board->board_pos_y - 2, true, true);
                showHoldPiece(pieces[0], pieces[0]->board->board_pos_x + (SCREEN_HEIGHT / 2), pieces[0]->board->board_pos_y - 2, true, true);
                int& velocity = std::get<int>(settings.velocity);
                int& max_velocity = std::get<int>(settings.max_velocity);
                //printf("lines: %i", current_velocity_lines_cleared[boards[0]]);
                //printf("velocity: %i", velocity);
                if(current_velocity_lines_cleared[pieces[0]] > 20)
                {
                    if(velocity < max_velocity)
                    {
                        velocity++;
                        pieces[0]->falling_frequency = 30 / velocity;
                        current_velocity_lines_cleared[pieces[0]] = 0;
                    }
                    else
                    {
                        gameWin();
                    }
                }
                if (pieces[0]->checkCollision() & COLLISION_DIRECTION::UP)
                {
                    gameOver();
                }
                break;
            }
        case GameModes::VERSUS:
            {
                static std::unordered_map<Piece*, int> garbage_values;
                static std::unordered_map<Piece*, bool> sends_garbage_next; 

                while(InputManager::controllers.size() != 2)
                    InputManager::searchForControllers();
                showUpcomingPieces(pieces[0], pieces[0]->board->board_pos_x - 5, pieces[0]->board->board_pos_y, true, false);
                showUpcomingPieces(pieces[1], pieces[1]->board->board_pos_x + (pieces[1]->board->board_width) + 1, pieces[1]->board->board_pos_y, true, false);

                if(pieces[0]->checkCollision() & COLLISION_DIRECTION::UP || pieces[1]->checkCollision() & COLLISION_DIRECTION::UP)
                    gameOver();

                for(auto& piece : pieces)
                {
                    if(piece->just_solidified)
                    {
                        if(piece->board->cleared_lines > 1)
                        {
                            garbage_values[piece] += piece->board->cleared_lines * std::get<int>(settings.garbage_multiplier);
                            sends_garbage_next[piece] = true;
                        }
                    }
                }


                std::function<void(Piece*, Piece*)> send_garbage = 
                    [this](Piece* sender_piece, Piece* receiver_piece)
                    {
                        int lines_to_send = 0;
                        int lines_avoided = 0;
                        if(sends_garbage_next[sender_piece] && receiver_piece->just_solidified)
                        {
                            lines_avoided = ((garbage_values[receiver_piece]) / 8);
                            lines_to_send = garbage_values[sender_piece] / 8;
                            if(lines_avoided > lines_to_send) lines_avoided = lines_to_send;
                            lines_to_send -= lines_avoided;
                            garbage_values[receiver_piece] -= lines_avoided * std::get<int>(settings.garbage_blocking);

                            receiver_piece->board->addGarbageLines(garbage_values[sender_piece] / 4);
                            garbage_values[sender_piece] = 0;
                            sends_garbage_next[sender_piece] = false;
                        }
                    };
                send_garbage(pieces[0], pieces[1]);
                send_garbage(pieces[1], pieces[0]);
                //printf("lines to send: %i", lines_to_send);
                break;
            }
        case GameModes::COOP:
            while(InputManager::controllers.size() != 2)
                InputManager::searchForControllers();
            showUpcomingPieces(pieces[0], pieces[0]->board->board_pos_x, pieces[0]->board->board_pos_y - 10, true, false);
            showUpcomingPieces(pieces[1], pieces[1]->board->board_pos_x + 10, pieces[1]->board->board_pos_y - 10, true, false);
            break;
    }
}

void Match::setUpcomingPieces(Piece* piece)
{
    for(int i = 0; i < std::get<int>(settings.max_amount_upcoming_pieces); i++)
    {
        piece->upcoming_pieces.push_back((PIECES)((rand() % 7) + 1));
    }
    piece->next_piece = piece->upcoming_pieces.front();
    piece->upcoming_pieces.pop_front();

}

void Match::updateUpcomingPieces(Piece* piece)
{
    PIECES next_piece = piece->upcoming_pieces.front();
    piece->upcoming_pieces.pop_front();
    piece->upcoming_pieces.push_back((PIECES)((rand() % 7) + 1));
    piece->next_piece = next_piece;
}

void Match::showUpcomingPieces(Piece* piece, int pos_x, int pos_y, bool vertical_layout, bool show_text)
{
    std::string drawn_next_text = "";

    if(show_text)
    {
        Language current_language = (Language)std::get<EnumOption>(tetrismatrix::settings.current_language).value;
        drawn_next_text = translate(current_language, Texts::NEXT);
        cropTextToFit(drawn_next_text, 3, 15);    
    }
    else
    {
        pos_y -= 2 / piece->scale;
    }

    if(vertical_layout)
        clearRegionOfScreen(tetrismatrix::hub75, 
                pos_x * piece->scale, 
                pos_y * piece->scale, 
                (pos_x + 4) * piece->scale, 
                (pos_y + 4 * (piece->upcoming_pieces.size() + 1)) * piece->scale, 
                tetrismatrix::vertical);
    else
        clearRegionOfScreen(tetrismatrix::hub75, 
                (pos_x - 4) * piece->scale, 
                pos_y * piece->scale, 
                (pos_x + 4 * (piece->upcoming_pieces.size() + 1)) * piece->scale, 
                (pos_y + 4 + 2) * piece->scale, 
                tetrismatrix::vertical);

    if(show_text)
        draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, drawn_next_text, pos_x - 1, pos_y - 1, 255, 255, 255, tetrismatrix::vertical);


    const int spacing = 2 * piece->scale;

    if(!vertical_layout)
        pos_x -= 4 / piece->scale;
    piece->printPiece(shapes[piece->next_piece], pos_x - piece->board->board_pos_x, pos_y - piece->board->board_pos_y, piece->scale);
    for(int i = 0; i < piece->upcoming_pieces.size(); i++)
    {
        int piece_pos_x = vertical_layout ? pos_x : pos_x + (spacing * (i + 1));
        int piece_pos_y = vertical_layout ? pos_y + (spacing * (i + 1)) : pos_y;
        piece->printPiece(shapes[piece->upcoming_pieces[i]], piece_pos_x - piece->board->board_pos_x, piece_pos_y - piece->board->board_pos_y, piece->scale);
    }



}

void Match::showScore(Piece* piece, int pos_x, int pos_y, bool vertical_layout, bool show_text)
{
    std::string drawn_score_text = "";
    if(show_text)
    {
        Language current_language = (Language)std::get<EnumOption>(tetrismatrix::settings.current_language).value;
        drawn_score_text = translate(current_language, Texts::SCORE);
        cropTextToFit(drawn_score_text, 4, 20);
    }
    size_t score_length = std::to_string(scores[piece]).size();
    size_t score_text_length = drawn_score_text.size();
    int score_pos_x = vertical_layout ? pos_x + score_length: pos_x + (drawn_score_text.size() * 4) + (score_length - 1);
    int score_pos_y = vertical_layout ? pos_y + 5 : pos_y;

    if(vertical_layout)
        clearRegionOfScreen(tetrismatrix::hub75, 
                pos_x, 
                pos_y + 4, 
                pos_x + std::max(drawn_score_text.size(), score_length) * 4 + score_pos_x, 
                pos_y + (4 * 2), 
                tetrismatrix::vertical);
    else
        clearRegionOfScreen(tetrismatrix::hub75, 
                score_pos_x, 
                pos_y, 
                score_pos_x + score_length, 
                pos_y + 4, 
                tetrismatrix::vertical);
    if(show_text)
        draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, drawn_score_text, pos_x, pos_y, 255, 255, 255, tetrismatrix::vertical);
    draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, std::to_string(scores[piece]), score_pos_x, score_pos_y, 255, 255, 255, tetrismatrix::vertical);
}

void Match::showHoldPiece(Piece* piece, int pos_x, int pos_y, bool vertical_layout, bool show_text)
{
    std::string drawn_hold_text = "";

    if(show_text)
    {
        Language current_language = (Language)std::get<EnumOption>(tetrismatrix::settings.current_language).value;
        drawn_hold_text = translate(current_language, Texts::HOLD);
        cropTextToFit(drawn_hold_text, 3, 15);
        draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, drawn_hold_text, pos_x - 1, pos_y - 1, 255, 255, 255, tetrismatrix::vertical);
    }

    int piece_pos_x = vertical_layout ? pos_x + 2 : pos_x + drawn_hold_text.size() * 3;
    int piece_pos_y = vertical_layout ? pos_y + (2 * piece->scale) - 1 : pos_y;
    if(vertical_layout)
        clearRegionOfScreen(tetrismatrix::hub75, 
                pos_x, 
                piece_pos_y, 
                pos_x + (4 * piece->scale), 
                piece_pos_y + (4 * piece->scale), 
                tetrismatrix::vertical);
    else
        clearRegionOfScreen(tetrismatrix::hub75, 
                piece_pos_x, 
                piece_pos_y, 
                piece_pos_x + (4 * piece->scale), 
                piece_pos_y + (4 * piece->scale), 
                tetrismatrix::vertical);

    piece->printPiece(shapes[piece->hold_piece], (piece_pos_x / piece->scale) - piece->board->board_pos_x, (piece_pos_y / piece->scale) - piece->board->board_pos_y, piece->scale);
}


void Match::gameOver()
{
    tetrismatrix::hub75.clear();
    ended = true;
    Language current_language = (Language)std::get<EnumOption>(tetrismatrix::settings.current_language).value;

    switch(game_mode)
    {
        case GameModes::MARATHON:
            for(int i = 0; i < boards[0]->board_height; i++)
            {
                for(int j = 0; j < boards[0]->board_width; j++)
                {
                    if(boards[0]->board_matrix[i][j] == PIECES::NONE)
                        continue;
                    boards[0]->board_matrix[i][j] = PIECES::GHOST;
                    boards[0]->updateBoard();
                    sleep_ms(5);
                }
            }
            draw5x5Text(tetrismatrix::hub75, tetrismatrix::graphics, translate(current_language, Texts::GAME_OVER), 20, 20, 255, 255, 255, tetrismatrix::vertical);
            boards[0]->updateBoard();
            break;

        case GameModes::VERSUS:
            int gg_pos_x;
            int gg_pos_y = 10;
            Piece* loser_piece;
            Piece* winner_piece;
            std::string gg_text = translate(current_language, Texts::GG) + "!!!";
            pieces[0]->updatePiece();
            pieces[1]->updatePiece();

            if(pieces[0]->checkCollision() & COLLISION_DIRECTION::UP)
            {
                loser_piece = pieces[0];
                winner_piece = pieces[1];
                gg_pos_x = (pieces[1]->board->board_pos_x) * pieces[1]->scale + 1;
            }
            else
            {
                loser_piece = pieces[1];
                winner_piece = pieces[0];
                gg_pos_x = (pieces[0]->board->board_width / 2) * pieces[0]->scale + 1;
            }
            won[loser_piece] = false;
            won[winner_piece] = true;
            printf("\n\n\ngg_pos_x: %i", gg_pos_x);
            for(int i = 0; i < loser_piece->board->board_height; i++)
            {
                for(int j = 0; j < loser_piece->board->board_width; j++)
                {
                    winner_piece->board->board_matrix[i][j] = PIECES::NONE;
                    if(loser_piece->board->board_matrix[i][j] == PIECES::NONE)
                        continue;
                    loser_piece->board->board_matrix[i][j] = PIECES::GHOST;
                    loser_piece->board->updateBoard();
                    winner_piece->board->updateBoard();
                    sleep_ms(5);
                }
            }
            tetrismatrix::graphics.set_pen(0, 0, 0);
            tetrismatrix::graphics.clear();
            tetrismatrix::graphics.set_pen(255, 255, 255);
            tetrismatrix::graphics.text(gg_text, Point(gg_pos_x, gg_pos_y), 1, 1.0f, 0.0f);
            tetrismatrix::hub75.update(&graphics);

            pieces[0]->board->printBoard(false);
            pieces[1]->board->printBoard(false);
            break;

    }

    sleep_ms(3000);
    std::vector<Menu::Option> options;
    switch(game_mode)
    {
        case GameModes::MARATHON:
            options = 
            {
                Menu::Option(translate(current_language, Texts::ENTER_YOUR_NAME),
                        [](){InputManager::clearAllInputs();},
                        &names[pieces[0]],
                        10,
                        10,
                        true
                        )
            };
            won[pieces[0]] = false;
            tetrismatrix::menus.push(new Menu(options));
            tetrismatrix::menus.top()->executeSelectedOption();
            // OKAY. so for some fucking reason only god knows why, the this pointer is getting corrupted or something, so i have to do the trick that is below. dont ask me why, i do not know.
            tetrismatrix::menus.top()->on_close = 
                [this]()
                {
                    trimTrailingSpaces(std::get<std::string>(names[pieces[0]]));
                    auto self = this; 
                    tetrismatrix::popMenu(); 
                    tetrismatrix::popMenu(); 
                    tetrismatrix::menus.push(createMenu(Menus::GAME_OVER)); 
                    self->showResults(self->pieces[0]);
                    InputManager::clearAllInputs();};

            break;

        case GameModes::VERSUS:
            options = 
            {
                Menu::Option(
                        translate(current_language, Texts::PLAYER) + "2: " + translate(current_language, Texts::ENTER_YOUR_NAME),
                        [](){InputManager::clearAllInputs();},
                        &names[pieces[1]],
                        10,
                        10,
                        true
                        )
            };
            tetrismatrix::vertical = true;
            tetrismatrix::menus.push(new Menu(options));
            tetrismatrix::menus.top()->executeSelectedOption();
            tetrismatrix::menus.top()->on_close = 
                [this]()
                { 
                    trimTrailingSpaces(std::get<std::string>(names[pieces[1]]));
                    auto self = this; 
                    tetrismatrix::menus.push(createMenu(Menus::GAME_OVER)); 
                    self->showResults(self->pieces[1]);
                };


            options[0].name = translate(current_language, Texts::PLAYER) + "1: " + translate(current_language, Texts::ENTER_YOUR_NAME);
            options[0].associated_value = &names[pieces[1]];
            tetrismatrix::menus.push(new Menu(options));
            tetrismatrix::menus.top()->executeSelectedOption();
            tetrismatrix::menus.top()->on_close = 
                [this]()
                { 
                    trimTrailingSpaces(std::get<std::string>(names[pieces[0]]));
                    auto self = this; 
                    tetrismatrix::popMenu(); 
                    tetrismatrix::popMenu(); 
                    self->showResults(self->pieces[0]);
                };

            break;
    }
}
// default ending function for versus, no matter who loses or wins. yeahhhhhh
void Match::gameWin()
{
    ended = true;
    Language current_language = (Language)std::get<EnumOption>(tetrismatrix::settings.current_language).value;
    std::string gg_text = translate(current_language, Texts::GG) + "!!!";

    switch(game_mode)
    {
        case GameModes::MARATHON:
            draw5x5Text(tetrismatrix::hub75, tetrismatrix::graphics, gg_text, 20, 20, 255, 255, 255, tetrismatrix::vertical);
            boards[0]->updateBoard();
            won[pieces[0]] = true;
            break;
    }
    sleep_ms(3000);
    std::vector<Menu::Option> options;
    switch(game_mode)
    {
        case GameModes::MARATHON:
            options = 
            {
                Menu::Option(translate(current_language, Texts::ENTER_YOUR_NAME),
                        [](){InputManager::clearAllInputs();},
                        &names[pieces[0]],
                        10,
                        10,
                        true
                        )
            };
            tetrismatrix::menus.push(new Menu(options));
            tetrismatrix::menus.top()->executeSelectedOption();
            tetrismatrix::menus.top()->on_close = 
                [this]()
                {
                    auto self = this; 
                    tetrismatrix::popMenu(); 
                    tetrismatrix::menus.push(createMenu(Menus::GAME_OVER)); 
                    self->showResults(self->pieces[0]); 
                    InputManager::clearAllInputs();
                };

            break;

        case GameModes::VERSUS:
            // this case is at the gameOver function.
            break;
    }


}

// i dont really like that much that the piece represents the player but yeah sorry:/
void Match::showResults(Piece* player_piece)
{ 

    Language current_language = (Language)std::get<EnumOption>(tetrismatrix::settings.current_language).value;

    std::vector<Menu::Option> options =
    {
        Menu::Option(translate(current_language, Texts::NAME) + ": " + std::get<std::string>(names[player_piece]), 
                [](){},
                true
                ),
        Menu::Option(translate(current_language, Texts::SCORE) + ": " + std::to_string(scores[player_piece]),
                [](){},
                true
                ),
        Menu::Option(translate(current_language, Texts::LINES_CLEARED) + ": " + std::to_string(total_lines_cleared[player_piece]),
                [](){},
                true
                ),
        Menu::Option(translate(current_language, Texts::SINGLE) + "S: " + std::to_string(singles_amount[player_piece]),
                [](){},
                true
                ),
        Menu::Option(translate(current_language, Texts::DOUBLE) + "S: " + std::to_string(doubles_amount[player_piece]),
                [](){},
                true
                ),
        Menu::Option(translate(current_language, Texts::TRIPLE) + "S: " + std::to_string(triples_amount[player_piece]),
                [](){},
                true
                ),
        Menu::Option(translate(current_language, Texts::TETRIS) + ": " + std::to_string(tetris_amount[player_piece]),
                [](){},
                true
                ),
    };

    auto upload_results = std::make_shared<OptionValue>(false);
    std::vector<Menu::Option> upload_results_options = 
    {
        Menu::Option(translate(current_language, Texts::UPLOAD) + " " + translate(current_language, Texts::RESULTS) + " " + "???",
                [](){InputManager::clearAllInputs();},
                upload_results.get(),
                0,
                1,
                true)
    };

    tetrismatrix::menus.push(new Menu("placeholder idk", upload_results_options,
                [player_piece]()
                { 

                }, 
                [player_piece, this, current_language]()
                {

                }));

    tetrismatrix::menus.top()->executeSelectedOption();

    tetrismatrix::menus.top()->on_close = 
        [player_piece, upload_results, this]()
        {
            printf("player piece addr: %p\n\n", player_piece);
            auto self = this; // just in case.......
            auto piece_ptr = player_piece;

            bool upload = false;
            if(std::holds_alternative<bool>(*upload_results))
                upload = std::get<bool>(*upload_results);
            tetrismatrix::popMenu();
            tetrismatrix::popMenu();


            if(upload)
                self->uploadResults(piece_ptr);
            InputManager::clearAllInputs();

        };



    tetrismatrix::menus.push(new Menu(translate(current_language, Texts::RESULTS), options,
                [player_piece]()
                { 
                if(InputManager::keys_pressed[player_piece->controller] & KEY_A)
                InputManager::keys_pressed[player_piece->controller] |= KEY_B;
                }, 
                [player_piece, this, current_language]()
                {
                tetrismatrix::popMenu();
                InputManager::clearAllInputs();
                }));

}

void Match::uploadResults(Piece* player_piece)
{
    auto current_language = (Language)std::get<EnumOption>(tetrismatrix::settings.current_language).value;
    bool connected = NetworkManager::enable();
    if(!connected)
    {
        std::string text = translate(current_language, Texts::CONNECT_TO_NETWORK) + " " + translate(current_language, Texts::FIRST);
        showMessage(tetrismatrix::hub75, text, 2000, tetrismatrix::vertical);
        text = translate(current_language, Texts::GO) + " " + translate(current_language, Texts::TO) + " " + translate(current_language, Texts::OPTIONS);
        showMessage(tetrismatrix::hub75, text, 2000, tetrismatrix::vertical);
        return;
    }
    printf("player piece addr: %p\n\n", player_piece);

    int player_id = DatabaseManager::savePlayer(std::get<std::string>(names[player_piece]));
    int match_id = DatabaseManager::saveMatch(game_mode);
    printf("player_id: %i, match_id: %i", player_id, match_id);
    int score_entry_id = DatabaseManager::saveScoreEntry(match_id, player_id, scores[player_piece], total_lines_cleared[player_piece], won[player_piece], singles_amount[player_piece], doubles_amount[player_piece], triples_amount[player_piece], tetris_amount[player_piece]);
    NetworkManager::disable();
    std::string text;
    if(player_id == -1 || match_id == -1 || score_entry_id == -1)
    {
        text = translate(current_language, Texts::ERROR) + translate(current_language, Texts::UPLOADING) + translate(current_language, Texts::SCORES) + ":C";
    }
    else
    {
        text = translate(current_language, Texts::SCORES) + " " + translate(current_language, Texts::UPLOADED);
        if(current_language == Language::ES)
            text += "S";
        text += " :D";
    }
    showMessage(tetrismatrix::hub75, text, 2000, tetrismatrix::vertical);
}




Match::Match()
{
    initMatch();
}

Match::Match(MatchSettings settings, GameModes game_mode) : settings(settings), game_mode(game_mode)
{
    initMatch();
}

Match::~Match()
{
    endMatch();
}
