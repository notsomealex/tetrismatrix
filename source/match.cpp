#include "match.h"
#include "board.h"
#include "piece.h"
#include "menu.h"
#include "inputmanager.h"
void Match::initMatch()
{
    srand(to_ms_since_boot(get_absolute_time()));
    board = new Board(settings.board_width, settings.board_height, settings.board_pos_x, settings.board_pos_y, settings.game_scale);
    piece = new Piece(board, upcoming_pieces, settings.game_scale);
    setUpcomingPieces();
    tetrismatrix::menus.top()->hide_menu = true;
}

void Match::endMatch()
{
    delete board;
    delete piece;
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
    board->updateBoard();
    
    if(piece->just_solidified)
    {
        updateUpcomingPieces();
        piece->just_solidified = false;
    }
    piece->updatePiece();

    
    if (piece->checkCollision() & COLLISION_DIRECTION::UP)
    {
        board->resetBoard();
        piece->restartPiece();
    }
}

void Match::setUpcomingPieces()
{
    for(int i = 0; i < settings.max_amount_upcoming_pieces; i++)
    {
        upcoming_pieces.push((PIECES)((rand() % 7) + 1));
    }
    piece->next_piece = upcoming_pieces.front();

}

void Match::updateUpcomingPieces()
{
    PIECES next_piece = upcoming_pieces.front();
    upcoming_pieces.pop();
    upcoming_pieces.push((PIECES)((rand() % 7) + 1));
    piece->next_piece = next_piece;
}



Match::Match()
{
    initMatch();
}

Match::Match(MatchSettings settings) : settings(settings)
{
    initMatch();
}

Match::~Match()
{
    endMatch();
}