#include "ftp.h"

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

    //init
    if(ftp_init(url.ip, 21) != 0){
        fprintf(stderr, "Error service no ready!\n");
        exit(1);
    }

    //login
    if(ftp_login(url.user, url.pass) != 0){
        fprintf(stderr, "Error logging in!\n");
        exit(1);
    }

    //binary mode
    if(ftp_binarymode() != 0){
        fprintf(stderr, "Error setting binary mode!\n");
        exit(1);
    }

    //download file
    if(ftp_download(url.path, filename) != 0){
        fprintf(stderr, "Error downloading file!\n");
        exit(1);
    }

    //close socket
    if(ftp_quit() != 0){
        fprintf(stderr, "Error closing socket!\n");
        exit(1);
    }

    return 0;
}