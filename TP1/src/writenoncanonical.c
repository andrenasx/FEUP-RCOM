/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "ll.h"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

int alarme=0, count=1;

void atende(){
	alarme=1;
	printf("alarme # %d\n", count++);
}

int main(int argc, char** argv)
{
	if ((argc < 2)) {
	  printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
	  exit(1);
	}

	int port = atoi(argv[1]);
	if (port!=0 && port!=1 && port!=10 && port!=11){
		printf("Port number must be one of {0 / 1 / 10 / 11}");
		exit(1);
	}

	int fd = llopen(port, TRANSMITTER);
	llclose(fd);

	return 0;
}