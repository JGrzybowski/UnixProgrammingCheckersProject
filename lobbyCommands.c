#include "lobbyCommands.h"


void lobby_help(){
	fprintf(stdout,"%18s - start a new game.\n",NEW_GAME_COMMAND);
	fprintf(stdout,"%18s - connect to the game with given gameID.\n",CONNECT_GAME_FULL_COMMAND);
	fprintf(stdout,"%18s - prints out the list of your gameIDs.\n", LIST_GAMES_COMMAND);
	fprintf(stdout,"%18s - print out this help.\n", HELP_COMMAND);
	fprintf(stdout,"%18s - close this program.\n", QUIT_COMMAND);
}

void start_new_game(int fd){
	char code;
	gameID game_id;
	send_code(fd,NEW_GAME_CODE);
	code = recv_code(fd);
	switch(code){
		case SUCCESS_RESPONSE_CODE:
			game_id = recv_game_id(fd);
			fprintf(stdout, "New game's id: %d\n", game_id);
			break;
		default:
			fprintf(stdout, "Error occured (errcode: %d)\n",code);
	}	
}

void read_games_list(int fd){
	char code;
	gameID game_id = 0;	
	
	send_code(fd, LIST_GAMES_CODE);
	while((code = recv_code(fd)) == SUCCESS_RESPONSE_CODE){
		game_id = recv_game_id(fd);
		fprintf(stdout, "[%d] %d \n", code, game_id);
	}
	fprintf(stderr,"[%d] end of list \n", code);
	//TODO check last RESPONSE CODE if end of list
}

void quit(int fd){
	fprintf(stdout, "Thanks for playing. Bye! :D\n");
}

