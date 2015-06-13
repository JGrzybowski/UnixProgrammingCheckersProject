#define _GNU_SOURCE 
#ifndef UNIX_PROJECT_COMMONS
#define UNIX_PROJECT_COMMONS

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <signal.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdbool.h>
#include <sys/wait.h>
#include "codes.h"

#define ERR(source) (perror(source),\
		     fprintf(stderr,"#%d %s:%d\n",getpid(), __FILE__,__LINE__),\
		     exit(EXIT_FAILURE))
#define HERR(source) (fprintf(stderr,"%s(%d) at %s:%d\n",source,h_errno,__FILE__,__LINE__),\
		     exit(EXIT_FAILURE))

#define NICK_SIZE 64

#define BOARD_SIZE 6
#define ROWS_OF_PIECES 2

#define MSG_SIZE 512
#define DATE_SIZE 20
#define MSG_TXT_SIZE MSG_SIZE-DATE_SIZE-1
#define POSITION_SIZE 1

#define WRONG_GAME_ID_MESSAGE "Wrong gameID"
#define WRONG_CODE_MESSAGE "Wrong command code!"

typedef unsigned char position_t;
typedef int32_t gameID;

int sethandler( void (*f)(int), int sigNo);

#endif
