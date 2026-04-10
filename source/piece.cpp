#include "piece.h"
#include "input.h"
#include "helpers.h"
void Piece::initPiece()
{
    shape = (PIECES)((rand() % 7) + 1);
    current_piece_matrix = shapes[shape];

    current_piece_matrix = shapes[shape];

    pos_x = board->boardCenter();
    pos_y = 0;
}

void Piece::updatePiece()
{
    printPiece();
    movePiece();
}

void Piece::movePiece()
{
    int temp_pos_x = pos_x;
    int temp_pos_y = pos_y;

    printf("moving piece!!!\n\n");
    printf("piece pos_x: %i, piece pos_y: %i", this->pos_x, this->pos_y);


    if(keys_down & KEY_L || keys_down & KEY_R)
        swapPiece();

    if (keys_down & KEY_UP)
    {
        hardDrop();
    }

    if (checkCollision() & COLLISION_DIRECTION::DOWN)
    {
        hit_ground = true;
        frames_colliding_down++;
    }
    else
    {
        frames_colliding_down = 0;
    }

    if (hit_ground)
        frames_since_hit_ground++;
    else
        frames_since_hit_ground = 0;

    if (frames % falling_frequency == 0 && frames > 0 && !(checkCollision() & COLLISION_DIRECTION::DOWN))
    {
        temp_pos_y++;
        if (temp_pos_y > board->topTileLimit() && temp_pos_y < board->bottomTileLimit())
        {
            pos_y = temp_pos_y;
        }
    }

    if (frames_moving == 1 || frames_moving > 5)
    {
        if (keys_down & KEY_LEFT && !(checkCollision() & COLLISION_DIRECTION::LEFT) && (frames_moving % 2 == 0 || frames_moving == 1))
        {
            temp_pos_x--;
            if (temp_pos_x >= board->leftTileLimit() && temp_pos_x < board->rightTileLimit())
            {
                pos_x = temp_pos_x;
            }
        }

        if (keys_down & KEY_RIGHT && !(checkCollision() & COLLISION_DIRECTION::RIGHT) && (frames_moving % 2 == 0 || frames_moving == 1))
        {
            temp_pos_x++;
            if (temp_pos_x >= board->leftTileLimit() && temp_pos_x < board->rightTileLimit())
            {
                pos_x = temp_pos_x;
            }
        }

        if (keys_down & KEY_DOWN && !(checkCollision() & COLLISION_DIRECTION::DOWN) && (frames_moving % 2 == 0 || frames_moving == 1) && (frames % falling_frequency != 0))
        {
            temp_pos_y++;
            if (temp_pos_y > board->topTileLimit() && temp_pos_y < board->bottomTileLimit())
            {
                pos_y = temp_pos_y;
            }
        }
    }

    if (keys_down & (KEY_LEFT | KEY_RIGHT | KEY_DOWN))
        frames_moving++;
    else
        frames_moving = 0;

    if ((checkCollision() & COLLISION_DIRECTION::DOWN && (frames_colliding_down >= 60 || frames_since_hit_ground >= 240)))
    {
        just_solidified = true;
        solidifyPiece();
        restartPiece();
        frames_colliding_down = 0;
    }
    else
        just_solidified = false;

    if (keys_down & KEY_B)
        rotateCW();

    if (keys_down & KEY_A)
    {
        rotateCCW();
        frames_moving++;
    }
}

void Piece::hardDrop()
{
  
        while (!(checkCollision() & COLLISION_DIRECTION::DOWN))
        {
            pos_y++;
        }
        frames_colliding_down = 60;
}
void Piece::printPiece()
{
    printGhostPiece();
    printPiece(current_piece_matrix, this->pos_x, this->pos_y);
}

