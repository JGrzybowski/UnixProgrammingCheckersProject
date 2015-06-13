#include "playerServices.h"

/*
 * Function: is_valid_nickname
 * ----------------------------
 * Checks is given c string is a proper nickname and doesn't contain andy special characters.
 *	
 *	nickname: validated nickname
 *	
 *	returns:	 0  if passed string is a proper nickname
 *				-1  if passed string is not a proper nickname 
 *					(is empty string or contains forbidden symbols)
 *
 */
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


/*
 * Function:  player_is_in_game
 * --------------------
 * checks if player with given nickname is one of the players playing the game with gameID :
 * 
 *   nickname:	the buffer with player's nickname
 *   buffer:	the buffer with a message send to the server containing a gameID in ARG2
 *
 *   returns:  	 2 if player with given nickname was player2 in the game
 *				 1 if player with given nickname was player1 in the game
 *           	 0 if player is not one of the players of the game.
 *           	-1 if error happened.
 */
int player_is_in_game(const char nickname[NICK_SIZE], gameID game_id){
	int fd =-1;
	char file_name[SERVER_FILE_PATH_SIZE] = {0};
	char txt_buffer[NICK_SIZE] = {0};
	struct flock lock;
	//Create file name
	set_players_file_name(file_name, game_id);

	//open players file
	if((fd = open_with_lock(file_name,O_RDONLY, &lock, READ_LOCK)) > 0){
		if(read_line(fd,txt_buffer,NICK_SIZE) < 0) ERR("SERVER read players 1st line:");
		//fprintf(stderr,"buf: |%s| nick: |%s| \n",txt_buffer, nickname);
		if(strstr(txt_buffer,nickname) != NULL){
				if(fd == 0) ERR("BUZZZ:");
			unlock_with_close(fd, &lock);
			return 1;
		} 
		if(read_line(fd,txt_buffer,NICK_SIZE) < 0) ERR("SERVER read players 2nd line:");
		//fprintf(stderr,"buf: |%s| nick: |%s| \n",txt_buffer, nickname);
		if(strstr(txt_buffer,nickname) != NULL){
				if(fd == 0) ERR("BUZZZ:");
			unlock_with_close(fd, &lock);
			return 2;
		}
		if(fd == 0) ERR("BUZZZ:");
		unlock_with_close(fd, &lock);
		return 0;	
	}
	fprintf(stderr,"Error during player_is_in_game: %d\n", fd);
	return fd;
}


void add_game_to_list(gameID game_id, const char nickname[NICK_SIZE]){
	char player_file_name[SERVER_FILE_PATH_SIZE];
	int player_fd;
	struct flock player_lock;
	//TODO fix buffer size
	char buffer[128];
	
	snprintf(buffer, 128, "%d", game_id);
	set_players_games_file_name(player_file_name, SERVER_FILE_PATH_SIZE, "players/%s", nickname);
	player_fd = open_with_lock_c(player_file_name, O_RDWR | O_APPEND | O_CREAT, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH , &player_lock, WRITE_LOCK);
	write_line(player_fd, buffer, 128);
	unlock_with_close(player_fd, &player_lock);	
}
