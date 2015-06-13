#include "clientGameRules.h"

int is_possible_move(position_t from, position_t to){
	position_t from_row = from / BOARD_SIZE;
	position_t from_col = from % BOARD_SIZE;
	position_t to_row = to / BOARD_SIZE;
	position_t to_col = to % BOARD_SIZE;
	
	if( ((from_col+from_row)%2==1) || ((to_col+to_row)%2==1) )
		return MOVE_ON_WHITE_FIELD;
	if(from_row >= BOARD_SIZE || from_col >= BOARD_SIZE || to_row >= BOARD_SIZE || to_col >= BOARD_SIZE)
		return MOVE_OUTSIDE_BOARD;
	if( (from_row+1 == to_row || from_row-1 == to_row) && (from_col+1 == to_col || from_col-1 == to_col) )
		return POSSIBLE_MOVE;
	else
		return IMPOSSIBLE_MOVE;
}