void Piece::printPiece(std::vector<std::vector<int>> piece_matrix, int pos_x, int pos_y)
{
    for (int i = 0; i < (int)piece_matrix.size(); i++)
    {
        for (int j = 0; j < (int)piece_matrix[0].size(); j++)
        {
            const PIECES current_block = (PIECES)piece_matrix[i][j];
            if(current_block == NONE) continue;

            int x = vertical ? (pos_y + board->board_pos_y + i) * game_scale : (pos_x + board->board_pos_x + j) * game_scale;
            int y = vertical ? (pos_x + board->board_pos_x + j) * game_scale : (pos_y + board->board_pos_y + i) * game_scale;
            drawSquare(hub75, game_scale, x, y, piece_colors.at(current_block).r, piece_colors.at(current_block).g, piece_colors.at(current_block).b);
            //hub75.set_pixel(x, y, piece_colors.at(current_block).r, piece_colors.at(current_block).g, piece_colors.at(current_block).b);
        }
    }
}

void Piece::printGhostPiece()
{
    int ghost_piece_pos_y;
    std::vector<std::vector<int>> ghost_piece_matrix = current_piece_matrix;

    // TODO: not the best way to do it. it recalculates the position
    // of the shadow piece every frame, not ideal at all.
    // works fine tho.
    int actual_pos_y = pos_y;
    while (!(checkCollision() & COLLISION_DIRECTION::DOWN))
    {
        pos_y++;
    }
    ghost_piece_pos_y = pos_y;
    pos_y = actual_pos_y;

    if(ghost_piece_pos_y == pos_y) return;
    
    for(int i = 0; i < (int)ghost_piece_matrix.size(); i++)
    {
        for(int j = 0; j < (int)ghost_piece_matrix[0].size(); j++)
        {
            if(ghost_piece_matrix[i][j] != PIECES::NONE)
            {
                // TODO: I SHOULDNT need to use this workaround, i should only need to use
                // the 'pieces' variable, but here for some reason its value
                // is the upcoming piece.
                PIECES current_shape = (PIECES)ghost_piece_matrix[i][j];
                // sets it to the ghost counterpart.
                ghost_piece_matrix[i][j] = GHOST;
            }
        }
    }
    printPiece(ghost_piece_matrix, this->pos_x, ghost_piece_pos_y);

}

/*
   OKAY. so to make this right, i'd have to implement the system that
   is actually used, where it would check all of the possible rotation
   situations, and check if it matches.
   but that would require a lot of effort so, for now,
   i'm sticking with what's below
   can get glitchy, but works for the most part.
*/
void Piece::rotateCCW()
{

    if (shape == O || (checkCollision() & RIGHT && checkCollision() & LEFT && pos_x >= board->rightTileLimit() - 2) || (checkCollision() & UP && checkCollision() & DOWN && checkCollision() & RIGHT && checkCollision() & LEFT))
        return;

    std::vector<std::vector<int>> rotated_piece;

    std::vector<std::vector<int>> unrotated_piece = current_piece_matrix;

    for (int j = int(current_piece_matrix[0].size()) - 1; j >= 0; j--)
    {
        std::vector<int> row;
        for (int i = 0; i < int(current_piece_matrix.size()); i++)
        {
            row.push_back(current_piece_matrix[i][j]);
        }
        rotated_piece.push_back(row);
    }
    current_piece_matrix = rotated_piece;

    // i hate everything that is below, but it must stick for now.
    while (checkCollision() & COLLISION_DIRECTION::RIGHT && !(checkCollision() & COLLISION_DIRECTION::LEFT))
    {
        pos_x--;
    }
    pos_x++;
    if (!(checkCollision() & COLLISION_DIRECTION::RIGHT))
    {
        pos_x--;
    }

    while (checkCollision() & COLLISION_DIRECTION::LEFT && !(checkCollision() & COLLISION_DIRECTION::RIGHT) && pos_x < board->rightTileLimit() - 1)
    {
        pos_x++;
    }
    while (checkCollision() & COLLISION_DIRECTION::RIGHT && !(checkCollision() & COLLISION_DIRECTION::LEFT) && pos_x > board->leftTileLimit())
    {
        pos_x--;
    }

    while (checkCollision() & COLLISION_DIRECTION::DOWN && !(checkCollision() & COLLISION_DIRECTION::UP) && pos_y > board->topTileLimit())
    {
        pos_y--;
    }
}

