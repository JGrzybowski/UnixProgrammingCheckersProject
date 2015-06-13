#ifndef UNIX_PROJECT_FILE_PATHS
#define UNIX_PROJECT_FILE_PATHS

#include "common.h"
#include "fileFunctions.h"

void set_status_file_name(char file_name_buffer[SERVER_FILE_PATH_SIZE], gameID game_id);
void set_board_file_name(char file_name_buffer[SERVER_FILE_PATH_SIZE], gameID game_id);
void set_players_file_name(char file_name_buffer[SERVER_FILE_PATH_SIZE], gameID game_id);
void set_gamelog_file_name(char file_name_buffer[SERVER_FILE_PATH_SIZE], gameID game_id);
void set_msg_file_name(char file_name_buffer[SERVER_FILE_PATH_SIZE], gameID game_id, int player_nr);
void set_players_games_file_name(char file_name_buffer[SERVER_FILE_PATH_SIZE], const char nickname[NICK_SIZE]);
void set_awaiting_game_file_name(char file_name_buffer[SERVER_FILE_PATH_SIZE]);
void set_awaiting_player_file_name(char file_name_buffer[SERVER_FILE_PATH_SIZE]);

#endif 
