#pragma once

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
    GHOST_O,
    GHOST_I,
    GHOST_L,
    GHOST_J,
    GHOST_S,
    GHOST_Z,
    GHOST_T
};

// u8 bitmask
enum COLLISION_DIRECTION
{
    LEFT = (1 << 0),  // 00000001
    RIGHT = (1 << 1), // 00000010
    UP = (1 << 2),    // 00000100
    DOWN = (1 << 3)   // 00001000
};