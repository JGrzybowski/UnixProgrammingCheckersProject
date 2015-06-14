#include "common.h"
#include "fileFunctions.h"
#include "board.h"
#include "gameCommands.h"
#include "lobbyCommands.h"
#include "clientGameRules.h"


void usage(char * name){
	fprintf(stderr,"USAGE: %s domain port \n",name);
}

//TODO move to player helpers
int is_valid_nickname(char nickname[NICK_SIZE]){
	int i;
	for(i=0;i<NICK_SIZE;i++){
		//fprintf(stderr, "%d \n", nickname[i]);		
		if(nickname[i] == '\0' || nickname[i] == '\n'){
			if(nickname[i] == '\n')			
				nickname[i] = '\0';
			if(i == 0) return -1;
			else return 0;
		}
		if (! ((nickname[i] >= '0' && nickname[i] <= '9') || 
			(nickname[i] >= 'a' && nickname[i] <= 'z') ||
			(nickname[i] >= 'A' && nickname[i] <= 'Z')) )
			return -1;			
	}
	return 0;
}

int login(int fd, char nickname[NICK_SIZE]){
	char response_code;	
	ssize_t size;
	//Read the player's nickname
	do{
		fprintf(stdout,"Put your nickname (max. %d alphanumerical characters only): ", NICK_SIZE);
		read_line(STDIN_FILENO, nickname, NICK_SIZE);
	}while(is_valid_nickname(nickname)==-1);
	//Send the messsage
	send_code(fd, LOG_IN_CODE); 
	send_buf(fd, nickname, NICK_SIZE);
	//Recieve and print the reponse
	if((size = TEMP_FAILURE_RETRY(bulk_read(fd,&response_code,RESPONSE_STATUS_SIZE))) < 0) ERR("Reciving the login response:"); 
	
	if(response_code == SUCCESS_RESPONSE_CODE){
		fprintf(stdout, "Welcome %s! Remember you can always type \"help\" to get list of avaliable commands.\n", nickname);
		return 0;
	}else{
		fprintf(stdout, "Sorry %s! Something went wrong. Restart the program. \n", nickname);
		return -1;
	}
}

void lobby_menu(int fd, char nickname[NICK_SIZE]){
	char command_buffer[COMMAND_LINE_SIZE] = {0};
	char *command_place;
	gameID game_id = 0;
	
	fprintf(stdout, "%s>", nickname);
	
	while(read_line(STDIN_FILENO, command_buffer,COMMAND_LINE_SIZE) > 0){
		//NEW GAME
		if((command_place = strstr(command_buffer,NEW_GAME_COMMAND)) == command_buffer){
			start_new_game(fd);
		//CONNECT
		}else if((command_place = strstr(command_buffer,CONNECT_GAME_COMMAND)) == command_buffer){
			send_code(fd, CONNECT_CODE);
			parse_out_game_id(command_buffer+7, sizeof(command_buffer) - sizeof(CONNECT_GAME_COMMAND) +1, &game_id);
			send_game_id(fd, game_id);
			fprintf(stdout, "Connecting to %d...\n", game_id);
			if(recv_code(fd) == SUCCESS_RESPONSE_CODE){
				game_menu(fd, game_id);	
			}else
				fprintf(stdout,"Error connecting to %d.\n", game_id);
		//LIST GAMES		
		}else if ((command_place = strstr(command_buffer,LIST_GAMES_COMMAND)) == command_buffer){
			read_games_list(fd);
		//HELP
		}else if((command_place = strstr(command_buffer,HELP_COMMAND)) == command_buffer){
			lobby_help();
		//QUIT
		}else if((command_place = strstr(command_buffer,QUIT_COMMAND)) == command_buffer){		
			quit(fd);
			break;
		}else 
			fprintf(stdout,"Wrong command. Try again.\n");
		fprintf(stdout, "%s>", nickname);
	}
}


int main(int argc, char** argv) {
	int fd;
	char nickname[NICK_SIZE] = {0};
	setbuf(stdout, NULL);
	if(argc!=3) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}
	if(sethandler(SIG_IGN,SIGPIPE)) ERR("Seting SIGPIPE:");
	fd=connect_socket(argv[1],atoi(argv[2]));
	
	if(login(fd,nickname) == 0)	
		lobby_menu(fd, nickname);

	if(TEMP_FAILURE_RETRY(close(fd))<0)ERR("close");
	return EXIT_SUCCESS;
}
