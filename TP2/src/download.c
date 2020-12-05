#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>

#include "rfc959.h"
#include "url_parser.h"
#include "socket.h"

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

int main(int argc, char* argv[]) {
    if(argc!=2) {
        fprintf(stderr, "usage: download ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(1);
    }

    url_args url;

    if(parseURL(argv[1], &url)!=0){
        fprintf(stderr, "Error parsing url!\n");
        exit(1);
    }

    printURL(url);
    char filename[64];
    strcpy(filename, url.filename);

    int socketfd;
    char command[256];
    int response_code;

    //init
    if((socketfd = init_connection(url.ip, 21))<0){
        fprintf(stderr, "Error initializing connection!\n");
        exit(1);
    }
    
    read_from_socket(socketfd, &response_code);
    if(response_code != SERVICE_READY_CODE){
        fprintf(stderr, "Error, service not ready\n");
        exit(1);
    }

    //login
    sprintf(command, "user %s\r\n", url.user);
    write_to_socket(socketfd, command);
    read_from_socket(socketfd, &response_code);
    if(response_code == NEED_PASSWORD_CODE){
        sprintf(command, "pass %s\r\n", url.pass);
        write_to_socket(socketfd, command);
        read_from_socket(socketfd, &response_code);
        if(response_code != USER_LOGGED_IN_CODE){
            fprintf(stderr, "Error logging in\n");
            exit(1);
        }
    }

    //passive move
    char *ip; ip = malloc(17);
    int port, datafd;

    sprintf(command, "pasv\r\n");
    write_to_socket(socketfd, command);
    read_passivemode(socketfd, &response_code, &ip, &port);
    if(response_code != ENTER_PASSIVE_MODE_CODE){
        fprintf(stderr, "Error entering passive mode\n");
        exit(1);
    }

    printf("\n%s %d\n", ip, port);

    if((datafd = init_connection(ip, port))<0){
        fprintf(stderr, "Error initializing data connection!\n");
        exit(1);
    }

    //retr file
    sprintf(command, "retr %s\r\n", url.path);
    write_to_socket(socketfd, command);
    read_from_socket(socketfd, &response_code);
    if(response_code != RETR_FILE_OKAY_CODE){
        fprintf(stderr, "Error retrieving file\n");
        exit(1);
    }

    if(transferFile(datafd, filename) != 0){
        fprintf(stderr, "Error transfering file!\n");
        exit(1);
    }

    //close fds
    if (close(socketfd) < 0) {
        fprintf(stderr, "Error closing socket!\n");
        exit(1);
    }

    if (close(datafd) < 0) {
        fprintf(stderr, "Error closing data socket!\n");
        exit(1);
    }

    return 0;
}