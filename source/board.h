#pragma once
#include "libs.h"

#define BOARD_DEFAULT_WIDTH 10
#define BOARD_DEFAULT_HEIGHT 20

class Board
{
private:
public:
    std::vector<std::vector<int>> board_matrix;

    int board_width = BOARD_DEFAULT_WIDTH;
    int board_height = BOARD_DEFAULT_HEIGHT;
    int board_pos_x = 10;
    int board_pos_y = 0;
    int cleared_lines = 0;

    void initBoard();
    void printBoard();
    void updateBoard();
    int leftTileLimit();
    int rightTileLimit();
    int bottomTileLimit();
    int topTileLimit();
    int boardCenter();
    void removeLines();
    void resetBoard();

    Board();
};