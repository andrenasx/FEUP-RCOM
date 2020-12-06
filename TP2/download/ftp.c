#include "ftp.h"

static int socketfd;
static int datafd;
static char command[256];

int ftp_init(char *ip, int port){
    int response_code;

    if((socketfd = init_connection(ip, 21))<0){
        fprintf(stderr, "Error initializing connection!\n");
        return -1;
    }
    
    response_code = read_from_socket(socketfd);
    if(response_code != SERVICE_READY_CODE){
        fprintf(stderr, "Error, not 220\n");
        close(socketfd);
        return -1;
    }

    return 0;
}

int ftp_quit(){
    sprintf(command, "QUIT\r\n");
    if(write_to_socket(socketfd, command) != 0){
        close(socketfd);
        return -1;
    }
    
    close(socketfd);
    return 0;
}

int ftp_login(char* user, char* pass){
    int response_code;

    //Send user
    sprintf(command, "USER %s\r\n", user);
    if(write_to_socket(socketfd, command) != 0){
        close(socketfd);
        return -1;
    }

    response_code = read_from_socket(socketfd);
    if(response_code == USER_LOGGED_IN_CODE){
        return 0;
    }
    else if(response_code != NEED_PASSWORD_CODE){
        fprintf(stderr, "Error, not 331\n");
        close(socketfd);
        return -1;
    }

    //Send pass
    sprintf(command, "PASS %s\r\n", pass);
    if(write_to_socket(socketfd, command) != 0){
        close(socketfd);
        return -1;
    }

    response_code = read_from_socket(socketfd);
    if(response_code != USER_LOGGED_IN_CODE){
        fprintf(stderr, "Error, not 230\n");
        close(socketfd);
        return -1;
    }

    return 0;
}

int ftp_binarymode(){
    int response_code;

    sprintf(command, "TYPE I\r\n");
    if(write_to_socket(socketfd, command) != 0){
        close(socketfd);
        return -1;
    }

    response_code = read_from_socket(socketfd);
    if(response_code != COMMAND_OKAY_CODE){
        fprintf(stderr, "Error, not 200\n");
        close(socketfd);
        return -1;
    }

    return 0;
}

int ftp_pasvmode(char *ip, int *port){
    int response_code;

    sprintf(command, "PASV\r\n");
    if(write_to_socket(socketfd, command) != 0){
        close(socketfd);
        return -1;
    }

    response_code = read_passivemode(socketfd, ip, port);
    if(response_code != ENTER_PASSIVE_MODE_CODE){
        fprintf(stderr, "Error, not 227\n");
        close(socketfd);
        return -1;
    }

    return 0;
}

int ftp_retr(char *file){
    int response_code;

    sprintf(command, "RETR %s\r\n", file);
    if(write_to_socket(socketfd, command) != 0){
        close(socketfd);
        return -1;
    }

    response_code = read_from_socket(socketfd);
    if(response_code != RETR_FILE_OKAY_CODE){
        fprintf(stderr, "Error, not 150\n");
        close(socketfd);
        return -1;
    }

    return 0;
}

int ftp_download(char *path, char *filename){
    char ip[32];
    int port;

    //Enter passive move
    if(ftp_pasvmode(ip, &port) != 0){
        fprintf(stderr, "Error entering passive mode\n");
        close(socketfd);
        return -1;
    }
    
    printf("%s %d\n", ip, port);

    //Open new socket for file download
    if((datafd = init_connection(ip, port))<0){
        fprintf(stderr, "Error initializing data connection!\n");
        close(socketfd);
        return -1;
    }

    //Retrieve file
    if(ftp_retr(path) != 0){
        fprintf(stderr, "Error file status!\n");
        close(socketfd);
        return -1;
    }

    //Download file
    if(transferFile(datafd, filename) != 0){
        fprintf(stderr, "Error transfering file!\n");
        close(socketfd);
        return -1;
    }

    //Close data socket
    close(datafd);
    int response_code = read_from_socket(socketfd);
    if(response_code != RETR_SUCCESS_CODE){
        fprintf(stderr, "Error, not 226\nError retrieving file\n");
        close(socketfd);
        return -1;
    }

    return 0;
}

int transferFile(int datafd, char *filename) {
    int filefd;

    if ((filefd = open(filename, O_WRONLY | O_CREAT, 0777)) < 0) {
        fprintf(stderr, "Error opening data file!\n");
        return -1;
    }

    char buf[1024];
    int numBytesRead;

    while((numBytesRead = read(datafd, buf, 1024)) > 0) {
        if (write(filefd, buf, numBytesRead) < 0) {
            fprintf(stderr, "Error writing data to file!\n");
            return -1;
        }
    }

    if (close(filefd) < 0) {
        fprintf(stderr, "Error closing file!\n");
        return -1;
    }

    return 0;
}