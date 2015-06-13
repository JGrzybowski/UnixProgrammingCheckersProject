#ifndef UNIX_PROJECT_CLIENT_GAME_RULES
#define UNIX_PROJECT_CLIENT_GAME_RULES

#include "common.h"

#define POSSIBLE_MOVE 0
#define MOVE_ON_WHITE_FIELD -1
#define MOVE_OUTSIDE_BOARD -2
#define IMPOSSIBLE_MOVE -3 

int is_possible_move(position_t from, position_t to);

#endif

