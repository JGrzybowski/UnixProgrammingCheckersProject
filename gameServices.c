#define _GNU_SOURCE
#include "gameServices.h"

void print_to_log(const char nickname[NICK_SIZE], const char* message){
	fprintf(stderr,"#%d %60s is %s\n", getpid(), nickname, message);
}

//TIME FORMATING 
//https://stackoverflow.com/questions/1442116/how-to-get-date-and-time-value-in-c-program

void append_surrender_to_game_log(gameID game_id, const char nickname[NICK_SIZE], struct tm timeInfo){
	char game_log_file_name[PATHSIZE] = {0};	
	char log_line[GAME_LOG_LINE_SIZE] = {0};
	int fd; 
	struct flock lock;
	//Generate name of the game log file.
	set_gamelog_file_name(game_log_file_name, game_id);
	//Open game log file and set locks.
	fd = open_with_lock(game_log_file_name, O_WRONLY | O_APPEND, &lock, WRITE_LOCK);
	if(fd < 0) 
		ERR("Opening game log file:");
 	//Generate line for the log and write it in the file and console.
	snprintf(log_line, GAME_LOG_LINE_SIZE, "%02d.%02d.%04d %02d:%02d:%02d %s surrenders!\n", 
			timeInfo.tm_mday, timeInfo.tm_mon+1, timeInfo.tm_year + 1900, 
			timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec, nickname);
		
	if(write_line(fd,log_line,GAME_LOG_LINE_SIZE) < 0) ERR("Writing to game log file:");
	//fprintf(stderr,log_line);
	
	if(unlock_with_close(fd, &lock) < 0) ERR("Unlocking gamelog file:");			
}

void append_move_to_game_log(gameID game_id, const char nickname[NICK_SIZE], unsigned char from, unsigned char to, int isBeating, struct tm timeInfo){
	char game_log_file_name[PATHSIZE] = {0};	
	char log_line[GAME_LOG_LINE_SIZE] = {0};
	int fd; 
	struct flock lock;
	char arrow = isBeating ? '=' : '-';
	//Generate name of the game log file.
	set_gamelog_file_name(game_log_file_name, game_id);
	//Open game log file and set locks.
	fd = open_with_lock(game_log_file_name, O_WRONLY | O_APPEND, &lock, WRITE_LOCK);
	if(fd < 0) 
		ERR("Opening game log file:");
	//Generate line for the log and write it in the file and console.
	snprintf(log_line, GAME_LOG_LINE_SIZE, "%02d.%02d.%04d %02d:%02d:%02d %s %c%c %c> %c%c\n", 
			timeInfo.tm_mday, timeInfo.tm_mon+1, timeInfo.tm_year + 1900, 
			timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec, 
			nickname,
			get_column_char(from), get_row_char(from), 
			arrow,
			get_column_char(to), get_row_char(to));
	if(write_line(fd,log_line,GAME_LOG_LINE_SIZE) < 0) ERR("Writing to game log file:");
	//fprintf(stderr,log_line);

	//Unlock and close the file
	if(unlock_with_close(fd, &lock) < 0) ERR("Unlocking gamelog file:");		
}

int is_player_allowed_to_move(const char nickname[NICK_SIZE], gameID game_id){
	int	player_nr = player_is_in_game(nickname, game_id);
	char status = get_game_status(game_id);
	if(player_nr > 0)
		switch(status){
			case AWAITING_FOR_OPPONENT:
			case P1_WON:
			case P2_WON: 
				return 0;
			case P1_TURN:
				return -player_nr + 2;
			case P2_TURN:
				return player_nr - 1;
			default:
				return -1;	
		}	
	else 
		return 0;	
}


void set_game_status(gameID game_id, char status){
	char file_name[PATHSIZE];
	struct flock lock;
	int fd;
	
	set_status_file_name(file_name, game_id);
	if((fd = open_with_lock(file_name, O_WRONLY | O_TRUNC, &lock, WRITE_LOCK)) > 0){
		fprintf(stderr,"Writing status %c \n",status); 
		write_line(fd, &status, sizeof(char));
		unlock_with_close(fd, &lock);
		//return 0;
	}
	//return -1;	
}

