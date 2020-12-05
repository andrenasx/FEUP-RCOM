#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int init_connection(char *ip, int port);
int write_to_socket(int sockfd, char *message);
int read_from_socket(int sockfd, int *response_code);
int read_passivemode(int sockfd, int *response_code, char **ip, int *port);