#include "common.h"
#include "gameCommands.h"


#ifndef UNIX_PROJECT_LOBBY_COMMANDS
#define UNIX_PROJECT_LOBBY_COMMANDS

#define NEW_GAME_COMMAND "new"
#define CONNECT_GAME_COMMAND "connect"
#define CONNECT_GAME_FULL_COMMAND "connect [gameID]" 
#define LIST_GAMES_COMMAND "mygames"
#define QUIT_COMMAND "quit"

void lobby_help();
void start_new_game(int fd);
void read_games_list(int fd);
void quit(int fd);
#endif
