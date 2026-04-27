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


struct MatchSettings
{
    // DEFAULT
    OptionValue max_amount_upcoming_pieces = 5;
    OptionValue game_scale = 2; // size of the squares
    OptionValue board_width = 32 / std::get<int>(game_scale);
    OptionValue board_height = (64 / std::get<int>(game_scale));
    OptionValue velocity = 1;
    OptionValue lock_delay = 60; // frames for a piece to solidify.

    // MARATHON
    OptionValue max_velocity = 10;

    // VERSUS
    OptionValue garbage_multiplier = 1;
    OptionValue garbage_blocking = 1;

};

class Match
{
public:
    std::vector<Board*> boards;
    std::vector<Piece*> pieces;


    std::queue<PIECES> upcoming_pieces;


    MatchSettings settings;
    GameModes game_mode = GameModes::ZEN;

    int score = 0;
    int lines_cleared = 0;
    std::string date;
    
    bool paused = false;

    void initMatch();
    void endMatch();
    void updateMatch();
    void setUpcomingPieces();
    void updateUpcomingPieces();
    void gameOver();
    Match();
    Match(MatchSettings settings, GameModes game_mode);
    ~Match();
};