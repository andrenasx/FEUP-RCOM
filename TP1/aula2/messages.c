#include "messages.h"

int sendSET(int fd) {
    unsigned char set[5] = {FLAG, A_ER, C_SET, BCC(A_ER, C_SET), FLAG);
    return write(fd, set, SET_SIZE);
}

int sendUA(int fd){
    unsigned char set[5] = {FLAG, A_RE, C_UA, BCC(A_RE, C_UA), FLAG);
    return write(fd, set, SET_SIZE);
}

int processMSG(enum states state, char* check, char byte){
    // state machine here
}