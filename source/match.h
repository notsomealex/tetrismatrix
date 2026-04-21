#pragma once

#include "piece.h"


enum class GameModes
{
    ZEN,
    MARATHON,
    VERSUS,
    COOP,
    DUAL_BOARD
};

class Match
{
public:
    std::vector<Board*> boards;
    std::vector<Piece*> pieces;


    std::queue<PIECES> upcoming_pieces;


    // todo: add here all the match settings that come up.
    MatchSettings settings;
    GameModes game_mode = GameModes::ZEN;

    void initMatch();
    void endMatch();
    void updateMatch();
    void setUpcomingPieces();
    void updateUpcomingPieces();
    Match();
    Match(MatchSettings settings, GameModes game_mode);
    ~Match();
};