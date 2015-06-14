#define _GNU_SOURCE
#include "fileFunctions.h"


//BASIC FILE OPERATIONS{
ssize_t bulk_read(int fd, char *buf, size_t count){
	int c;
	size_t len=0;
	do{
		c=TEMP_FAILURE_RETRY(read(fd,buf,count));
		if(c<0) return c;
		if(0==c) return len;
		buf+=c;
		len+=c;
		count-=c;
	}while(count>0);
	return len ;
}

ssize_t bulk_write(int fd, char *buf, size_t count){
	int c;
	size_t len=0;
	do{
		c=TEMP_FAILURE_RETRY(write(fd,buf,count));
		if(c<0) return c;
		buf+=c;
		len+=c;
		count-=c;
	}while(count>0);
	return len ;
}

ssize_t read_line(const int fd, char *buf, size_t count){
	char c;
	size_t i = 0;
	//fprintf(stderr, "read_line start fd %d\n",fd);
	for(i=0; i<count; i++){
		TEMP_FAILURE_RETRY(read(fd,&c,1));	
		if(c=='\n' || c=='\0'){
			buf[i]='\0';
			//fprintf(stderr, "read_line EOF EOL fd %d\n",fd);
			return i;		
		}
		buf[i]=c;
	}
	//fprintf(stderr, "read_line count fd %d\n",fd);
	return i;
}

ssize_t write_line(int fd, const char *buf, size_t count){
	char c;
	size_t i = count;
	do{
		c = *buf;
		if(c =='\0')
			c ='\n';
		if(c == '\n'){		
			TEMP_FAILURE_RETRY(write(fd,&c,sizeof(char)));	
			return i - count;
		}	
		TEMP_FAILURE_RETRY(write(fd,&c,sizeof(char)));	
		buf++;	
		count--;
	}while(count>1);
	TEMP_FAILURE_RETRY(write(fd,"\n",sizeof(char)));	
	return count;
}
//}

//FILE LOCKING{
int lock_file(int fd, struct flock *fl, short l_type){
	struct flock lock = {l_type, SEEK_SET,   0,      0,     0 };
	*fl = lock;
	//fl->l_pid = getpid();
	if (TEMP_FAILURE_RETRY(fcntl(fd, F_SETLKW, fl)) == -1) {
		fprintf(stderr,"!%d!\n ",fd);
		ERR("File locking (fcntl):");
	}
	return 0;
}
int write_lock_file(int fd, struct flock* fl){
	return lock_file(fd, fl, F_WRLCK);
}
int read_lock_file(int fd, struct flock* fl){
	return lock_file(fd, fl, F_RDLCK);
}
int unlock_file(int fd, struct flock* fl){
	fl->l_type = F_UNLCK; 
	if (fcntl(fd, F_SETLK, fl) == -1) 
		ERR("File write unlock:");
	return 0;
}

int open_with_lock_c(const char *filepath, int flags, mode_t mode, struct flock *lock, int lock_type){
	int fd = -1;

	fprintf(stderr,"#%d %s Opening file\n", getpid(), filepath);
	if((fd = TEMP_FAILURE_RETRY(open(filepath, flags, mode))) < 0) 
		return OPEN_ERROR;
	
	fprintf(stderr,"#%d (fd:%d)%s Locking\n", getpid(), fd, filepath);
	
	if(lock_type == READ_LOCK){
		if(read_lock_file(fd, lock) < 0) return LOCK_ERROR ;
	}else if(lock_type == WRITE_LOCK){
		if(write_lock_file(fd, lock) < 0) return LOCK_ERROR;
	}else
		return LOCK_ERROR;
	fprintf(stderr,"#%d %s File opened and locked.\n", getpid(),filepath);
	return fd;
}


int open_with_lock(const char *filepath, int flags, struct flock *lock, int lock_type){
	int fd = -1;
	
	fprintf(stderr,"#%d %s Opening file\n", getpid(), filepath);
	fd = TEMP_FAILURE_RETRY(open(filepath, flags)); 
	if(fd < 0) {
		ERR("OPEN with lock:");
		return OPEN_ERROR;
	}
	fprintf(stderr,"#%d (fd:%d)%s Locking\n", getpid(), fd, filepath);
	
	if(lock_type == READ_LOCK){
		if(read_lock_file(fd, lock) < 0) return LOCK_ERROR ;
	}else if(lock_type == WRITE_LOCK){
		if(write_lock_file(fd, lock) < 0) return LOCK_ERROR;
	}else
		return LOCK_ERROR;
	fprintf(stderr,"#%d %s File opened and locked.\n", getpid(),filepath);
	return fd;
}

