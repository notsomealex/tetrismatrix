#pragma once

#include "piece.h"


class Match
{
public:
    Board *board = nullptr;
    Piece *piece = nullptr;


    std::queue<PIECES> upcoming_pieces;



    // todo: add here all the match settings that come up.
    struct MatchSettings
    {
        int max_amount_upcoming_pieces = 5; // 5 by default.
        int game_scale = 2; // size of the squares
        int board_width = 32 / game_scale;
        int board_height = 64 / game_scale;
        int board_pos_x = 0;
        int board_pos_y = 0;
    } settings;

    void initMatch();
    void endMatch();
    void updateMatch();
    void setUpcomingPieces();
    void updateUpcomingPieces();
    Match(); // this constructor could maybe take as parameters the settings of the match (game mode, for example).
    ~Match();
};