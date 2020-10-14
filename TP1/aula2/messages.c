#include "messages.h"

int sendSET(int fd) {
    unsigned char set[5] = {FLAG, A_ER, C_SET, BCC(A_ER, C_SET), FLAG};
    return write(fd, set, SET_SIZE);
}

int sendUA(int fd){
    unsigned char set[5] = {FLAG, A_RE, C_UA, BCC(A_RE, C_UA), FLAG};
    return write(fd, set, UA_SIZE);
}

void processSET(enum states *state, unsigned char *check, unsigned char byte){
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
                check[0] = byte;
                printf("A:\t%#4.2x\n", byte);
            }
            else if (byte!= FLAG){
                *state = START;
            }
            break;
        case A_RCV:
            if (byte == C_SET) {
                *state = C_RCV;
                check[1] = byte;
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
            if (byte == BCC(check[0],check[1])){
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
    }
}

void processUA(enum states *state, unsigned char *check, unsigned char byte){
    switch (*state) {
        case START:
            if (byte == FLAG){
                *state = FLAG_RCV;
                printf("F:\t%#4.2x\n", byte);
            }
            break;
        case FLAG_RCV:
            if (byte == A_RE) {
                *state = A_RCV;
                check[0] = byte;
                printf("A:\t%#4.2x\n", byte);
            }
            else if (byte!= FLAG){
                *state = START;
            }
            break;
        case A_RCV:
            if (byte == C_UA) {
                *state = C_RCV;
                check[1] = byte;
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
            if (byte == BCC(check[0],check[1])){
                *state = BCC_OK;
                printf("BCC1:\t%#4.2x\n", byte);
            }
            else if (byte == FLAG){
                *state = FLAG_RCV;
            }
            else{
                *state = START;
            }
            // precisa de valores de A & C
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
    }
}