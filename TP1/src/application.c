#include "application.h"

applicationLayer applayer;

int sendFile(int fd, char *file_name){
	applayer.sent_file_fd = open(file_name, O_RDONLY);
	if(applayer.sent_file_fd < 0){
		printf("Error opening file\n");
		return -1;
	}

	struct stat info;
	if(fstat(applayer.sent_file_fd, &info) == -1){
		printf("Error in fstat\n");
		return -1;
	}

	applayer.serial_fd = fd;
	applayer.sentFileSize = info.st_size;
	applayer.sentFileName = file_name;

	// Send Start Control Packet
	if(sendControlPacket(C_START) == -1){
		printf("Error sending Start Control Packet\n");
		return -1;
	}

	// Send Data Packets
	if (sendDataPackets() == -1) {
		printf("Error sending Data Packets\n");
		return -1;
	}

	// Send End Control Packet
	if(sendControlPacket(C_END) == -1){
		printf("Error sending End Control Packet\n");
		return -1;
	}

	return 0;
}

int sendControlPacket(unsigned char control_field){
    int index = 0;
    int file_length = sizeof(applayer.sentFileSize);
    unsigned char packet[CONTROL_PACKET_SIZE + file_length + strlen(applayer.sentFileName)];

    packet[0] = control_field;
    
    //Insert file size
    packet[1] = T_FILE_SIZE; //type
    packet[2] = file_length; //length
	memcpy(&packet[3], &file_length, sizeof(file_length)); //file size

    //Insert file name
    packet[file_length+3] = T_FILE_NAME; //type
    packet[file_length+4] = strlen(applayer.sentFileName); //length
	memcpy(&packet[file_length+5], applayer.sentFileName, strlen(applayer.sentFileName)); //file name
    
    if(llwrite(applayer.serial_fd, packet, index) == -1){
        printf("Error llwrite control packet\n");
        return -1;
    }
    else
        printf("Sent Control Packet\n");

    return 0;
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


int readControlPacket(){
	int index = 0;
	int size_length;
	off_t file_size = 0;
	char* file_name;

	unsigned char packet[MAX_DATA_SIZE];
	llread(applayer.serial_fd, packet);

	//FILE SIZE
	if(packet[1] == T_FILE_SIZE){
		size_length = packet[2];

		for (int i=3, j=0; i < size_length+3; i++, j++){
			file_size += packet[i] << 8 * j;
		}

		printf("File size: %d bytes\n", file_size);
	}

	if (file_size <= 0) {
		perror("File size error\n");
		return -1;
	}

	applayer.recFileSize = file_size;

	index = size_length+3;

	//FILE NAME
	if (packet[index] == T_FILE_NAME) {
		index++;
		int name_length = packet[index];
		index++;

		file_name = malloc(sizeof(char) * (name_length + 1));
		for (int i = 0; i < name_length; i++) {
			file_name[i] = packet[index];
			index++;
		}
		file_name[name_length] = '\0';

		printf("File Name: %s\n", file_name);
	}

	applayer.recFileName = file_name;

	applayer.rec_file_fd = open(applayer.recFileName, O_RDWR | O_CREAT, 0777);

	return applayer.rec_file_fd;
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