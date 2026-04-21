#include "match.h"
#include "board.h"
#include "piece.h"
#include "menu.h"
#include "inputmanager.h"
void Match::initMatch()
{
    srand(to_ms_since_boot(get_absolute_time()));

    // main boards and pieces.
    boards.push_back(new Board(settings.board_width, settings.board_height, settings.board_pos_x, settings.board_pos_y, settings.game_scale));
    pieces.push_back( new Piece(boards[0], upcoming_pieces, settings.game_scale));
    setUpcomingPieces();
    tetrismatrix::menus.top()->hide_menu = true;
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
    if(menus.top()->menu_type == Menus::PAUSE)
        return;
    int16_t& keys_pressed = InputManager::keys_pressed[InputManager::controllers[0]];


    if(keys_pressed & KEY_START)
        menus.push(createMenu(Menus::PAUSE));
        
    printf("updating match!");
    boards[0]->updateBoard();
    
    if(pieces[0]->just_solidified)
    {
        updateUpcomingPieces();
        pieces[0]->just_solidified = false;
    }
    pieces[0]->updatePiece();

    
    if (pieces[0]->checkCollision() & COLLISION_DIRECTION::UP)
    {
        boards[0]->resetBoard();
        pieces[0]->restartPiece();
    }
}

void Match::setUpcomingPieces()
{
    for(int i = 0; i < settings.max_amount_upcoming_pieces; i++)
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