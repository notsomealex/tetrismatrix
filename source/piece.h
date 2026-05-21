#pragma once
#ifndef PIECE_H
#define PIECE_H

#include "board.h"
#include "types.h"


const std::vector<std::vector<int>> shapes[] =
    {
        // NONE //
        {{0}},
        // 'O' //
        {
            {O, O},
            {O, O}},
        // 'I' //
        {
            {I},
            {I},
            {I},
            {I}},
        // 'L' //
        {
            {L, 0},
            {L, 0},
            {L, L}},
        // 'J' //
        {{0, J},
         {0, J},
         {J, J}},
        // 'S' //
        {
            {S, 0},
            {S, S},
            {0, S},
        },
        // 'Z' //
        {
            {0, Z},
            {Z, Z},
            {Z, 0},
        },
        // 'T' //
        {
            {0, T, 0},
            {T, T, T}
        }
    };


class Piece
{
public:
    int pos_x = 0;
    int pos_y = 0;
    int frames_moving = 0;
    int falling_frequency = 60;
    int frames_colliding_down = 0;
    int frames_since_hit_ground = 0;
    int lock_delay = 60;
    int start_pos_x = 0;

    uni_controller_t* controller;

    std::function<uint16_t()> extra_collision = nullptr;




    bool hit_ground = false;
    bool just_solidified = false;
    bool already_swapped = false;

    std::deque<PIECES>upcoming_pieces;


    Board *board = nullptr;

    PIECES shape = I;
    PIECES next_piece;
    PIECES hold_piece = NONE;
    std::vector<std::vector<int>> current_piece_matrix;
    int scale = 1;
    void printPiece();
    void printPiece(std::vector<std::vector<int>> piece_matrix, int pos_x, int pos_y, int scale);
    uint8_t checkCollision();
    void updatePiece();
    void movePiece();
    void hardDrop();
    void rotateCW();
    void rotateCCW();
    void initPiece();
    void solidifyPiece();
    void restartPiece();
    void printGhostPiece();
    void swapPiece();

    Piece(Board *board, int scale, int falling_frequency, int lock_delay, uni_controller_t* controller);
    Piece(int start_pos_x, Board *board, int scale, int falling_frequency, int lock_delay, uni_controller_t* controller);

};

#endif