char get_game_status(gameID game_id){
	char status =-1;
	char file_name[PATHSIZE];
	struct flock lock;
	int fd = -1;
	
	set_status_file_name(file_name, game_id);
	fd = open_with_lock(file_name, O_RDONLY, &lock, READ_LOCK);
	fprintf(stderr,"#%d Comparing fd if  %d > 0 \n", getpid(), fd);
	if(fd > 0){
		//fprintf(stderr, "BEFORE read_line fd %d\n",fd);
		//TODO Investigate!!
		//read_line(fd, &status, sizeof(char));
		status = recv_code(fd);
		//fprintf(stderr, "AFTER read_line fd %d\n",fd);
		fprintf(stderr,"game's %d status %c \n", game_id, status);
		if(fd == 0) 
			ERR("BUZZZ:");
		unlock_with_close(fd, &lock);
		return status;
	}
	return -1;
}

void send_board(int clientFd, gameID game_id){
	int fd;
	struct flock lock;
	char board[BOARD_SIZE * BOARD_SIZE];
	char file_name[SERVER_FILE_PATH_SIZE];
	
	set_board_file_name(file_name, game_id);
	if((fd = open_with_lock(file_name, O_RDONLY, &lock, READ_LOCK)) > 0){
		read_line(fd, board, BOARD_SIZE * BOARD_SIZE);
		send_code(clientFd, SUCCESS_RESPONSE_CODE);
		send_buf(clientFd, board, BOARD_SIZE * BOARD_SIZE);
		unlock_with_close(fd,&lock);
	}
	//TODO replace -1
	else send_code(clientFd, -1);
}

void send_status(int clientFd, gameID game_id){
	char status;
	char response_code;
	//fprintf(stderr,"#%d CHECK_STATUS \n",getpid());
	status = get_game_status(game_id);
	if(status == P1_TURN || status == P2_TURN)
		response_code = ACTIVE;
	else if(status == P1_WON || status == P2_WON)
		response_code = RESOLVED;
	else if(status == AWAITING_FOR_OPPONENT)
		response_code = AWAITING;
	else 
		response_code = GAME_STATUS_ERROR;
	send_code(clientFd,response_code);
}

void send_turn(int clientFd, gameID game_id, int player_nr){
	char status; 
	char response_code;
	status = get_game_status(game_id);

	if(status == P1_TURN || status == P1_WON)
		response_code = YOUR_TURN;
	else if(status == P2_TURN || status == P2_WON)
		response_code = OPPONENTS_TURN;
	else if(status == AWAITING_FOR_OPPONENT)
		response_code = AWAITING;
	else 
		response_code = GAME_STATUS_ERROR;

	if(player_nr==2){
		if(response_code == OPPONENTS_TURN) 
			response_code = YOUR_TURN;
		else if (response_code == YOUR_TURN) 
			response_code = OPPONENTS_TURN;
	}
	send_code(clientFd,response_code);
}

void surrender_game(int clientFd, gameID game_id, const char nickname[NICK_SIZE], int player_nr){
	char status;
	int log_fd, status_fd;
	struct flock log_lock, status_lock;
	char game_log_file_name[SERVER_FILE_PATH_SIZE];
	char status_file_name[SERVER_FILE_PATH_SIZE];
	time_t t = time(NULL);
	struct tm timeInfo = *localtime(&t);
	
	
	set_gamelog_file_name(game_log_file_name, game_id);
	set_status_file_name(status_file_name, game_id);
	
	log_fd = open_with_lock(game_log_file_name, O_RDWR, &log_lock, WRITE_LOCK);
	status_fd = open_with_lock(status_file_name, O_RDWR, &status_lock, WRITE_LOCK);
	
	status = get_game_status(game_id);
	switch(status){
		case P1_TURN:
		case P2_TURN:
			if (player_nr == 1) status = P2_WON;
			else status = P1_WON;
		
			append_surrender_to_game_log(game_id, nickname, timeInfo);
			set_game_status(game_id, status);
			fprintf(stderr,"#%d Game %d got resolved. \n",getpid(), game_id);
			send_code(clientFd, SUCCESS_RESPONSE_CODE);
			break;
		case P1_WON:
		case P2_WON:
			send_code(clientFd, GAME_ALREADY_FINISHED);
			break;
		case AWAITING_FOR_OPPONENT:
			send_code(clientFd, GAME_HASNT_STARTED);
			break;		
		default:
			break;		
	}

	unlock_with_close(log_fd, &log_lock);
	unlock_with_close(status_fd, &status_lock);
}


