#include "application.h"
#include <stdio.h>


int main(int argc, char** argv) {
	// Parse Args
    if(argc != 4){
        printf("Usage: ./main <receiver/transmitter> <destination/filename> <port={0,1,10,11}>\n");
	    exit(1);
    }

    int port = atoi(argv[3]);
	if (port!=0 && port!=1 && port!=10 && port!=11){
		printf("Port number must be one of {0, 1, 10, 11}\n");
		exit(1);
	}

	int flag;
	if (strcmp("receiver",argv[1])==0){
		flag = RECEIVER;
	}
	else if(strcmp("transmitter",argv[1])==0){
		flag = TRANSMITTER;
	}
	else  {
		printf("Flag must be either 'receiver' or 'transmitter'\n");
		exit(1);
	}

    int fd = llopen(port, flag);
	if(fd == -1){
		printf("llopen error\n");
		return -1;
	}

	// Init application
	if(flag==RECEIVER){
		receiveFile(fd, argv[2]);
	}
	else if(flag==TRANSMITTER){
		sendFile(fd, argv[2]);
	}
	else {
		printf("Flag error\n");
		exit(1);
	}

	if(llclose(fd) == -1){
		printf("llclose error\n");
		return -1;
	}

	return 0;
}