#include "board.h"
#include "helpers.h"

void Board::initBoard()
{
    board_matrix = std::vector<std::vector<int>>(board_height, std::vector<int>(board_width));

    for (int i = 0; i < board_height; i++)
    {
        for (int j = 0; j < board_width; j++)
        {
            board_matrix[i][j] = 0; //(i % 6) + 1; <- RAINBOW
        }
    }
if(tetrismatrix::vertical) 
{
    board_pos_x = ((SCREEN_HEIGHT / 2) - (board_width * scale / 2)) / scale;
    board_pos_y = (SCREEN_WIDTH - (board_height * scale)) / scale;
} 
else 
{
    board_pos_x = (SCREEN_WIDTH / 2) - (board_width * scale / 2);
    board_pos_y = 0;
}
    

}

void Board::printBoard(bool print_empty_blocks)
{

    //printf("board_pos_x: %d, board_pos_y: %d\n", board_pos_x, board_pos_y);
    //printf("board_width: %d, board_height: %d, scale: %d\n", board_width, board_height, scale);
    for (int i = 0; i < board_height; i++)
    {
        for (int j = 0; j < board_width; j++)
        {
            const PIECES current_block = (PIECES)board_matrix[i][j];
            if(!print_empty_blocks && current_block == NONE)
                continue;

            int x = vertical ? ((i + board_pos_y) * scale) : ((j + board_pos_x) * scale);
            // i dont like that i have to do this hack so that it doesnt show mirrored but mehhhhhhh
            int y = vertical ? ((board_width - 1 - j + board_pos_x) * scale) : ((i + + board_pos_y) * scale);
            
            drawSquare(hub75, scale, x, y, piece_colors.at(current_block).r, piece_colors.at(current_block).g, piece_colors.at(current_block).b);
            //hub75.set_pixel(x, y, piece_colors.at(current_block).r, piece_colors.at(current_block).g, piece_colors.at(current_block).b);
        }
    }

    // draw border
    for(int i = -1; i <= board_height; i++) {
    // left border
    int x = vertical ? ((i + board_pos_y) * scale) : (((-1) + board_pos_x) * scale);
    int y = vertical ? ((board_width + board_pos_x) * scale) : ((i + board_pos_y) * scale);
    drawSquare(hub75, scale, x, y, 255, 255, 255);

    // right border
    x = vertical ? ((i + board_pos_y) * scale) : ((board_width + board_pos_x) * scale);
    y = vertical ? (((-1) + board_pos_x) * scale) : ((i + board_pos_y) * scale);
    drawSquare(hub75, scale, x, y, 255, 255, 255);
}

}

void Board::updateBoard()
{
    printBoard(true);
}

int Board::leftTileLimit()
{
    return 0;
}

int Board::rightTileLimit()
{
    return board_width;
}

int Board::bottomTileLimit()
{
    return board_height;
}

int Board::topTileLimit()
{
    return 0;
}

int Board::boardCenter()
{
    return (board_width / 2);
}

void Board::removeLines()
{
    std::vector<std::vector<int>> new_matrix;
    std::vector<int> empty_row(board_width, 0);
    for (int i = 0; i < board_height; i++)
    {
        bool current_row_full = true;
        for (int j = 0; j < board_width; j++)
        {
            if (board_matrix[i][j] == 0)
            {
                current_row_full = false;
                break;
            }
        }
        if (!current_row_full)
            new_matrix.push_back(board_matrix[i]);
    }

    int lines_cleared = board_height - new_matrix.size();
    bool matrix_changed = lines_cleared > 0;

    if (matrix_changed)
    {
        for (int i = 0; i < lines_cleared; i++)
            new_matrix.insert(new_matrix.begin(), empty_row);

        board_matrix = new_matrix;
    }
    this->cleared_lines = lines_cleared;
}

void Board::addGarbageLines(int amount)
{
    if(amount <= 0) return;
    int empty_column = rand() % board_width;

    std::vector<int> garbage_row(board_width, PIECES::GARBAGE);
    for(int i = 0; i < board_height - amount; i++)
    {
        board_matrix[i] = board_matrix[i + amount];
    }

    for(int i = 0; i < amount; i++)
    {
        board_matrix[board_height - i - 1] = garbage_row;
        board_matrix[board_height - i - 1][empty_column] = PIECES::NONE;
    }


}


void Board::resetBoard()
{
    for (int i = 0; i < board_height; i++)
    {
        for (int j = 0; j < board_width; j++)
        {
            board_matrix[i][j] = 0;
        }
    }
}


Board::Board()
{
    initBoard();
}

Board::Board(int width, int height, int scale) : board_width(width), board_height(height), scale(scale)
{
    initBoard();
}