void Piece::rotateCW()
{
    // lol. lazy af but works.
    for (int i = 0; i < 3; i++)
        rotateCCW();
}

uint8_t Piece::checkCollision()
{
    uint8_t collision_direction = 0;

    for (int i = 0; i < (int)current_piece_matrix.size(); i++)
    {
        for (int j = 0; j < (int)current_piece_matrix[0].size(); j++)
        {
            int pos_in_board_y = i + pos_y;
            int pos_in_board_x = j + pos_x;
            if (pos_in_board_y < board->board_height && pos_in_board_x < board->board_width)
            {
                if (pos_in_board_y + 1 < board->board_height)
                {
                    if (board->board_matrix[pos_in_board_y + 1][pos_in_board_x] != 0 && current_piece_matrix[i][j] != 0)
                        collision_direction |= COLLISION_DIRECTION::DOWN;
                }

                if (pos_in_board_y - 1 >= 0 && pos_in_board_y == 1) // don't really like this last condition but works for now.
                {
                    if (board->board_matrix[pos_in_board_y - 1][pos_in_board_x] != 0 && current_piece_matrix[i][j] != 0)
                        collision_direction |= COLLISION_DIRECTION::UP;
                }

                if (pos_in_board_x - 1 >= 0)
                {
                    if (board->board_matrix[pos_in_board_y][pos_in_board_x - 1] != 0 && current_piece_matrix[i][j] != 0)
                        collision_direction |= COLLISION_DIRECTION::LEFT;
                }
                if (pos_in_board_x + 1 < board->board_width)
                {
                    if (board->board_matrix[pos_in_board_y][pos_in_board_x + 1] != 0 && current_piece_matrix[i][j] != 0)
                        collision_direction |= COLLISION_DIRECTION::RIGHT;
                }
            }
        }
    }

    // --  board limits collision detections  -- //
    if (pos_x + (int)current_piece_matrix[0].size() >= board->rightTileLimit())
        collision_direction |= COLLISION_DIRECTION::RIGHT;

    if (pos_x <= board->leftTileLimit())
        collision_direction |= COLLISION_DIRECTION::LEFT;

    // useless: pos_y can be less or  equal than that

    // if (pos_y <= board->topTileLimit() - 1)
    // collision_direction |= COLLISION_DIRECTION::UP;

    if (pos_y + (int)current_piece_matrix.size() >= board->bottomTileLimit())
        collision_direction |= COLLISION_DIRECTION::DOWN;

    return collision_direction;
}

void Piece::solidifyPiece()
{
    for (int i = 0; i < (int)current_piece_matrix.size(); i++)
    {
        for (int j = 0; j < (int)current_piece_matrix[0].size(); j++)
        {
            if (current_piece_matrix[i][j] == 0)
                continue;

            if (i + pos_y < board->board_height && i + pos_y >= 0 && j + pos_x < board->board_width && j + pos_x >= 0)
                board->board_matrix[i + pos_y][j + pos_x] = current_piece_matrix[i][j];
        }
    }
    board->removeLines();


    already_swapped = false;
    shape = next_piece;
}

// THIS THING DOES NOT UPDATE THE PIECE'S SHAPE.
// so, also do shape = next_piece when needed. lol.
void Piece::restartPiece()
{
    pos_y = 0;
    hit_ground = false;
    pos_x = board->boardCenter();

    current_piece_matrix = shapes[shape];
}

void Piece::swapPiece()
{
    if(already_swapped) return;

    PIECES old_piece = shape;

    if (hold_piece == NONE)
    {
        hold_piece = old_piece;
        shape = upcoming_pieces.front();
        upcoming_pieces.pop();
    }
    else
    {
        shape = hold_piece;
        hold_piece = old_piece;
    }

    restartPiece();
    already_swapped = true;

}


// idk why i dont use more this syntax for constructors.
Piece::Piece(Board *board, std::queue<PIECES> &upcoming_pieces)
: board(board), upcoming_pieces(upcoming_pieces)
{
    initPiece();
}
