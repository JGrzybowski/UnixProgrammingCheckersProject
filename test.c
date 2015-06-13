#include "common.h"
#include "playerServices.h"
#include "board.h"
#include "gameServices.h"

int main(int argc, char** argv) {
	gameID id;
	struct flock lock;
	int fd;
	int i;
	char buffer[1024];
	char board[BOARD_SIZE * BOARD_SIZE] = { 48,49,50,51,52,53,54,55,56,57 };
	time_t t = time(NULL);
	struct tm timem = *localtime(&t);

	memset(buffer,0,1024);

/*	if((fd = open("./testTXT",O_RDONLY)) > 0){*/
/*		while(read_line(fd,buffer,1024) > 0)*/
/*			fprintf(stdout,"# %s \n",buffer);	*/
/*	}*/
/*	close(fd);*/
/*	fprintf(stdout, "%d \n", fd);*/
		
	//for(i=0; i<16; i++)
	//	fprintf(stdout,"%c%c \n",get_column_char(i),get_row_char(i));	

	
	//append_surrender_to_game_log(1213,"Loser", timem);
	//append_move_to_game_log(1213,"Me", 6,13, 0, timem);
	//append_move_to_game_log(1213,"Me", 6,13, 1, timem);
	
		
/*	if((fd = open_with_lock("./testTXT", O_RDONLY, &lock, READ_LOCK)) > 0){	*/
/*		read_line(fd, buffer, 1024);*/
/*		unlock_with_close(fd,&lock);*/
/*		fprintf(stdout, "%s \n", buffer);*/
/*	}*/
/*	fprintf(stdout, "%d \n", fd);*/
	
//	fprintf(stdout, "%c \n", get_game_status(1213));

/*	fprintf(stdout, "player in game: %d \n", player_is_in_game("Jan", 1213));*/

	print_board(board);
/*	if((fd = open("./testTXT", O_RDWR)) > 0){*/
/*		while(read_line(fd,buffer,1024) > 0)*/
/*			fprintf(stdout,"# %s \n",buffer);*/
/*		lseek(fd,0,SEEK_SET);*/
/*		bulk_write(fd, "nowa linia\n",11);*/
/*		lseek(fd,0,SEEK_SET);*/
/*		while(read_line(fd,buffer,1024) > 0)*/
/*			fprintf(stdout,"@ %s \n",buffer);*/
/*		close(fd);*/
/*	}*/
	//fprintf(stdout,"Position A4 = %d \n",get_position_from_chars('A','4'));
	exit (EXIT_SUCCESS);
}
