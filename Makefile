CC=gcc
C_FLAGS=-Wall -g
L_FLAGS=-lm -lrt

COMMON_FILES= common.o fileFunctions.o  board.o
CLIENT_FILES= client.c gameCommands.o lobbyCommands.o clientGameRules.o 
SERVER_FILES= server.c playerServices.o gameServices.o lobbyServices.o filePaths.o

SERVER_NAME = bin/checkers_server 
CLIENT_NAME = bin/checkers_client


all: server client
install: server client init
init: 
	mkdir games
	mkdir players
	touch .awaiting_game
	touch .awaiting_player

server: ${SERVER_FILES} ${COMMON_FILES}
	${CC} -o ${SERVER_NAME} ${SERVER_FILES} ${COMMON_FILES} ${C_FLAGS} ${L_FLAGS}
client: ${CLIENT_FILES} ${COMMON_FILES}
	${CC} -o ${CLIENT_NAME} ${CLIENT_FILES} ${COMMON_FILES} ${C_FLAGS} ${L_FLAGS}

common.o: common.h common.c
	${CC} -o common.o -c common.c ${C_FLAGS} ${L_FLAGS}
fileFunctions.o: common.o fileFunctions.h fileFunctions.c
	${CC} -o fileFunctions.o -c fileFunctions.c ${C_FLAGS} ${L_FLAGS}
board.o: common.o fileFunctions.o board.c board.h
	${CC} -o board.o -c board.c fileFunctions.o common.o ${C_FLAGS} ${L_FLAGS}

gameServices.o:  fileFunctions.o gameServices.c gameServices.h 
	${CC} -o gameServices.o -c gameServices.c fileFunctions.o  ${C_FLAGS} ${L_FLAGS}
playerServices.o: fileFunctions.o playerServices.c playerServices.h
	${CC} -o playerServices.o -c playerServices.c fileFunctions.o ${C_FLAGS} ${L_FLAGS}
lobbyServices.o: fileFunctions.o lobbyServices.c gameServices.o lobbyServices.h
	${CC} -o lobbyServices.o -c lobbyServices.c fileFunctions.o gameServices.o ${C_FLAGS} ${L_FLAGS}
filePaths.o: common.o
	${CC} -o filePaths.o -c filePaths.c common.o ${C_FLAGS} ${L_FLAGS}
	
lobbyCommands.o: lobbyCommands.h lobbyCommands.c ${COMMON_FILES}
	${CC} -o lobbyCommands.o -c lobbyCommands.c common.o  fileFunctions.o board.o ${C_FLAGS} ${L_FLAGS}
gameCommands.o : gameCommands.h gameCommands.c common.o fileFunctions.o board.o clientGameRules.o
	${CC} -o gameCommands.o -c gameCommands.c common.o  fileFunctions.o board.o ${C_FLAGS} ${L_FLAGS}
clientGameRules.o : clientGameRules.c clientGameRules.h
	${CC} -o clientGameRules.o -c clientGameRules.c ${C_FLAGS} ${L_FLAGS}

test: gameServices.o playerServices.o common.o  test.c board.o
	${CC} -o test test.c ${COMMON_FILES} gameServices.o playerServices.o
	./test

.PHONY: clean 
clean: 
	rm *.o ${SERVER_NAME} ${CLIENT_NAME}


