#ifndef UNIX_PROJECT_BOARD
#define UNIX_PROJECT_BOARD

#include "common.h"
#include "fileFunctions.h"

#define P1_PIECE 'X'
#define P2_PIECE 'O'
#define BLACK_PIECE 'X'
#define WHITE_PIECE 'O'
#define EMPTY_SPACE ' '
#define WHITE_SPACE ' '

#define TABLE_CROSSING '+'
#define TABLE_HORIZONTAL '-'
#define TABLE_VERTICAL '|'

#define TABLE_MARGIN 1

int board_position(int column_index, int row_index);
void print_board(const char board[BOARD_SIZE * BOARD_SIZE]);
void print_table_line();

position_t get_board_position(int row, char column);
char get_row_char(position_t position);
char get_column_char(position_t position);

#endif

