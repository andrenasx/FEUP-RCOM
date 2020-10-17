#include "ll.h"

linkLayer linklayer;

void setDataLinkLayer(char *port, int flag){
    strcpy(linklayer.port, port);
    linklayer.flag = flag;
    linklayer.baudRate = BAUDRATE;
    linklayer.numTransmissions = 0;
    linklayer.alarm = 0;
    linklayer.timeout = 1;
}

int llopen(char* port, int flag){
    int fd, res;
    
    setDataLinkLayer(port, flag);
    fd = openSerial();
    
    if(linklayer.flag == RECEIVER){ //RECEIVER
        unsigned char check[2];
	    unsigned char byte;
	    enum states state = START;
	
	    while (state != STOP) {       /* loop for input */
		if (read(fd,&byte,1) == -1)   /* returns after 1 char has been input */
			printf("Error reading SET byte\n");
		else
			processFrameSU(&state, byte);
	    }

	    printf("Recived SET\n");
	
        if ((res=sendUA(fd)) == -1)
            printf("Error sending UA\n");
        else
            printf("%d UA bytes written\n", res);
    }

    else if(linklayer.flag == TRANSMITTER){
        unsigned char byte;
        
        do {
            if ((res=sendSET(fd)) == -1)
                printf("Error sending SET\n");
            else
                printf("%d SET bytes written\n", res);

            alarm(3);
            linklayer.alarm=0;

            enum states state = START;
            unsigned char check[2];
            while(state!=STOP && !linklayer.alarm) {
                if (read(fd, &byte, 1) == -1)
                    printf("Error reading UA byte\n");
                else
				    processFrameSU(&state, byte);
            }
            if (linklayer.alarm) printf("Timed out! Retrying\n");
        } while(linklayer.numTransmissions < MAX_TRANSMISSIONS && linklayer.alarm);
    }

    return fd; 
}

int llclose(int fd){
    int res;
    if(linklayer.flag == RECEIVER){
        unsigned char check[2];
	    unsigned char byte;
	    enum states state = START;

        //read DISC frame
	    while (state != STOP) {       /* loop for input */
		if (read(fd,&byte,1) == -1)   /* returns after 1 char has been input */
			printf("Error reading SET byte\n");
		else
			processFrameSU(&state, byte);
	    }
        
        //send DISC frame
        if((res=sendDISC(fd)) == -1)
            printf("Error sending UA\n");
        else
            printf("%d UA bytes written\n", res);

        //read UA frame
        while (state != STOP) {       /* loop for input */
		if (read(fd,&byte,1) == -1)   /* returns after 1 char has been input */
			printf("Error reading SET byte\n");
		else
			processFrameSU(&state, byte);
	    }

	        printf("Recived SET\n");
	
	    if ((res=sendUA(fd)) == -1)
		    printf("Error sending UA\n");
	    else
		    printf("%d UA bytes written\n", res);

    }

    else if(linklayer.flag == TRANSMITTER){
        //send DISC fram
        //read DISC frame
        //send UA frame
    }

    return fd;
}

int openSerial(){
    /*
	Open serial port device for reading and writing and not as controlling tty
	because we don't want to get killed if linenoise sends CTRL-C.
    */
    int fd = open(linklayer.port, O_RDWR | O_NOCTTY);
	if (fd <0) {perror(linklayer.port); exit(-1); }

	if ( tcgetattr(fd,&linklayer.oldtio == -1) ){ /* save current port settings */
		perror("tcgetattr");
		exit(-1);
	}

	bzero(&linklayer.newtio, sizeof(linklayer.newtio));
	linklayer.newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	linklayer.newtio.c_iflag = IGNPAR;
	linklayer.newtio.c_oflag = 0;

	/* set input mode (non-canonical, no echo,...) */
	linklayer.newtio.c_lflag = 0;

	linklayer.newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
	linklayer.newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */

    /* 
	VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
	leitura do(s) pr�ximo(s) caracter(es)
    */

	tcflush(fd, TCIOFLUSH);

	if ( tcsetattr(fd,TCSANOW,&linklayer.newtio) == -1) {
		perror("tcsetattr");
		exit(-1);
	}

	printf("New termios structure set\n");
    return fd;

}

