#ifndef UNIX_PROJECT_CODES
#define UNIX_PROJECT_CODES

#define CODE_SIZE 1

//Game codes
#define BOARD_CODE 'b'
#define STATUS_CODE 's'
#define TURN_CODE 't'
#define LOG_CODE 'l'
#define READ_MSG_CODE 'r'
#define WRITE_MSG_CODE 'w'
#define MOVE_CODE 'm'
#define SURRENDER_CODE 'g'
#define EXIT_GAME_CODE 'x'
//Lobby codes
#define NEW_GAME_CODE 'N'
#define CONNECT_CODE 'C'
#define LIST_GAMES_CODE 'L'
#define LOG_IN_CODE 'n'
#define QUIT_CODE 'Q'

//Server response codes

	#define RESPONSE_STATUS_SIZE CODE_SIZE
	
	//Game statuses
	#define ACTIVE 1
	#define RESOLVED 2
	#define AWAITING 3
	#define GAME_STATUS_ERROR -2
	//Geme turns 
	#define YOUR_TURN 1
	#define OPPONENTS_TURN 2
	
	#define SUCCESS_RESPONSE_CODE 1
	#define END_OF_LIST_CODE 2
	
	#define IMPROPER_COMMAND_CODE -1 
	#define WRONG_GAME_ID_RESPONSE_CODE -2
	#define GAME_HASNT_STARTED -3
	#define GAME_ALREADY_FINISHED -4
	#define NOT_YOUR_TURN -5
	#define NO_YOUR_PIECE -6
	#define BEATING_OWN_PIECE -7
	
#endif
