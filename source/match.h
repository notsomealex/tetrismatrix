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

inline std::unordered_map<GameModes, MatchSettings> default_settings
{
    {
        GameModes::ZEN, 
        {
            .max_amount_upcoming_pieces = 5,
            .game_scale = 2,
            .board_width = 32 / 2,
            .board_height = (64 / 2),
            .velocity = 1,
            .lock_delay = 60, 
        }
    },
    {
        GameModes::MARATHON,
        {
            .max_amount_upcoming_pieces = 5,
            .game_scale = 2,
            .board_width = 10,
            .board_height = 20,
            .velocity = 1,
            .lock_delay = 60, 

            .max_velocity = 15,
        }
    },
    {
        GameModes::VERSUS,
        {
            .max_amount_upcoming_pieces = 5,
            .game_scale = 2,
            .board_width = 10,
            .board_height = 16,
            .velocity = 1,
            .lock_delay = 60, 

            .max_velocity = 15,

            .garbage_multiplier = 1,
            .garbage_blocking = 1
        }
    },
    {
        GameModes::COOP,
        {
            .max_amount_upcoming_pieces = 5,
            .game_scale = 2,
            .board_width = 30,
            .board_height = 20,
            .velocity = 1,
            .lock_delay = 60, 

            .max_velocity = 15,
        }
    },
    {
        GameModes::DUAL_BOARD,
        {
            .max_amount_upcoming_pieces = 5,
            .game_scale = 2,
            .board_width = 10,
            .board_height = 20,
            .velocity = 1,
            .lock_delay = 60, 

            .max_velocity = 15,
        }
    }

};

class Match
{
public:
    std::vector<Board*> boards;
    std::vector<Piece*> pieces;


    MatchSettings settings;
    GameModes game_mode = GameModes::ZEN;

    std::unordered_map<Piece*, int> scores;
    std::unordered_map<Board*, int> total_lines_cleared;
    std::unordered_map<Board*, int> current_velocity_lines_cleared;

    std::unordered_map<Board*, int> singles_amount;
    std::unordered_map<Board*, int> doubles_amount;
    std::unordered_map<Board*, int> triples_amount;
    std::unordered_map<Board*, int> tetris_amount;

    OptionValue name = ""; // PLACEHOLDER later actually save this name to the db.

    
    std::string date;
    
    bool paused = false;
    bool ended = false;

    void initMatch();
    void endMatch();
    void updateMatch();
    void setUpcomingPieces(Piece* piece);
    void updateUpcomingPieces(Piece* piece);
    void showUpcomingPieces(Piece* piece, int pos_x, int pos_y, bool vertical_layout, bool show_text);
    void showScore(Piece* piece, int pos_x, int pos_y, bool vertical_layout, bool show_text);
    void showHoldPiece(Piece* piece, int pos_x, int pos_y, bool vertical_layout, bool show_text);
    void gameOver();
    void gameWin();
    void versusOver(Piece* winner);
    void showResults();
    Match();
    Match(MatchSettings settings, GameModes game_mode);
    ~Match();
};