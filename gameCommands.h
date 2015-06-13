#ifndef UNIX_PROJECT_GAME_COMMANDS
#define UNIX_PROJECT_GAME_COMMANDS

#include "common.h"
#include "fileFunctions.h"
#include "board.h"
#include "clientGameRules.h"

#define COMMAND_LINE_SIZE MSG_SIZE

#define SHOW_BOARD_COMMAND "board"
#define STATUS_COMMAND "status"
#define TURN_COMMAND "turn"
#define GAME_LOG_COMMAND "log"
#define READ_MSG_COMMAND "readmsg"
#define WRITE_MSG_COMMAND "writemsg"
#define MOVE_COMMAND "move"
#define MOVE_FULL_COMMAND "move [from] [to]"
#define SURRENDER_COMMAND "surrender"
#define EXIT_GAME_COMMAND "exit"
#define HELP_COMMAND "help"

void game_menu(int fd, gameID game_id);
void game_help();

#endif
