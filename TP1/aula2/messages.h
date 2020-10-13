#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "macros.h"

#define ATTEMPT_NUM 3

enum states {START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP};

int sendSET(int fd);
int sendUA(int fd);
int processSET(enum states *state, char* check, char byte);
int processUA(enum states *state, char* check, char byte);