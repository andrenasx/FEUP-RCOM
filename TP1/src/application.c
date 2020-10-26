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

int readControlPacket(unsigned char control){
	int index = 0;
	int file_size = 0;
	char* file_name;

	unsigned char packet[MAX_DATA_SIZE];
	llread(applayer.serial_fd, packet);

    if(packet[index++] != control) {
        printf("Wrong control packet");
        return -1;
    }

	//FILE SIZE
	if(packet[index] == T_FILE_SIZE){
		index++;
		int size_length = packet[index];
		index++;

		for (int i = 0; i < size_length; i++){
			file_size += packet[index] << 8 * (size_length - 1 - i);
			index++;
		}

		// if (app.receivedFileSize != 0){
		// 	if(app.receivedFileSize == file_size){
		// 		printf("Start Control Packet and End Control Packet data does not match\n");
		// 		return -1;
		// 	}
		// }

		printf("File size: %d bytes\n", file_size);
	}

	if (file_size <= 0) {
		perror("File size error\n");
		return -1;
	}

	applayer.recFileSize = file_size;

	//FILE NAME
	if (packet[index] == T_FILE_NAME) {
		index++;
		int name_length = packet[index];
		index++;

		file_name = (char*) malloc(name_length + 1);
		for (int i = 0; i < name_length; i++) {
			file_name[i] = packet[index];
			index++;
		}

		file_name[name_length] = '\0';

		// if (app.receivedFilename != NULL){
		// 	if(app.receivedFilename == file_name){
		// 		printf("Start Control Packet and End Control Packet data does not match\n");
		// 		return -1;
		// 	}
		// }

		printf("File Name: %s\n\n", file_name);
	}

	applayer.recFileName = file_name;

	applayer.serial_fd = open(applayer.recFileName, O_WRONLY | O_CREAT | O_APPEND);

	return applayer.serial_fd;
}

int sendDataPacket(){
	char buf[MAX_DATA_SIZE];
	int numbytes = 0, sequenceNumber = 0;

	while((numbytes = read(applayer.sent_file_fd, &buf, MAX_DATA_SIZE)) != 0){
		unsigned char packet[MAX_DATA_SIZE + numbytes];

		//build data packet
		packet[0] = C_DATA;
		packet[1] = sequenceNumber % 255;
		packet[2] = numbytes / 256;
		packet[3] = numbytes % 256;
		memcpy(&packet[4], &buf, numbytes);
		
		if(llwrite(applayer.serial_fd, packet, numbytes + CONTROL_PACKET_SIZE) == -1){
        	printf("Error sending the data packet\n");
        	return -1;
    	}
    	else
        	printf("Sent Data Packet\n");
		
		sequenceNumber++;

	}
	
	return 0;
}

int readDataPacket(unsigned char *packet){
	int dataSize = 256 * packet[2] + packet[3];

	if(write(applayer.rec_file_fd, &packet[4], dataSize) == -1){
		printf("Error writting data packet to file\n");
		return -1;
	}
	else
		printf("Wrote data packet to file\n");
	
	return 0;
}