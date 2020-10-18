#include "messages.h"

int sendSET(int fd) {
    unsigned char set[5] = {FLAG, A_ER, C_SET, BCC(A_ER, C_SET), FLAG};
    return write(fd, set, SET_SIZE);
}

int sendUA(int fd){
    unsigned char ua[5] = {FLAG, A_ER, C_UA, BCC(A_ER, C_UA), FLAG};
    return write(fd, ua, UA_SIZE);
}

int sendDISC(int fd){
    unsigned char disc[5] = {FLAG, A_ER, C_DISC, BCC(A_ER, C_DISC), FLAG};
    return write(fd, disc, 5);
}

void processFrameSU(enum states *state, unsigned char byte){
    static unsigned char c = 0;
    switch (*state) {
        case START:
            if (byte == FLAG) {
                *state = FLAG_RCV;
                printf("F:\t%#4.2x\n", byte);
            }
            break;

        case FLAG_RCV:
            if (byte == A_ER) {
                *state = A_RCV;
                printf("A:\t%#4.2x\n", byte);
            }
            else if (byte!= FLAG){
                *state = START;
            }
            break;

        case A_RCV:
            if (VERIFY_C(byte)) {
                *state = C_RCV;
                c = byte;
                printf("C:\t%#4.2x\n", byte);
            }
            else if (byte == FLAG){
                *state = FLAG_RCV;
            }
            else{
                *state = START;
            }
            break;

        case C_RCV:
            if (byte == BCC(A_ER,c)){
                *state = BCC_OK;
                printf("BCC1:\t%#4.2x\n", byte);
            }
            else if (byte == FLAG){
                *state = FLAG_RCV;
            }
            else{
                *state = START;
            }
            break;

        case BCC_OK:
            if (byte == FLAG){
                *state = STOP;
                printf("F:\t%#4.2x\n", byte);
            }
            else{
                *state = START;
            }
            break;

        case STOP:
            break;
        
        default:
            break;
    }
}

int readCommand(int fd){
    unsigned char byte;
    enum states state = START;
	
    while (state != STOP) {       /* loop for input */
        if (read(fd,&byte,1) == -1){ /* returns after 1 char has been input */
            printf("Error reading SET byte\n");
        }
        else{
            processFrameSU(&state, byte);
        }
    }

    return 0;
}

int readResponse(int fd){
    unsigned char byte;
    enum states state = START;

    while(state!=STOP && !linklayer.alarm) {
        if (read(fd, &byte, 1) == -1){
            printf("Error reading UA byte\n");
        }
        else{
            processFrameSU(&state, byte);
        }
    }

    if(linklayer.alarm) return -1;

    return 0;
}