#include "ll.h"

struct termios oldtio, newtio;
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
    int fd;
    
    setDataLinkLayer(port, flag);
    fd = openSerial();

    if(linklayer.flag == TRANSMITTER){
        do {
            if (sendSET(fd) == -1){
                printf("Error sending SET\n");
            }
            else {
                printf("Sent SET\n");
            }

            setAlarm();

            if(readResponse(fd) == -1){
                printf("Error receiving UA\n");
            }
            else {
                printf("Received UA\n");
            }

        } while (linklayer.numTransmissions < MAX_TRANSMISSIONS && linklayer.alarm);


        unsetAlarm();
    }
    
    else if(linklayer.flag == RECEIVER){
	    if (readCommand(fd) == -1 ){
            printf("Error receiving SET\n");
            return -1;
        }
        else {
            printf("Received SET\n");
        }
        
	
        if (sendUA(fd) == -1){
            printf("Error sending UA\n");
        }
        else {
            printf("Sent UA\n");
        }

    }

    return fd; 
}

int llclose(int fd){
    // int res;
    // if(linklayer.flag == RECEIVER){
    //     unsigned char check[2];
	//     unsigned char byte;
	//     enum states state = START;

    //     //read DISC frame
	//     while (state != STOP) {       /* loop for input */
	// 	if (read(fd,&byte,1) == -1)   /* returns after 1 char has been input */
	// 		printf("Error reading SET byte\n");
	// 	else
	// 		processFrameSU(&state, byte);
	//     }
        
    //     //send DISC frame
    //     if((res=sendDISC(fd)) == -1)
    //         printf("Error sending UA\n");
    //     else
    //         printf("%d UA bytes written\n", res);

    //     //read UA frame
    //     while (state != STOP) {       /* loop for input */
	// 	if (read(fd,&byte,1) == -1)   /* returns after 1 char has been input */
	// 		printf("Error reading SET byte\n");
	// 	else
	// 		processFrameSU(&state, byte);
	//     }

	//         printf("Recived SET\n");
	
	//     if ((res=sendUA(fd)) == -1)
	// 	    printf("Error sending UA\n");
	//     else
	// 	    printf("%d UA bytes written\n", res);

    // }

    // else if(linklayer.flag == TRANSMITTER){
    //     //send DISC fram
    //     //read DISC frame
    //     //send UA frame
    // }

    if (tcsetattr(fd,TCSANOW,&oldtio) == -1) {
		perror("tcsetattr");
		exit(-1);
	}

	close(fd);

    return 0;
}

int openSerial(){
    /*
	Open serial port device for reading and writing and not as controlling tty
	because we don't want to get killed if linenoise sends CTRL-C.
    */
    int fd = open(linklayer.port, O_RDWR | O_NOCTTY );
	if (fd <0) {perror(linklayer.port); exit(-1); }

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

	newtio.c_cc[VTIME] = 0;   /* inter-character timer unused */
	newtio.c_cc[VMIN] = 1;   /* blocking read until 1 chars received */

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
    return fd;
}