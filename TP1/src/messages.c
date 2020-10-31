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

int sendRR0(int fd){
    unsigned char rr0[5] = {FLAG, A_ER, C_RR0, BCC(A_ER, C_RR0), FLAG};
    return write(fd, rr0, 5);
}

int sendRR1(int fd){
    unsigned char rr1[5] = {FLAG, A_ER, C_RR1, BCC(A_ER, C_RR1), FLAG};
    return write(fd, rr1, 5);
}

int sendREJ0(int fd){
    unsigned char rej0[5] = {FLAG, A_ER, C_REJ0, BCC(A_ER, C_REJ0), FLAG};
    return write(fd, rej0, 5);
}

int sendREJ1(int fd){
    unsigned char rej1[5] = {FLAG, A_ER, C_REJ1, BCC(A_ER, C_REJ1), FLAG};
    return write(fd, rej1, 5);
}

unsigned char processFrameSU(enum states *state, unsigned char byte){
    static unsigned char c = 0;
    switch (*state) {
        case START:
            if (byte == FLAG) {
                *state = FLAG_RCV;
            }
            break;

        case FLAG_RCV:
            if (byte == A_ER) {
                *state = A_RCV;
            }
            else if (byte!= FLAG){
                *state = START;
            }
            break;

        case A_RCV:
            if (VERIFY_C(byte)) {
                *state = C_RCV;
                c = byte;
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
    return c;
}

void processFrameI(enum states *state, unsigned char byte){
    static unsigned char c = 0;
    switch (*state) {
        case START:
            if (byte == FLAG) {
                *state = FLAG_RCV;
            }
            break;

        case FLAG_RCV:
            if (byte == A_ER) {
                *state = A_RCV;
            }
            else if (byte!= FLAG){
                *state = START;
            }
            break;

        case A_RCV:
            if ((byte==C_I0 && linklayer.sequenceNumber==0) || (byte==C_I1 && linklayer.sequenceNumber==1)) {
                *state = C_RCV;
                c = byte;
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
            }
            else if (byte == FLAG){
                *state = FLAG_RCV;
            }
            else{
                *state = START;
            }
            break;

        case BCC_OK:
            if (byte != FLAG){
                *state = DATA;
            }
            else{
                *state = START;
            }
            break;
        case DATA:
            if (byte == FLAG){
                *state = STOP;
            }
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

int readFrameI(int fd, unsigned char *frame){
    unsigned char byte;
    int length = 0;
    enum states state = START;
	
    while (state != STOP) {       /* loop for input */
        if (read(fd,&byte,1) == -1){ /* returns after 1 char has been input */
            printf("Error reading I byte\n");
        }
        else{
            processFrameI(&state, byte);

            if(state == FLAG_RCV && length !=0) length = 0;

            frame[length++] = byte;
        }
    }

    return length;
}

int readAck(int fd){
    unsigned char byte;
    enum states state = START;
    unsigned char control_field;

    while(state!=STOP && !linklayer.alarm) {
        if (read(fd, &byte, 1) == -1){
            printf("Error reading ACK byte\n");
        }
        else{
            control_field = processFrameSU(&state, byte);
        }
    }

    if(linklayer.sequenceNumber == 0 && control_field == C_RR1){
        printf("Received RR1\n");
        linklayer.sequenceNumber = 1;
        return 0;
    }

    else if(linklayer.sequenceNumber == 1 && control_field == C_RR0){
        printf("Received RR0\n");
        linklayer.sequenceNumber = 0;
        return 0;
    }

    else if(control_field == C_REJ0){
        printf("Received REJ0\n");
        return -1;
    }

    else if(control_field == C_REJ1){
        printf("Received REJ1\n");
        return -1;
    }

    return -1;
}

int writeStuffedFrame(int fd, unsigned char *buffer, int length) {
    unsigned char frame[2 * length + 6];

    // Frame Header
    frame[0] = FLAG;
    frame[1] = A_ER;
    frame[2] = (linklayer.sequenceNumber==0 ? C_I0 : C_I1);
    frame[3] = BCC(A_ER, frame[2]);

    // BCC2 before byte stuffing
    unsigned char bcc2 = buffer[0];
    for(int i=1; i<length; i++){
        bcc2 ^= buffer[i];
    }

    // Process data
    int dataIndex=0, frameIndex=4;
    unsigned char bufferAux;

    while(dataIndex < length) {
        bufferAux = buffer[dataIndex++];

        // Byte Stuffing
        if(bufferAux == FLAG || bufferAux == ESCAPE) {
            frame[frameIndex++] = ESCAPE;
            frame[frameIndex++] = bufferAux ^ STUFFING;
        }
        else {
            frame[frameIndex++] = bufferAux;
        }
    }

    // Frame Footer
    if(bcc2 == FLAG || bcc2 == ESCAPE) {
        frame[frameIndex++] = ESCAPE;
        frame[frameIndex++] = bcc2 ^ STUFFING;
    }
    else {
        frame[frameIndex++] = bcc2;
    }

    frame[frameIndex++] = FLAG;
    write(fd, frame, frameIndex);

    return frameIndex;
}

int destuffFrame(unsigned char* frame, int length, unsigned char* destuffed_frame){
    // Frame Header
    destuffed_frame[0] = frame[0]; // FLAG
    destuffed_frame[1] = frame[1]; // A
    destuffed_frame[2] = frame[2]; // C
    destuffed_frame[3] = frame[3]; // BCC1

    // Process data
    int dframeIndex=4, frameIndex;

    for(frameIndex=4; frameIndex < length-1; frameIndex++){
        if(frame[frameIndex] == ESCAPE){
            frameIndex++;
            if(frame[frameIndex] == (FLAG ^ STUFFING))
                destuffed_frame[dframeIndex] = FLAG;
            else if(frame[frameIndex] == (ESCAPE ^ STUFFING))
                destuffed_frame[dframeIndex] = ESCAPE;
        }
        else{
            destuffed_frame[dframeIndex] = frame[frameIndex];
        }
        dframeIndex++;
    }

    // Frame Footer
    destuffed_frame[dframeIndex++] = frame[frameIndex++]; // FLAG

    return dframeIndex;
}