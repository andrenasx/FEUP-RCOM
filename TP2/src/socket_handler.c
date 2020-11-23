#include "socket_handler.h"

int open_socket(const char* ip_address, const int port) {
    int	sockfd;
	struct sockaddr_in server_addr;
	
	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip_address);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port);		/*server TCP port must be network byte ordered */
    
	/*open an TCP socket*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    	perror("socket()");
        return -1;
    }

	/*connect to the server*/
    if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("connect()");
		return -1;
	}

    return sockfd;
}

int write_to_socket(const int sockfd, const char* buf, const size_t buf_size) {
    int bytes;

    if((bytes = write(sockfd, buf, strlen(buf))) <= 0){
        perror("write to socket\n");
        return -1;
    }

    printf("Bytes written to server: %d\nInfo: %s\n", bytes, buf);
    return 0;
}