void send_line_by_line(int clientFd, const char file_name[SERVER_FILE_PATH_SIZE]){
	int fd;
	struct flock lock;
	char msg_buffer[MSG_SIZE];
	
	if((fd = open_with_lock(file_name, O_RDONLY, &lock, READ_LOCK)) > 0){
		while(read_line(fd, msg_buffer, MSG_SIZE) > 0)
		{
			send_code(clientFd, SUCCESS_RESPONSE_CODE);
			send_msg(clientFd, msg_buffer);	
		}
		send_code(clientFd, END_OF_LIST_CODE);	
		unlock_with_close(fd,&lock);
	}
	else 
		//TODO set err number
		send_code(clientFd,-1);
}

void send_logs(int clientFd, gameID game_id){
	char game_log_file_name[SERVER_FILE_PATH_SIZE];
	set_gamelog_file_name(game_log_file_name, game_id);
	send_line_by_line(clientFd, game_log_file_name);
}

void send_messages(int clientFd, gameID game_id, int player_nr){
	char messages_file_name[SERVER_FILE_PATH_SIZE];
	set_msg_file_name(messages_file_name, game_id, player_nr);
	send_line_by_line(clientFd, messages_file_name);
}

void make_move(int clientFd, gameID game_id, int player_nr, const char nickname[NICK_SIZE]){
	char game_log_file_name[SERVER_FILE_PATH_SIZE];
	char status_file_name[SERVER_FILE_PATH_SIZE];
	char board_file_name[SERVER_FILE_PATH_SIZE];
	struct flock log_lock, status_lock, board_lock;
	int log_fd, status_fd, board_fd;
	char board[BOARD_SIZE *BOARD_SIZE];
	position_t from, to;	
	int is_beating = 0;
	char players_piece = (player_nr == 1) ? P1_PIECE : P2_PIECE;
	char opponents_piece = (player_nr == 1) ? P2_PIECE : P1_PIECE;
	char new_status = (player_nr == 1) ? P2_TURN : P1_TURN;
	char code;
	time_t t = time(NULL);
	struct tm timeInfo = *localtime(&t);
	
	set_gamelog_file_name(game_log_file_name, game_id);
	set_status_file_name(status_file_name, game_id);
	set_board_file_name(board_file_name, game_id);
	
	from = recv_code(clientFd);
	to = recv_code(clientFd);
	
	log_fd = open_with_lock(game_log_file_name, O_RDWR, &log_lock, WRITE_LOCK);
	status_fd = open_with_lock(status_file_name, O_RDWR, &status_lock, WRITE_LOCK);
	board_fd = open_with_lock(board_file_name, O_RDWR, &board_lock, WRITE_LOCK);
	
	if(is_player_allowed_to_move(nickname, game_id)){
		recv_buf(board_fd, board, BOARD_SIZE*BOARD_SIZE);
		if(board[from] == players_piece){
			if(board[to] == players_piece){
				code = BEATING_OWN_PIECE;
			}else{
				if(board[to] == opponents_piece)
					is_beating = 1;
			
				append_move_to_game_log(game_id,nickname, from,to, is_beating, timeInfo);

				if(ftruncate(board_fd,0)<0) ERR("ftruncate:");
				if(lseek(board_fd, 0, SEEK_SET) < 0) ERR("lseek:");
				board[from] = EMPTY_SPACE;
				board[to] = players_piece;
				send_buf(board_fd, board, BOARD_SIZE * BOARD_SIZE);
				
				if(ftruncate(status_fd,0) < 0) ERR("ftruncate:");
				if(lseek(status_fd, 0, SEEK_SET) < 0) ERR("lseek:");
				send_code(status_fd, new_status);
			
				code = SUCCESS_RESPONSE_CODE;
			}
		}else
			code = NO_YOUR_PIECE;	
	}else
		code = NOT_YOUR_TURN;
	
	unlock_with_close(log_fd, &log_lock);
	unlock_with_close(status_fd, &status_lock);
	unlock_with_close(board_fd, &board_lock);
	send_code(clientFd, code);
}

