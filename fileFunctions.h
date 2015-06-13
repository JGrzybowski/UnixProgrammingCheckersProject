#ifndef UNIX_PROJECT_FILE_FUNCTIONS
#define UNIX_PROJECT_FILE_FUNCTIONS

#include "common.h"

#define READ_LOCK 	F_RDLCK
#define WRITE_LOCK 	F_WRLCK

#define UNLOCK_ERROR	-2
#define LOCK_ERROR 		-3
#define OPEN_ERROR		-4
#define CLOSE_ERROR		-5
#define FILE_DOES_NOT_EXIST -6

#define SERVER_FILE_PATH_SIZE 4096

ssize_t bulk_read(int fd, char *buf, size_t count);
ssize_t bulk_write(int fd, char *buf, size_t count);
ssize_t read_line(const int fd, char *buf, size_t count);
ssize_t write_line(int fd, const char *buf, size_t count);

int open_with_lock_c(const char *filepath, int flags, mode_t mode, struct flock *lock, int lock_type);
int open_with_lock(const char *filepath, int flags, struct flock *lock, int lock_type);
int unlock_with_close(int fd, struct flock *lock);

int write_lock_file(int fd, struct flock* fl);
int read_lock_file(int fd, struct flock* fl);
int unlock_file(int fd, struct flock* fl);
gameID read_game_id_line(int fd, int* outint);

#endif

#ifndef UNIX_PROJECT_COMMUNICATION
#define UNIX_PROJECT_COMMUNICATION

//COMMON
int make_socket(int domain, int type);

void send_buf(int fd, char* msg_buffer, ssize_t buffer_size);
void send_msg(int fd, char buffer[MSG_SIZE]);
void send_code(int fd, char code);
void send_game_id(int fd, gameID game_id);

ssize_t recv_buf(int fd, char* buf, size_t buffer_size);
ssize_t recv_msg(int fd, char msg_buf[MSG_SIZE]);
char recv_code(int fd);
gameID recv_game_id(int fd);

//SERVER
#define BACKLOG 3
int bind_tcp_socket(uint16_t port);
int add_new_client(int sfd);

//CLIENT
struct sockaddr_in make_address(char *address, uint16_t port);
int connect_socket(char *name, uint16_t port);

#endif
