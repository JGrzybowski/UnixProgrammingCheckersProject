#ifndef UNIX_PROJECT_LOBBY_SERVICES
#define UNIX_PROJECT_LOBBY_SERVICES

#include "common.h"
#include "fileFunctions.h"
#include "board.h"
#include "gameServices.h"
#include "filePaths.h"

void connect_to_the_game(int clientFd, const char nickname[MSG_SIZE]);

void initialize_board(int board_fd);
int create_new_game_files(gameID game_id, const char nickname[NICK_SIZE]);
void join_game(gameID game_id, const char nickname[NICK_SIZE]);
gameID create_new_game(const char nickname[NICK_SIZE]);

void list_games(int clientFd, char nickname[NICK_SIZE]);

#endif
