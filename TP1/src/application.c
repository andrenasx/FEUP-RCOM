#include "application.h"

applicationLayer applayer;

int sendControlPacket(unsigned char control_field){
    int index = 0;
    int file_length = sizeof(applayer.sentFileSize);
    unsigned char packet[CONTROL_PACKET_SIZE + file_length + strlen(applayer.sentFileName)];

    packet[index++] = control_field;
    
    //Insert file size
    packet[index++] = T_FILE_SIZE; //type
    packet[index++] = file_length; //length

    for(int i = 0; i < file_length; i++){
        packet[index++] = (applayer.sentFileSize >> 8*(file_length - 1 - i)) & 0xFF; //value ??
    }

    //Insert file name
    packet[index++] = T_FILE_NAME; //type
    packet[index++] = strlen(applayer.sentFileName); //length

    for(int i = 0; i < applayer.sentFileName; i++){
        packet[index++] = applayer.sentFileName[i]; //value
    }
    if(llwrite(applayer.serial_fd, packet, index) == -1){
        printf("Error sending the control packet\n");
        return -1;
    }
    else
        printf("Sent Control Packet");

    return 0;
}