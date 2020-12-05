#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>

#include "url_parser.h"

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

    return 0;
}