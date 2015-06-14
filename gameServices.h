#ifndef UNIX_PROJECT_GAME_SERVICES
#define UNIX_PROJECT_GAME_SERVICES

#include "fileFunctions.h"
#include "board.h"
#include "playerServices.h"
#include "common.h"
#include "filePaths.h"


//Internal game statuses
#define P1_TURN '1'
#define P2_TURN '2'
#define P1_WON '3'
#define P2_WON '4'
#define AWAITING_FOR_OPPONENT '5'
	
void append_surrender_to_game_log(gameID game_id, const char nickname[NICK_SIZE], struct tm timeInfo);
void append_move_to_game_log(gameID game_id, const char nickname[NICK_SIZE], unsigned char from, unsigned char to, int isBeating, struct tm timeInfo);
int is_player_allowed_to_move(const char nickname[NICK_SIZE], gameID game_id);
void set_game_status(gameID game_id, char status);
char get_game_status(gameID game_id);

void send_board(int clientFd, gameID game_id);
void send_status(int clientFd, gameID game_id);
void send_turn(int clientFd, gameID game_id, int player_nr);
void surrender_game(int clientFd, gameID game_id, const char nickname[NICK_SIZE], int player_nr);

void send_logs(int clientFd, gameID game_id);
void send_messages(int clientFd, gameID game_id, int player_nr);

void make_move(int clientFd, gameID game_id, int player_nr, const char nickname[NICK_SIZE]);

void save_message(int clientFd, gameID game_id, int player_nr);
void game_loop(int clientFd, gameID game_id, const char nickname[NICK_SIZE], int player_nr);


#endif
