#define _GNU_SOURCE
#include "common.h"
#include "fileFunctions.h"
#include "gameServices.h"
#include "playerServices.h"
#include "lobbyServices.h"
#include "filePaths.h"

#define MAX_LOG_MESSAGE_SIZE 2048

volatile sig_atomic_t do_work=1 ;
volatile sig_atomic_t active_players = 0;

void sigint_handler(int sig) {
	do_work=0;
}

void sigchld_handler(int sig) {
	pid_t pid;
	for(;;){
		pid=waitpid(0, NULL, WNOHANG);
		if(0==pid) return;
		if(0>=pid) {
			if(ECHILD==errno) return;
			ERR("waitpid:");
		}
		active_players--;
	}
}

void usage(char * name){
	fprintf(stderr,"USAGE: %s port\n",name);
}

int login(int clientFd, char nickname[NICK_SIZE]){
	char code;
	char buffer[NICK_SIZE] = {0};	
	
	if((code = recv_code(clientFd)) != LOG_IN_CODE){
		send_code(clientFd, IMPROPER_COMMAND_CODE); 
		return IMPROPER_COMMAND_CODE;
	}
	if(bulk_read(clientFd, buffer, NICK_SIZE) < NICK_SIZE ) ERR("Reading nickname:");
	
	strncpy(nickname, buffer, NICK_SIZE);
	send_code(clientFd, SUCCESS_RESPONSE_CODE); 
	return 0;
}

void serve(int clientFd){
	ssize_t size;
	gameID game_id;
	char buffer[MSG_SIZE];
	char nickname[NICK_SIZE];

	fprintf(stderr,"#%d connected to the new client. \n", getpid());

	//Login the user
	login(clientFd, nickname);
	
	while(do_work && ((size=bulk_read(clientFd, buffer, sizeof(char[CODE_SIZE]))) != 0) ){
		if(size<0) ERR("SERVER - reading a message:");
		switch (buffer[0]){
			case CONNECT_CODE:
				connect_to_the_game(clientFd, nickname);
				break;
			case NEW_GAME_CODE:
				if((game_id = create_new_game(nickname)) > 0){
					send_code(clientFd, SUCCESS_RESPONSE_CODE);
					send_game_id(clientFd, game_id);
				}else 
					send_code(clientFd, -1);
				break;
			case LIST_GAMES_CODE:
				list_games(clientFd, nickname);
				break;
			case QUIT_CODE:
				exit(EXIT_SUCCESS);
			default:
				send_code(clientFd, IMPROPER_COMMAND_CODE);				
				ERR("Wrong Command Code:");
		}
	}
	if(TEMP_FAILURE_RETRY(close(clientFd))<0)ERR("close");
}

void doServer(int socket){
	int clientFd;
	fd_set base_rfds, rfds ;
	sigset_t mask, oldmask;
	FD_ZERO(&base_rfds);
	FD_SET(socket, &base_rfds);
	sigemptyset (&mask);
	sigaddset (&mask, SIGINT);
	sigprocmask (SIG_BLOCK, &mask, &oldmask);
	while(do_work){
		rfds=base_rfds;
		if(pselect(socket+1,&rfds,NULL,NULL,NULL,&oldmask)>0){
			clientFd = add_new_client(socket);
			if(clientFd>=0) 
				switch (fork()){
					case 0:
						serve(clientFd);
						exit(EXIT_SUCCESS);
					case -1: ERR("Fork:");
					default:
						active_players++;
						fprintf(stderr,"#%d Total active players:%d \n", getpid(), active_players);
						if(TEMP_FAILURE_RETRY(close(clientFd))) ERR("close");
				}
		}else{
			if(EINTR==errno) continue;
			ERR("pselect");
		}
	}
	sigprocmask (SIG_UNBLOCK, &mask, NULL);
}

int main(int argc, char** argv) {
	int socket;
	int new_flags;
	if(argc!=2) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}
	if(sethandler(sigchld_handler,SIGCHLD)) ERR("Seting SIGCHLD:");
	if(sethandler(SIG_IGN,SIGPIPE)) ERR("Seting SIGPIPE:");
	if(sethandler(sigint_handler,SIGINT)) ERR("Seting SIGINT:");
	socket=bind_tcp_socket(atoi(argv[1]));
	fprintf(stderr,"#Initialization finished#\n");
	
	new_flags = fcntl(socket, F_GETFL) | O_NONBLOCK;
	fcntl(socket, F_SETFL, new_flags);

	doServer(socket);

	if(TEMP_FAILURE_RETRY(close(socket))<0) ERR("close");
	fprintf(stderr,"Server has terminated.\n");
	return EXIT_SUCCESS;
}
