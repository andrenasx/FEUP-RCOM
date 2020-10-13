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
#include "messages.h"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

int alarme=0, count=0;

void atende(){
	alarme=1;
	printf("alarme # %d\n", ++count);
}

int main(int argc, char** argv)
{
	int fd, res;
	struct termios oldtio,newtio;
	
	if ( (argc < 2) || 
  		 ((strcmp("/dev/ttyS10", argv[1])!=0) && 
  		  (strcmp("/dev/ttyS11", argv[1])!=0) )) {
	  printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
	  exit(1);
	}


  /*
	Open serial port device for reading and writing and not as controlling tty
	because we don't want to get killed if linenoise sends CTRL-C.
  */

	fd = open(argv[1], O_RDWR | O_NOCTTY );
	if (fd <0) {perror(argv[1]); exit(-1); }

	if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
		perror("tcgetattr");
		exit(-1);
	}

	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;

	/* set input mode (non-canonical, no echo,...) */
	newtio.c_lflag = 0;

	newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */

  /* 
	VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
	leitura do(s) pr�ximo(s) caracter(es)
  */

	tcflush(fd, TCIOFLUSH);

	if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
		perror("tcsetattr");
		exit(-1);
	}

	printf("New termios structure set\n");

	(void) signal(SIGALRM, atende);

	unsigned char byte;

	do {
		if ((res=sendSET(fd)) == -1)
			printf("Error sending SET");
		else
			printf("%d SET bytes written\n", res);

		alarm(3);
		alarme=0;
		
		enum states state = START;
		unsigned char check[2];
		while(state!=STOP && !alarme) {
			if (read(fd, &byte, 1) == -1)
				printf("Error reading UA byte");
			else 
				processUA(&state, check, &byte);
		}
		if (alarme) printf("Timed out! Retrying");
	} while(count<3 && alarme);
   

	if (tcsetattr(fd,TCSANOW,&oldtio) == -1) {
		perror("tcsetattr");
		exit(-1);
	}

	close(fd);
	return 0;
}