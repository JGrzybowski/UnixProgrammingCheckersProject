#include "lobbyServices.h"

void connect_to_the_game(int clientFd, const char nickname[MSG_SIZE]){
	gameID game_id;
	int i;
	
	if((game_id = recv_game_id(clientFd)) < 0)
		ERR("Reading game id for connecting:");
	
	if((i = player_is_in_game(nickname, game_id)) <= 0){
		send_code(clientFd, WRONG_GAME_ID_RESPONSE_CODE);
	}else{		
		//TODO Check if game directory exists
		send_code(clientFd, SUCCESS_RESPONSE_CODE);
		//fprintf(stderr,"check %d\n",i);
		game_loop(clientFd, game_id, nickname, i);
	}
}

void initialize_board(int board_fd){
	char board[BOARD_SIZE * BOARD_SIZE] = {0};
	int col,row,position;
	for(row=0;row<BOARD_SIZE;row++){
		for(col=0;col<BOARD_SIZE;col++){
			position = board_position(col,row);
			if((col+row)%2 == 0){
				if(row < ROWS_OF_PIECES)
					board[position] = P1_PIECE;
				else if(row > BOARD_SIZE-1-ROWS_OF_PIECES)
					board[position] = P2_PIECE;
				else 
					board[position] = EMPTY_SPACE;
			}else
				board[position] = WHITE_SPACE;				
		}
	
	}
	fprintf(stderr,"board :%s\n",board); 
	send_buf(board_fd,board, BOARD_SIZE*BOARD_SIZE);
}


