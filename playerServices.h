#include "common.h"
#include "fileFunctions.h"

#ifndef UNIX_PROJECT_PLAYER_HELPERS
#define UNIX_PROJECT_PLAYER_HELPERS

int is_valid_nickname(char nickname[NICK_SIZE]);
int player_is_in_game(const char nickname[NICK_SIZE], gameID game_id);
void add_game_to_list(gameID game_id, const char nickname[NICK_SIZE]);
#endif