void save_message(int clientFd, gameID game_id, int player_nr){
	char message[MSG_SIZE] = {0};
	char msg_txt[MSG_TXT_SIZE] = {0};
	char msg_file_name[SERVER_FILE_PATH_SIZE];
	struct flock lock;
	int fd;
	time_t t = time(NULL);
	struct tm timeInfo = *localtime(&t);
	int opponents_nr = (player_nr == 1) ? 2 : 1;
		
	set_msg_file_name(msg_file_name, game_id, opponents_nr);
	
	fd = open_with_lock(msg_file_name, O_WRONLY | O_APPEND, &lock, WRITE_LOCK);
	if(fd > 0){
		snprintf(message, MSG_SIZE, "%02d.%02d.%04d %02d:%02d:%02d ", 
				timeInfo.tm_mday, timeInfo.tm_mon+1, timeInfo.tm_year + 1900, 
				timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
		
		//fprintf(stderr,"Reciving msg. \n");
		recv_buf(clientFd, msg_txt, MSG_TXT_SIZE);
		strncat(message, msg_txt, MSG_TXT_SIZE);
		//fprintf(stderr,"Text recieved: %s \n",msg_txt);
		//fprintf(stderr,"Final msg: %s\n",message);
		write_line(fd, message, MSG_SIZE);
		unlock_with_close(fd, &lock);
		send_code(clientFd, SUCCESS_RESPONSE_CODE);
	}
	else{
		//TODO error code
		send_code(clientFd, -1);
	}
}

void game_loop(int clientFd, gameID game_id, const char nickname[NICK_SIZE], int player_nr){
	char game_directory[SERVER_FILE_PATH_SIZE] = {0};
	char code;
	bool ingame = 1;
	//TODO check if game exists?
	snprintf(game_directory, SERVER_FILE_PATH_SIZE, "games/%d", game_id);
	
	while(ingame && ((code = recv_code(clientFd)) > 0)){
		switch(code){
			case STATUS_CODE:
				print_to_log(nickname,"checking the status of the game.");
				send_status(clientFd, game_id);
				break;
			case TURN_CODE:
				print_to_log(nickname,"checking the turn.");
				send_turn(clientFd, game_id, player_nr);
				break;
			case SURRENDER_CODE:
				print_to_log(nickname,"surrendering.");
				surrender_game(clientFd, game_id, nickname, player_nr);
				break;
			case LOG_CODE:
				print_to_log(nickname,"checking the log.");
				send_logs(clientFd, game_id);
				break;
			case READ_MSG_CODE:
				print_to_log(nickname,"reding the messages.");
				send_messages(clientFd, game_id, player_nr);
				break;
			case WRITE_MSG_CODE:
				print_to_log(nickname,"sending a message.");
				save_message(clientFd, game_id, player_nr);
				break;
			case MOVE_CODE: 
				print_to_log(nickname,"making a move.");
				make_move(clientFd, game_id, player_nr, nickname);
				break;
			case EXIT_GAME_CODE:
				print_to_log(nickname,"exiting a game");
				ingame = 0;
				send_code(clientFd, SUCCESS_RESPONSE_CODE);
				break;	
			case BOARD_CODE:
				print_to_log(nickname,"checking the board.");
				send_board(clientFd, game_id);
				break;					
			default:
				
				break;
	
		}	
	}	
}



