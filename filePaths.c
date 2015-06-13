#include "filePaths.h"

void set_status_file_name(char file_name_buffer[SERVER_FILE_PATH_SIZE], gameID game_id){
	snprintf(file_name_buffer, SERVER_FILE_PATH_SIZE, "games/%d/status", game_id);
}
void set_board_file_name(char file_name_buffer[SERVER_FILE_PATH_SIZE], gameID game_id){
	snprintf(file_name_buffer, SERVER_FILE_PATH_SIZE, "games/%d/board", game_id);
}
void set_players_file_name(char file_name_buffer[SERVER_FILE_PATH_SIZE], gameID game_id){
	snprintf(file_name_buffer, SERVER_FILE_PATH_SIZE, "games/%d/players", game_id);
}
void set_gamelog_file_name(char file_name_buffer[SERVER_FILE_PATH_SIZE], gameID game_id){
	snprintf(file_name_buffer, SERVER_FILE_PATH_SIZE, "games/%d/gamelog", game_id);
}
void set_msg_file_name(char file_name_buffer[SERVER_FILE_PATH_SIZE], gameID game_id, int player_nr){
	snprintf(file_name_buffer, SERVER_FILE_PATH_SIZE, "games/%d/player%dmsg", game_id, player_nr);
}
void set_players_games_file_name(char file_name_buffer[SERVER_FILE_PATH_SIZE], const char nickname[NICK_SIZE]){
	snprintf(file_name_buffer, SERVER_FILE_PATH_SIZE, "players/%s", nickname);
}
void set_awaiting_game_file_name(char file_name_buffer[SERVER_FILE_PATH_SIZE]){
	snprintf(file_name_buffer, SERVER_FILE_PATH_SIZE, "awaiting_game");
}
void set_awaiting_player_file_name(char file_name_buffer[SERVER_FILE_PATH_SIZE]){
	snprintf(file_name_buffer, SERVER_FILE_PATH_SIZE, "awaiting_player");
}
