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
    } settings;

    void initMatch();
    void endMatch();
    void updateMatch();
    void setUpcomingPieces();
    void updateUpcomingPieces();
    Match(); // this constructor could maybe take as parameters the settings of the match (game mode, for example).
    ~Match();
};