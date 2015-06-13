#include "board.h"


void print_board_header(){
	int i = 0, margin;
	fprintf(stdout, "   ");
	while(i < BOARD_SIZE){
		margin = 1 + TABLE_MARGIN;
		while(margin-->0) fprintf(stdout,"%c", ' ');
		fprintf(stdout,"%c",'A'+i);
		margin = TABLE_MARGIN;
		while(margin-->0) fprintf(stdout,"%c", ' ');
		i++;
	}
	fprintf(stdout,"\n");
}

void print_board(const char board[BOARD_SIZE * BOARD_SIZE]){
	int row;
	int column;
	int margin;
	
	print_board_header();
	for(row = 0;row < BOARD_SIZE; row++){
		print_table_line();
		fprintf(stdout," %d ",row+1);
		for(column = 0; column < BOARD_SIZE; column++){
			fprintf(stdout,"%c", TABLE_VERTICAL);
			margin = TABLE_MARGIN;
			while(margin-->0) { fprintf(stdout, "%c", EMPTY_SPACE); }
			fprintf(stdout,"%c", board[board_position(column,row)]);
			margin = TABLE_MARGIN;
			while(margin-->0) { fprintf(stdout, "%c", EMPTY_SPACE); }
		}
		fprintf(stdout,"%c\n",TABLE_VERTICAL);
	}
	print_table_line();
}

void print_table_line(){
	int i;
	int margin;
	fprintf(stdout, "   ");
	for(i = 0; i < BOARD_SIZE; i++){
		fprintf(stdout, "%c", TABLE_CROSSING);
		margin = 2*TABLE_MARGIN+1;
		do { fprintf(stdout, "%c", TABLE_HORIZONTAL); } while(--margin > 0);
	}
	fprintf(stdout, "%c \n", TABLE_CROSSING);
}

int board_position(int column_index, int row_index){
	//fprintf(stderr, "%d,%d=>%d \n", column_index, row_index, column_index+row_index*BOARD_SIZE);
	return column_index+row_index*BOARD_SIZE;
}

position_t get_board_position(int row, char column){
/*	if(col > 'Z' || col < 'A' || row > '9' || row < '1')*/
/*		ERR("Wrong chars");*/
	return board_position((column-'A'),(row-1));
}

char get_row_char(position_t position){
	if (position >= (BOARD_SIZE*BOARD_SIZE))
		return -1;
	return (position / BOARD_SIZE)+'1';
}

char get_column_char(position_t position){
	if (position >= (BOARD_SIZE*BOARD_SIZE))
		return -1;
	return (position % BOARD_SIZE)+'A';
}


