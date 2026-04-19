#pragma once

#include "piece.h"


class Match
{
public:
    Board *board = nullptr;
    Piece *piece = nullptr;


    std::queue<PIECES> upcoming_pieces;


    // todo: add here all the match settings that come up.
    MatchSettings settings;

    void initMatch();
    void endMatch();
    void updateMatch();
    void setUpcomingPieces();
    void updateUpcomingPieces();
    Match();
    Match(MatchSettings settings);
    ~Match();
};