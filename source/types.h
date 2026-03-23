#pragma once
#ifndef TYPES_H
#define TYPES_H

#include "libs.h"

inline uint32_t rgb_to_int32(int r, int g, int b)
{
    return (r << 16) | (g << 8) | (b);
}

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
    GHOST
};

struct Color
{
    uint8_t r, g , b;    
};

inline std::unordered_map<PIECES, Color> piece_colors =
{
    {NONE, {0, 0, 0}},
    {O, {255, 255, 0}},
    {I, {0, 255, 255}},
    {L, {255, 165, 0}},
    {J, {0, 0, 255}},
    {S, {57, 255, 20}},
    {Z, {238, 75, 43}},
    {T, {191, 0, 255}},
    {GHOST, {211, 211, 211}}
};

// u8 bitmask
enum COLLISION_DIRECTION
{
    LEFT = (1 << 0),  // 00000001
    RIGHT = (1 << 1), // 00000010
    UP = (1 << 2),    // 00000100
    DOWN = (1 << 3)   // 00001000
};

#endif
