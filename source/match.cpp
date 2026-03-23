#include "match.h"
#include "board.h"
#include "piece.h"
void Match::initMatch()
{
    srand(time(0));
    board = new Board();
    piece = new Piece(board, upcoming_pieces);
    setUpcomingPieces();
    
}

void Match::endMatch()
{
    delete board;
    delete piece;
}

void Match::updateMatch()
{
    board->updateBoard();
    board->printBoard();
    
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

Match::~Match()
{
    endMatch();
}