/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ll.h"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int DONE=FALSE;

int main(int argc, char** argv)
{
	if ( (argc < 2) || 
  		((strcmp("/dev/ttyS10", argv[1])!=0) && 
  		  (strcmp("/dev/ttyS11", argv[1])!=0) )) {
	  printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
	  exit(1);
	}

	int fd = llopen(argv[1], RECEIVER);
	llclose(fd);

	return 0;
}