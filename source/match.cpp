#include "match.h"
#include "board.h"
#include "piece.h"
#include "menu.h"
#include "helpers.h"
#include "language.h"
#include "inputmanager.h"
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
    if(paused)
        return;

    int16_t& keys_pressed = InputManager::keys_pressed[InputManager::controllers[0]];


    if(keys_pressed & KEY_START)
    {
        menus.push(createMenu(Menus::PAUSE));
        paused = true;
    }
        
    printf("updating match!");
    
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
            scores[boards[i]] += boards[i]->cleared_lines * 10 * std::get<int>(settings.velocity);
            total_lines_cleared[boards[i]] += boards[i]->cleared_lines;
            current_velocity_lines_cleared[boards[i]] += boards[i]->cleared_lines;
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
            showUpcomingPieces(pieces[0], pieces[0]->board->board_pos_x, pieces[0]->board->board_pos_y - 10, false, true);
            int& velocity = std::get<int>(settings.velocity);
            int& max_velocity = std::get<int>(settings.max_velocity);
            printf("lines: %i", current_velocity_lines_cleared[boards[0]]);
            printf("velocity: %i", velocity);
            if(current_velocity_lines_cleared[boards[0]] > 20)
            {
                if(velocity < max_velocity)
                {
                    velocity++;
                    pieces[0]->falling_frequency = 30 / velocity;
                    current_velocity_lines_cleared[boards[0]] = 0;
                }
                else
                {
                    gameWin();
                }
            }
            break;
        }
        case GameModes::VERSUS:
            while(InputManager::controllers.size() != 2)
                InputManager::searchForControllers();
            showUpcomingPieces(pieces[0], pieces[0]->board->board_pos_x - 5, pieces[0]->board->board_pos_y, true, false);
            showUpcomingPieces(pieces[1], pieces[1]->board->board_pos_x + (pieces[1]->board->board_width) + 1, pieces[1]->board->board_pos_y, true, false);
            break;
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

void Match::showUpcomingPieces(Piece* piece, int pos_x, int pos_y, bool vertical, bool show_text)
{
    
if(vertical)
    clearRegionOfScreen(tetrismatrix::hub75, 
        pos_x * piece->scale, 
        pos_y * piece->scale, 
        (pos_x + 4) * piece->scale, 
        (pos_y + 4 * (piece->upcoming_pieces.size() + 1)) * piece->scale, 
        tetrismatrix::vertical);
else
    clearRegionOfScreen(tetrismatrix::hub75, 
        pos_x * piece->scale, 
        pos_y * piece->scale, 
        (pos_x + 4 * (piece->upcoming_pieces.size() + 1)) * piece->scale, 
        (pos_y + 4 + 2) * piece->scale, 
        tetrismatrix::vertical);

    if(show_text)
    {
        Language current_language = (Language)std::get<EnumOption>(tetrismatrix::settings.current_language).value;
        std::string drawn_next_text = translate(current_language, Texts::NEXT);
        cropTextToFit(drawn_next_text, 3, 15);
        draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, drawn_next_text, pos_x - 1, pos_y - 1, 255, 255, 255, tetrismatrix::vertical);
    }

    const int spacing = 2 * piece->scale;
    piece->printPiece(shapes[piece->next_piece], pos_x - piece->board->board_pos_x, pos_y - piece->board->board_pos_y, piece->scale);
    for(int i = 0; i < piece->upcoming_pieces.size(); i++)
    {
        int piece_pos_x = vertical ? pos_x : pos_x + (spacing * (i + 1));
        int piece_pos_y = vertical ? pos_y + (spacing * (i + 1)) : pos_y;
        piece->printPiece(shapes[piece->upcoming_pieces[i]], piece_pos_x - piece->board->board_pos_x, piece_pos_y - piece->board->board_pos_y, piece->scale);
    }
    

    
}

void Match::gameOver()
{

}

void Match::gameWin()
{

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