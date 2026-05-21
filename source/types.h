#pragma once
#ifndef TYPES_H
#define TYPES_H

#include "libs.h"

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32



enum class Menus
{
    MAIN_MENU,
    OPTIONS,
    SCORES,
    PAUSE,
    GAME_MODE_SELECTION,
    MATCH_SETTINGS,
    GAME_OVER,
    RESULTS,
    CONNECT_TO_NETWORK
};


enum class GameModes
{
    ZEN,
    MARATHON,
    VERSUS,
    COOP,
    DUAL_BOARD
};

enum PIECES
{
    NONE,
    O,
    I,
    L,
    J,
    S,
    Z,
    T,
    GHOST,
    GARBAGE
};

struct Color
{
    uint8_t r, g , b;    
};

inline std::unordered_map<PIECES, Color> piece_colors =
{
    // RBG
    {NONE, {0, 0, 0}},
    {O, {255, 0, 255}},
    {I, {0, 255, 255}},
    {L, {255, 0, 165}},
    {J, {0, 255, 52}},
    {S, {57, 20, 255}},
    {Z, {238, 43, 75}},
    {T, {191, 255, 0}},
    {GHOST, {50, 50, 50}},
    {GARBAGE, {100, 100, 100}}
};

// u8 bitmask
enum COLLISION_DIRECTION
{
    LEFT = (1 << 0),  // 00000001
    RIGHT = (1 << 1), // 00000010
    UP = (1 << 2),    // 00000100
    DOWN = (1 << 3)   // 00001000
};

struct EnumOption 
{
    int value;
    std::vector<std::string> labels;
};
using OptionValue = std::variant<int, bool, float, std::string, EnumOption>;


#endif
