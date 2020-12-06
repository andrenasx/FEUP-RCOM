#pragma once

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

int ftp_init(char *ip, int port);
int ftp_quit();
int ftp_login(char* user, char* pass);
int ftp_binarymode();
int ftp_pasvmode(char *ip, int *port);
int ftp_retr(char *file);
int ftp_download(char *path, char *filename);
int transferFile(int datafd, char *filename);