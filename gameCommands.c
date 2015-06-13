#include "gameCommands.h"

void game_help(){
	fprintf(stdout,"%18s - display the board\n", SHOW_BOARD_COMMAND);
	fprintf(stdout,"%18s - check game status (active, resolved, waiting for opponent)\n", STATUS_COMMAND);
	fprintf(stdout,"%18s - check the turn (mine,opponents)\n", TURN_COMMAND);
	fprintf(stdout,"%18s - list all moves made\n", GAME_LOG_COMMAND);
	fprintf(stdout,"%18s - read messages from the opponent\n", READ_MSG_COMMAND);
	fprintf(stdout,"%18s - make move in form of coordinates e.g. b6 to c6\n", WRITE_MSG_COMMAND);
	fprintf(stdout,"%18s - leave any number of messages to the opponent\n", MOVE_FULL_COMMAND);
	fprintf(stdout,"%18s - give up\n", SURRENDER_COMMAND);
	fprintf(stdout,"%18s - print out this help\n", HELP_COMMAND);
	fprintf(stdout,"%18s - leaves the game room, but not surrender\n", EXIT_GAME_COMMAND);
}

void exit_game(int fd, gameID game_id){
	char code;
	send_code(fd, EXIT_GAME_CODE);
	if((code = recv_code(fd)) != SUCCESS_RESPONSE_CODE) 
		ERR("Couldn't exit game properly. Closing client:");
	fprintf(stdout,"Exiting game %d.\n", game_id);
} 

void check_board(int fd){
	char code;
	char board[BOARD_SIZE * BOARD_SIZE];
	
	send_code(fd, BOARD_CODE);
	code = recv_code(fd);
	if(code == SUCCESS_RESPONSE_CODE){
		recv_buf(fd, board, BOARD_SIZE * BOARD_SIZE);
		print_board(board);	
	}
	else
		fprintf(stdout,"Checking board failed. Try again in a moment. (Err: %d)\n", code);			
}

void check_status(int fd){
	char status = -1;
	send_code(fd, STATUS_CODE);
	status = recv_code(fd);
	switch (status){
		case ACTIVE:
			fprintf(stdout,"Game active.\n");		
			break;
		case RESOLVED:		
			fprintf(stdout,"Game resolved.\n");		
			break;
		case AWAITING:			
			fprintf(stdout,"Awaiting for an oponent.\n");
			break;
		default:
			fprintf(stdout,"Error occurred. errcode %d\n", status);					
			break;
	}
}

void check_turn(int fd){
	char status = -1;
	send_code(fd, TURN_CODE);
	status = recv_code(fd);
	switch (status){
		case YOUR_TURN:
			fprintf(stdout,"Your turn.\n");		
			break;
		case OPPONENTS_TURN:		
			fprintf(stdout,"Opponent's Turn.\n");		
			break;
		case AWAITING:			
			fprintf(stdout,"Awaiting for an oponent.\n");
			break;
		default:
			fprintf(stdout,"Error occurred. errcode %d\n", status);					
			break;
	}
}

void surrender(int fd){
	char code;
	send_code(fd, SURRENDER_CODE);
	code = recv_code(fd);
	switch (code){
		case SUCCESS_RESPONSE_CODE:
			fprintf(stdout,"You lose this game.\n");
			break;
		case GAME_ALREADY_FINISHED:
			fprintf(stdout,"Game is already finished.\n");
			break;
		case GAME_HASNT_STARTED:
			fprintf(stdout,"The game hasn't started yet.\n");
			break;
		default:
			fprintf(stdout,"I don't know how to say it.. You failed to lose. \n");	
			break;
	}
}

void read_line_by_line(int fd){
	char code;
	char msg_buffer[MSG_SIZE] = {0};
	while((code = recv_code(fd)) == SUCCESS_RESPONSE_CODE){
		recv_msg(fd,msg_buffer);
		fprintf(stdout,"%s\n",msg_buffer);
	}
	if(code != END_OF_LIST_CODE)
		//TODO
		fprintf(stdout,"Error during reading the list.\n");
}

void read_logs(int fd){
	send_code(fd, LOG_CODE);
	read_line_by_line(fd);
}

void read_messages(int fd){
	send_code(fd,READ_MSG_CODE);
	read_line_by_line(fd);
}


