#include "ll.h"

struct termios oldtio, newtio;
linkLayer linklayer;

statistics initStatistics() {
    statistics stats;
    stats.numSentFramesI = 0;
    stats.numReceivedFramesI = 0;
    stats.numSentRR = 0;
    stats.numReceivedRR = 0;
    stats.numSentREJ = 0;
    stats.numReceivedREJ = 0;
    if(clock_gettime(CLOCK_MONOTONIC, &stats.start) < 0) {
		printf("Error in start clock_gettime\n");
		exit(-1);
    }

    return stats;
}

void initDataLinkLayer(int port, int flag){
    char porta[12];
    snprintf(porta, 12, "/dev/ttyS%d", port);
    strcpy(linklayer.port, porta);
    linklayer.flag = flag;
    linklayer.numTransmissions = 0;
    linklayer.alarm = 0;
    linklayer.timeout = TIMEOUT;
    linklayer.sequenceNumber = 0;
    linklayer.stats = initStatistics();
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
    printf("\n***Started Connection***\n\n");
    return fd;
}

int closeSerial(int fd){
    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd,TCSANOW,&oldtio) == -1) {
		perror("tcsetattr");
		exit(-1);
	}

    printf("\n***Closed Connection***\n\n");

    return close(fd);
}

int llopen(int port, int flag){
    int fd;
    
    initDataLinkLayer(port, flag);
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
        if(linklayer.numTransmissions >= MAX_TRANSMISSIONS){
            printf("Reached max retries\n");
            return -1;
        }
        linklayer.numTransmissions=0;
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

    printf("\n***Established Connection***\n\n");

    return fd; 
}

int llclose(int fd){
    if(linklayer.flag == RECEIVER){
		//read DISC frame
       if(readCommand(fd) == -1){
           printf("Error reading DISC frame\n");
           return -1;
       }
       else
       {
           printf("Received DISC\n");
       }      
       //send DISC frame
       if (sendDISC(fd) == -1){
            printf("Error sending DISC\n");
        }
        else {
            printf("Sent DISC\n");
        }
        //read UA
        if(readCommand(fd) == -1){
            printf("Error reading UA\n");
            return -1;
        }
		else
		{
			printf("Received UA\n");
		}
		       
    }

    else if(linklayer.flag == TRANSMITTER){
        do{
			//send DISC
			if (sendDISC(fd) == -1){
                printf("Error sending DISC\n");
        	}	
        	else {
                printf("Sent DISC\n");
        	}

			setAlarm();

			//read DISC
			if(readResponse(fd) == -1){
           		printf("Error reading DISC frame\n");
       		}
       		else{
           		printf("Received DISC\n");
       		} 
		}while (linklayer.numTransmissions < MAX_TRANSMISSIONS && linklayer.alarm);

		unsetAlarm();
        if(linklayer.numTransmissions >= MAX_TRANSMISSIONS){
            printf("Reached max retries\n");
            return -1;
        }
        linklayer.numTransmissions=0;

        if (sendUA(fd) == -1){
            printf("Error sending UA\n");
        }
        else {
            printf("Sent UA\n");
        }
    }

    printf("\n***Terminated Connection***\n\n");

    if(clock_gettime(CLOCK_MONOTONIC, &linklayer.stats.end) < 0) {
		printf("Error in end clock_gettime\n");
		exit(-1);
    }

    sleep(1);
	return closeSerial(fd);
}

int llwrite(int fd, unsigned char* buffer, int length){
    do{
        // Send frame
        if (writeStuffedFrame(fd, buffer, length)) printf("Sent Frame\n");
        setAlarm();
        // Read receiver ACK
        if(readAck(fd) == -1){
            unsetAlarm();
            linklayer.alarm = 1;
            continue;
        }
    }while(linklayer.numTransmissions < MAX_TRANSMISSIONS && linklayer.alarm);

    unsetAlarm();
    if(linklayer.numTransmissions >= MAX_TRANSMISSIONS){
        printf("Reached max retries\n");
        return -1;
    }
    linklayer.numTransmissions=0;
    
    return length;
}

int llread(int fd, unsigned char *buffer) {
    int received = 0;
    int frame_length = 0;
    int dframe_length = 0;
    int packet_length = 0;
    unsigned char frame[2*MAX_SIZE+6];
    unsigned char dframe[MAX_SIZE];
    unsigned char control;

    while(!received){
        if((frame_length = readFrameI(fd, frame))){
            printf("Received Frame\n");	
            control = frame[2];

            // Destuff received frame
            dframe_length = destuffFrame(frame, frame_length, dframe);

            // Calculate BCC2 after destuffing
            unsigned char bcc2 = dframe[4];
            for(int i=5; i<dframe_length-7+5; i++){
                bcc2 ^= dframe[i];
            }

            // Verify if calculted BCC2 matches with received BCC2
            if(bcc2!=dframe[dframe_length-2]){
                printf("BCC2 Error\n");
                
                // If not, send REJ
                if(control == C_I0){
                    if(sendREJ0(fd) == -1){
                        printf("Error sending REJ0\n");
                    }
                    else {
                        linklayer.stats.numSentREJ++;
                        printf("Sent REJ0\n");
                    }
                    
                }
                else if(control == C_I1){
                    if(sendREJ1(fd) == -1){
                       printf("Error sending REJ1\n");
                    }
                    else {
                        linklayer.stats.numSentREJ++;
                        printf("Sent REJ1\n");
                    }
                }

                return -1;
            }

            else {
                // Fills arg buffer with Data from destuffed frame
                for(int i=4; i< dframe_length - 2; i++){
                    buffer[packet_length++] = dframe[i];
                }

                // Send correct RR
                if(control == C_I0) {
                    if(sendRR1(fd) == -1){
                        printf("Error sending RR1\n");
                    }
                    else {
                        linklayer.stats.numSentRR++;
                        printf("Sent RR1\n");
                    }
                }
                else if(control == C_I1) {
                    if(sendRR0(fd) == -1){
                        printf("Error sending RR0\n");
                    }
                    else {
                        linklayer.stats.numSentRR++;
                        printf("Sent RR0\n");
                    }
                }

                received = 1;
            }
            
        }
        
    }

    // Change sequence number (0 / 1)
    linklayer.sequenceNumber^=0x01;

    return packet_length;
}

void displayStats() {
	printf("\n\n***Statistics:***\n\n");
	printf("Total execution time: %f seconds\n",  (double)(linklayer.stats.end.tv_sec - linklayer.stats.start.tv_sec) + ((double)(linklayer.stats.end.tv_nsec - linklayer.stats.start.tv_nsec)/1000000000.0));
	printf("Number of sent Frames I: %d\n", linklayer.stats.numSentFramesI);
	printf("Number of received Frames I: %d\n", linklayer.stats.numReceivedFramesI);
	printf("Number of timeouts: %d\n", linklayer.stats.numTimeouts);
	printf("Number of sent Frames RR: %d\n", linklayer.stats.numSentRR);
	printf("Number of received Frames RR: %d\n", linklayer.stats.numReceivedRR);
	printf("Number of sent Frames REJ: %d\n", linklayer.stats.numSentREJ);
	printf("Number of received Frames REJ: %d\n", linklayer.stats.numReceivedREJ);
	printf("\n");
}