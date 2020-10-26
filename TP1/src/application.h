#pragma once
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ll.h"

#define C_DATA  0x01
#define C_START 0x02
#define C_END   0x03

#define T_FILE_SIZE 0x00
#define T_FILE_NAME 0x01

#define DATA_PACKET_SIZE 4
#define CONTROL_PACKET_SIZE 5

typedef struct {
    int serial_fd;
    int sent_file_fd;
    int rec_file_fd;
    char sentFileName[256];
	char recFileName[256];
	off_t sentFileSize;
	off_t recFileSize;
} applicationLayer;

int sendFile(int fd, char *file_name);
int sendControlPacket(unsigned char control_field);
int sendDataPacket();
int receiveFile(int fd, char *dest);
int readControlPacket(unsigned char *packet);
int readDataPacket(unsigned char *packet);