void make_move(int fd, const char command_buffer[COMMAND_LINE_SIZE]){
	int from_col=-1, to_col=-1;
	char from_row=-1, to_row=-1;
	position_t from;
	position_t to;
	char code;
	int possibility;

	//TODO fix scanning for moves
	//fprintf(stdout, "#%s#\n", command_buffer);
	sscanf(command_buffer, "move %c%d %c%d", &from_row, &from_col, &to_row, &to_col);
	//fprintf(stdout,"Move %c,%d -> %c,%d \n", from_row, from_col, to_row, to_col);
	
	if(from_col==-1 || from_row==-1 || to_col==-1 || to_row==-1){
		fprintf(stdout,"Wrong fromat of move.\n");
		return;
	}
	
	from = get_board_position(from_col, from_row);
	to = get_board_position(to_col, to_row);
//	fprintf(stderr,"from %d [%d,%d] to %d [%d,%d] \n", from, from_row, from_col, to, to_row, to_col);
		fprintf(stderr,"from %d to %d\n", from, to);
	if ((possibility = is_possible_move(from, to)) != POSSIBLE_MOVE){
		fprintf(stdout,"Improper move. (%d)\n",possibility);
		return;
	}
	
	send_code(fd, MOVE_CODE);
	send_code(fd, from);
	send_code(fd, to);
	code = recv_code(fd);
	switch(code){
		case SUCCESS_RESPONSE_CODE:
			fprintf(stdout,"Move sent.\n");
			break;
		case NOT_YOUR_TURN:
			fprintf(stdout,"It's not Your turn! Wait for opponent's move.\n");
			break;
		case NO_YOUR_PIECE:
			fprintf(stdout,"In field You picked there is no Your piece.\n");
			break;
		case BEATING_OWN_PIECE:
			fprintf(stdout,"You cannot beat Your own piece.\n");
			break;
		default:
			fprintf(stdout,"Error.\n");
			break;
	}
}

void write_message(int fd){
	char message[MSG_TXT_SIZE] ={0};
	char code;
	fprintf(stdout,"Write a single-line message (max. %d characters): ",MSG_TXT_SIZE);
	fflush(stdout);
	if(read_line(STDIN_FILENO,message,MSG_TXT_SIZE) <= 0){
		fprintf(stdout,"Cannot send empty message. \n");
		return;
	}
	send_code(fd, WRITE_MSG_CODE);
	send_buf(fd, message, MSG_TXT_SIZE);
	code = recv_code(fd);
	switch(code){
		case SUCCESS_RESPONSE_CODE:
			fprintf(stdout,"Message sent. \n");
			break;
		default:		
			fprintf(stdout,"Error occured when sending the message. \n");
			break;	
	}	
}

void game_menu(int fd, gameID game_id){
	char command_buffer[COMMAND_LINE_SIZE] = {0};
	char *command_place;
	fprintf(stdout, "%d>", game_id);
	fflush(stdout);
	
	while(read_line(STDIN_FILENO, command_buffer,COMMAND_LINE_SIZE) > 0){
		//STATUS
		if((command_place = strstr(command_buffer,STATUS_COMMAND)) == command_buffer){
			check_status(fd);
		//TURN
		}else if((command_place = strstr(command_buffer,TURN_COMMAND)) == command_buffer){
			check_turn(fd);
		//LOG
		}else if((command_place = strstr(command_buffer,GAME_LOG_COMMAND)) == command_buffer){
			read_logs(fd);
		//READ MSG
		}else if((command_place = strstr(command_buffer,READ_MSG_COMMAND)) == command_buffer){
			read_messages(fd);
		//WRITE MSG
		}else if((command_place = strstr(command_buffer,WRITE_MSG_COMMAND)) == command_buffer){
			write_message(fd);	
		//MOVE
		}else if((command_place = strstr(command_buffer,MOVE_COMMAND)) == command_buffer){
			make_move(fd, command_buffer);
		//SURRENDER
		}else if((command_place = strstr(command_buffer,SURRENDER_COMMAND)) == command_buffer){
			surrender(fd);	
		//BOARD
		}else if((command_place = strstr(command_buffer,SHOW_BOARD_COMMAND)) == command_buffer){
			check_board(fd);
		//HELP
		}else if((command_place = strstr(command_buffer,HELP_COMMAND)) == command_buffer){
			game_help();
		//EXIT
		}else if((command_place = strstr(command_buffer,EXIT_GAME_COMMAND)) == command_buffer){
			exit_game(fd, game_id);
			break;
		}else 
			fprintf(stdout,"Wrong command. Try again.\n");
		fprintf(stdout, "%d>", game_id);
		fflush(stdout);
	}
}