int create_new_game_files(gameID game_id, const char nickname[NICK_SIZE]){
	int log_fd =-1, board_fd =-1, players_fd=-1, status_fd=-1, msg1_fd=-1, msg2_fd=-1;
	struct flock log_lock, board_lock, players_lock, status_lock, msg1_lock, msg2_lock;
	char file_name[SERVER_FILE_PATH_SIZE];
	
	snprintf(file_name, sizeof(file_name), "games/%d", game_id);
	
	if(mkdir(file_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0){
		fprintf(stderr, "#%d %s was created! \n", getpid(), file_name);
		//TODO move to different functions!
		set_gamelog_file_name(file_name, game_id);
		log_fd = open_with_lock_c(file_name, O_RDWR | O_CREAT, 
					S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH, &log_lock, WRITE_LOCK);	
		//fprintf(stderr, "FDs %d %d %d %d %d %d \n", players_fd, msg2_fd, msg1_fd, board_fd, status_fd, log_fd);
		
		set_status_file_name(file_name, game_id);
		status_fd = open_with_lock_c(file_name, O_RDWR | O_CREAT, 
					S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH, &status_lock, WRITE_LOCK);
		send_code(status_fd, AWAITING_FOR_OPPONENT);
		//set_game_status(game_id, AWAITING_FOR_OPPONENT);
		//fprintf(stderr, "FDs %d %d %d %d %d %d \n", players_fd, msg2_fd, msg1_fd, board_fd, status_fd, log_fd);
		
		set_board_file_name(file_name, game_id);
		board_fd = open_with_lock_c(file_name , O_RDWR | O_CREAT, 
					S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH, &board_lock, WRITE_LOCK);
		initialize_board(board_fd);	
		//fprintf(stderr, "FDs %d %d %d %d %d %d \n", players_fd, msg2_fd, msg1_fd, board_fd, status_fd, log_fd);
	
		set_msg_file_name(file_name, game_id, 1);
		msg1_fd = open_with_lock_c(file_name, O_RDWR | O_CREAT, 
					S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH, &msg1_lock, WRITE_LOCK);
		//fprintf(stderr, "FDs %d %d %d %d %d %d \n", players_fd, msg2_fd, msg1_fd, board_fd, status_fd, log_fd);
	
		set_msg_file_name(file_name, game_id, 2);
		msg2_fd = open_with_lock_c(file_name, O_RDWR | O_CREAT, 
					S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH, &msg2_lock, WRITE_LOCK);
		//fprintf(stderr, "FDs %d %d %d %d %d %d \n", players_fd, msg2_fd, msg1_fd, board_fd, status_fd, log_fd);
	
		set_players_file_name(file_name, game_id);
		players_fd = open_with_lock_c(file_name, O_RDWR | O_CREAT, 
					S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH, &players_lock, WRITE_LOCK);
		write_line(players_fd, nickname, NICK_SIZE);
		//fprintf(stderr, "FDs %d %d %d %d %d %d \n", players_fd, msg2_fd, msg1_fd, board_fd, status_fd, log_fd);
			
		add_game_to_list(game_id, nickname);
		
		fprintf(stderr, "Game %d files prepared, but still locked!\n", game_id);
		fprintf(stderr, "FDs %d %d %d %d %d %d\n", players_fd, msg2_fd, msg1_fd, board_fd, status_fd, log_fd);
	
		unlock_with_close(players_fd, &players_lock);
		unlock_with_close(msg2_fd, &msg2_lock);
		unlock_with_close(msg1_fd, &msg1_lock);
		unlock_with_close(board_fd, &board_lock);
		unlock_with_close(status_fd, &status_lock);
		unlock_with_close(log_fd, &log_lock);	
		fprintf(stderr, "Game %d prepared!\n", game_id);
		return 1;
	}
	return -1;
}

void join_game(gameID game_id, const char nickname[NICK_SIZE]){
	int log_fd, status_fd, players_fd;
	struct flock log_lock, status_lock, players_lock;
	char log_file_name[SERVER_FILE_PATH_SIZE];
	char status_file_name[SERVER_FILE_PATH_SIZE];
	char players_file_name[SERVER_FILE_PATH_SIZE];		
	
	//Construct files' names
	set_gamelog_file_name(log_file_name, game_id);
	set_status_file_name(status_file_name, game_id);
	set_players_file_name(players_file_name, game_id);
	
	//Open log, status, players
	log_fd = open_with_lock(log_file_name, O_RDWR | O_APPEND, &log_lock, WRITE_LOCK);
	status_fd = open_with_lock(status_file_name, O_RDWR, &status_lock, WRITE_LOCK);
	players_fd = open_with_lock(players_file_name, O_RDWR | O_APPEND, &players_lock, WRITE_LOCK);
	
	//TODO log joining
	
	//set status P1_TURN
	set_game_status(game_id, P1_TURN);
	
	//append nickname to players
	write_line(players_fd, nickname, NICK_SIZE);
	
	//open player's game list file
	//Append game to games list
	add_game_to_list(game_id, nickname);
		
	//unlock with close all files
	unlock_with_close(log_fd, &log_lock);
	unlock_with_close(status_fd, &status_lock);
	unlock_with_close(players_fd, &players_lock);
	
}


gameID create_new_game(const char nickname[NICK_SIZE]){
	char awaiting_game_file_name[SERVER_FILE_PATH_SIZE];
	char awaiting_player_file_name[SERVER_FILE_PATH_SIZE];
	char buffer[GAME_LOG_LINE_SIZE];
	char game_creator[NICK_SIZE];
	gameID awaiting_game_id = -1;
	int awa_game_fd,awa_player_fd;
	struct flock awa_game_lock,awa_player_lock;
	
	set_awaiting_game_file_name(awaiting_game_file_name);
	set_awaiting_player_file_name(awaiting_player_file_name);
	
	awa_game_fd = open_with_lock(awaiting_game_file_name, O_RDWR, &awa_game_lock, WRITE_LOCK);
	awa_player_fd = open_with_lock(awaiting_player_file_name, O_RDWR, &awa_player_lock, WRITE_LOCK);
	
	//TODO
	read_line(awa_game_fd, buffer, GAME_LOG_LINE_SIZE);
	fprintf(stderr, "New game's id: %s\n", buffer);
	parse_out_game_id(buffer, GAME_LOG_LINE_SIZE, &awaiting_game_id);
	//awaiting_game_id = recv_game_id(awa_game_fd);
	read_line(awa_player_fd, game_creator, NICK_SIZE);
	fprintf(stderr, "Awaiting player: !%s!\n", game_creator);
	
	//CREATE NEW GAME
	if (! ((game_creator[0] >= '0' && game_creator[0] <= '9') || 
			(game_creator[0] >= 'a' && game_creator[0] <= 'z') ||
			(game_creator[0] >= 'A' && game_creator[0] <= 'Z')) ){
		create_new_game_files(awaiting_game_id, nickname);
				
		if(ftruncate(awa_player_fd, 0) < 0) ERR("ftruncate:");
		if(lseek(awa_player_fd, 0, SEEK_SET) < 0) ERR("lseek:");
		write_line(awa_player_fd, nickname, NICK_SIZE);
	//JOIN GAME
	}else{
		join_game(awaiting_game_id, nickname);
		
		if(ftruncate(awa_game_fd, 0) < 0) ERR("ftruncate:");
		if(lseek(awa_game_fd, 0, SEEK_SET) < 0) ERR("lseek:");
		snprintf(buffer, sizeof(buffer), "%d", awaiting_game_id+1);
		write_line(awa_game_fd, buffer, GAME_LOG_LINE_SIZE);
		
		if(ftruncate(awa_player_fd, 0) < 0) ERR("ftruncate:");
	}
	
	unlock_with_close(awa_game_fd, &awa_game_lock);
	unlock_with_close(awa_player_fd, &awa_player_lock);
		
	return awaiting_game_id;
}

/*
 * Function:  list_games
 * --------------------
 * sends all player's gameIDs one by one :
 * 
 *   nickname:	the buffer with player's nickname
 *   clientFd:	file descriptor of connection with client
 *
 */
void list_games(int clientFd, char nickname[NICK_SIZE]){
	int list_fd;
	char file_name[SERVER_FILE_PATH_SIZE] = {0};
	gameID game_id;
	struct flock lock;
	
	fprintf(stderr,"#%d Listing games.\n",getpid());
	//Create file name
	set_players_games_file_name(file_name, nickname);

	//Open players file
	if((list_fd = open_with_lock(file_name,O_RDONLY, &lock, READ_LOCK)) < 0) ERR("Opening file:");
	
	//Lock file
	//read_lock_file(list_fd, &lock);
	while(read_game_id_line(list_fd, &game_id)){
		fprintf(stderr,"#%d %d \n", getpid(), game_id);
		send_code(clientFd, SUCCESS_RESPONSE_CODE);
		send_game_id(clientFd, game_id);	
	}
	fprintf(stderr,"#%d End of list. \n", getpid());
	send_code(clientFd, END_OF_LIST_CODE);
	//Unlock file
	//unlock_file(list_fd, &lock);
	//Close file
	if(unlock_with_close(list_fd, &lock) > 0) ERR("Closing player's games list file:");

}




