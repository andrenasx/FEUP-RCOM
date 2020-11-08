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
#include "messages.h"
#include "alarm.h"

#define BAUDRATE B19200
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define TRANSMITTER 0
#define RECEIVER 1

#define TIMEOUT 5
#define MAX_TRANSMISSIONS 3

typedef struct {
    unsigned int numSentFramesI;
    unsigned int numReceivedFramesI;
    unsigned int numTimeouts;
    unsigned int numSentRR;
    unsigned int numReceivedRR;
    unsigned int numSentREJ;
    unsigned int numReceivedREJ;
    struct timespec start, end;
} statistics;

typedef struct {
    char port[20]; // /dev/ttySx
    int flag; //TRANSMITTER/RECEIVER
    unsigned int sequenceNumber; //trama sequence
    unsigned int timeout;
    unsigned int numTransmissions; //atempt number in case of failure
    unsigned int alarm;
    statistics stats;
} linkLayer;

extern linkLayer linklayer;

statistics initStatistics();
void initDataLinkLayer(int port, int flag);

//ll functions
int llopen(int port, int flag);
int llclose(int fd);
int llwrite(int fd, unsigned char* buffer, int length);
int llread(int fd, unsigned char *buffer);

int openSerial();
int closeSerial(int fd);

void displayStats();