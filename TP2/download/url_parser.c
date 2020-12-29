#include "url_parser.h"

int parseURL(char* args, url_args *url) {
    char* ftp = strtok(args, "/");
    char* urlrest = strtok(NULL, "/");
    char* path = strtok(NULL, "");

    if (strcmp(ftp, "ftp:") != 0) {
        fprintf(stderr, "Error: Not using ftp\n");
        return -1;
    }
  
    char* user = strtok(urlrest, ":");
    char* pass = strtok(NULL, "@");


    // default user and pass
    if (pass == NULL) {
        user = "anonymous";
        pass = "pass";
        strcpy(url->host, urlrest);
    }
    else {
        char* host = strtok(NULL, "");
        strcpy(url->host, host);
    }
  
    strcpy(url->path, path);
    strcpy(url->user, user);
    strcpy(url->pass, pass);
  
    if (getIP(url) != 0){
        fprintf(stderr, "Error: getIp()\n");
        return -1;
    }

    if (getFilename(url) != 0){
        fprintf(stderr, "Error: getFileName()\n");
        return -1;
    }

    return 0;
}

int getIP(url_args *url){
    struct hostent *h;

    if ((h=gethostbyname(url->host)) == NULL) {  
        herror("gethostbyname");
        return -1;
    }

    char* host_name = h->h_name;
    strcpy(url->host_name, host_name);

    char* ip = inet_ntoa(*((struct in_addr*)h->h_addr));
    strcpy(url->ip, ip);

    return 0;
}

int getFilename(url_args *url){
    char* filename = url->path;
    char* p;
    for(p = url->path; *p; p++){
        if(*p == '/' || *p == '\\' || *p == ':'){
            filename = p+1;
        }
    }

    strcpy(url->filename, filename);

    return 0;
}

void printURL(url_args url){
    printf("\nUser: %s\n", url.user);
    printf("Password: %s\n", url.pass);
    printf("Host: %s\n", url.host);
    printf("Path: %s\n", url.path);
    printf("Filename: %s\n", url.filename);
    printf("Host name: %s\n", url.host_name);
    printf("IP address: %s\n\n", url.ip);
}