int unlock_with_close(int fd, struct flock *lock){
	fprintf(stderr,"#%d (fd:%d)Unlocking and closing file.\n", getpid(), fd);
	if(unlock_file(fd, lock) <0) {
		fprintf(stderr,"#%d Error unlocking file. \n", getpid());
		return UNLOCK_ERROR;
	}
	if(TEMP_FAILURE_RETRY(close(fd))<0){
		fprintf(stderr,"#%d Error closing file. \n", getpid());
		return CLOSE_ERROR;
	}
	//fprintf(stderr,"#%d DONE.\n", getpid());
	return 0;
}
//}

//GAME ID READING AND PARSING{
gameID parse_out_game_id(char* buffer, size_t buffer_size, gameID* out){
	int matches = 0;	
	if((matches = (sscanf(buffer, " %d ", out))) < 1){
		if (matches == 0)
			return 0;
		else 
			return -1;
	}else 
		return *out;
}

gameID read_game_id_line(int fd, gameID* out){
	//TODO set buffer size
	ssize_t size;
	char buffer[150];	
	
	if((size = read_line(fd, buffer, 150)) < 0) 
		ERR("read_id read_line:");
	if(size == 0) 
		return 0;
	return parse_out_game_id(buffer, 150, out);
	//fprintf(stderr,"@read line: %s \n gameID: %d \n",buffer,*outint);
}
//}

//SENDING MESSAGES{
void send_buf(int fd, char* msg_buffer, ssize_t buffer_size){
	ssize_t size;	
	if((size = bulk_write(fd,msg_buffer,buffer_size)) < buffer_size)
		ERR("Sending msg:");
}
void send_msg(int fd, char buffer[MSG_SIZE]){
	send_buf(fd, buffer, sizeof(char[MSG_SIZE]));	
}
void send_code(int fd, char code){
	char status_msg[CODE_SIZE];
	status_msg[0] = code;
	send_buf(fd, status_msg, CODE_SIZE);
}
void send_game_id(int fd, gameID game_id){
	game_id = htonl(game_id);
	send_buf(fd, (char*)&game_id, sizeof(gameID));
}
//}

//RECIEVING MESSAGGES{
ssize_t recv_buf(int fd, char* buf, size_t buffer_size){
	ssize_t size;	
	if((size = bulk_read(fd, buf,buffer_size)) < 0)
		ERR("Sending msg:");
	return size;
}

ssize_t recv_msg(int fd, char msg_buf[MSG_SIZE]){
	return recv_buf(fd, msg_buf, MSG_SIZE);
}

char recv_code(int fd){
	char code[CODE_SIZE];	
	recv_buf(fd, code, CODE_SIZE);
	return code[0];	
}

gameID recv_game_id(int fd){
	gameID game_id;
	recv_buf(fd, (char*)&game_id,sizeof(gameID));
	game_id = ntohl(game_id);
	return game_id;
}

int make_socket(int domain, int type){
	int sock;
	sock = socket(domain,type,0);
	if(sock < 0) ERR("socket");
	return sock;
}
//}

//SERVER TCP COMMUNICATION{
int bind_tcp_socket(uint16_t port){
	struct sockaddr_in addr;
	int socketfd,t=1;
	socketfd = make_socket(PF_INET,SOCK_STREAM);
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR,&t, sizeof(t))) ERR("setsockopt");
	if(bind(socketfd,(struct sockaddr*) &addr,sizeof(addr)) < 0)  ERR("bind");
	if(listen(socketfd, BACKLOG) < 0) ERR("listen");
	return socketfd;
}
int add_new_client(int sfd){
	int nfd;
	if((nfd=TEMP_FAILURE_RETRY(accept(sfd,NULL,NULL)))<0) {
		if(EAGAIN==errno||EWOULDBLOCK==errno) return -1;
		ERR("accept");
	}
	return nfd;
}
//}

//CLIENT TCP COMMUNICATION{
struct sockaddr_in make_address(char *address, uint16_t port){
	struct sockaddr_in addr;
	struct hostent *hostinfo;
	addr.sin_family = AF_INET;
	addr.sin_port = htons (port);
	hostinfo = gethostbyname(address);
	if(hostinfo == NULL)HERR("gethostbyname");
	addr.sin_addr = *(struct in_addr*) hostinfo->h_addr;
	return addr;
}
int connect_socket(char *name, uint16_t port){
	struct sockaddr_in addr;
	int socketfd;
	socketfd = make_socket(PF_INET,SOCK_STREAM);
	addr=make_address(name,port);
	if(connect(socketfd,(struct sockaddr*) &addr,sizeof(struct sockaddr_in)) < 0){
		if(errno!=EINTR) ERR("connect");
		else { 
			fd_set wfds ;
			int status;
			socklen_t size = sizeof(int);
			FD_ZERO(&wfds);
			FD_SET(socketfd, &wfds);
			if(TEMP_FAILURE_RETRY(select(socketfd+1,NULL,&wfds,NULL,NULL))<0) ERR("select");
			if(getsockopt(socketfd,SOL_SOCKET,SO_ERROR,&status,&size)<0) ERR("getsockopt");
			if(0!=status) ERR("connect");
		}
	}
	return socketfd;
}
//}
