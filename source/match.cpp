#include "match.h"
#include "board.h"
#include "piece.h"
#include "menu.h"
#include "inputmanager.h"
void Match::initMatch()
{
    srand(to_ms_since_boot(get_absolute_time()));

    // main boards and pieces.
    boards.push_back(new Board(std::get<int>(settings.board_width), std::get<int>(settings.board_height), std::get<int>(settings.game_scale)));
    pieces.push_back(new Piece(boards[0], upcoming_pieces, std::get<int>(settings.game_scale)));
    setUpcomingPieces();

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
    {
        return;
    }
    int16_t& keys_pressed = InputManager::keys_pressed[InputManager::controllers[0]];


    if(keys_pressed & KEY_START)
    {
        menus.push(createMenu(Menus::PAUSE));
        paused = true;
    }
        
    printf("updating match!");
    boards[0]->updateBoard();
    
    if(pieces[0]->just_solidified)
    {
        updateUpcomingPieces();
        pieces[0]->just_solidified = false;
    }
    pieces[0]->updatePiece();

    

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
            if(pieces[0]->just_solidified)
                score += boards[0]->cleared_lines * 10;
            break;
        case GameModes::VERSUS:
            while(InputManager::controllers.size() != 2)
                InputManager::searchForControllers();
            break;
        case GameModes::COOP:
            while(InputManager::controllers.size() != 2)
                InputManager::searchForControllers();
            break;
    }
}

void Match::setUpcomingPieces()
{
    for(int i = 0; i < std::get<int>(settings.max_amount_upcoming_pieces); i++)
    {
        upcoming_pieces.push((PIECES)((rand() % 7) + 1));
    }
    pieces[0]->next_piece = upcoming_pieces.front();

}

void Match::updateUpcomingPieces()
{
    PIECES next_piece = upcoming_pieces.front();
    upcoming_pieces.pop();
    upcoming_pieces.push((PIECES)((rand() % 7) + 1));
    pieces[0]->next_piece = next_piece;
}

void Match::gameOver()
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