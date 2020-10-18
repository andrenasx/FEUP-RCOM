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
	int fd, res;
	
	if ( (argc < 2) || 
  		 ((strcmp("/dev/ttyS10", argv[1])!=0) && 
  		  (strcmp("/dev/ttyS11", argv[1])!=0) )) {
	  printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
	  exit(1);
	}

	fd = llopen(argv[1], TRANSMITTER);
	llclose(fd);

	return 0;
}