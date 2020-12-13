#pragma once

#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define h_addr h_addr_list[0]	//The first address in h_addr_list. 

typedef struct {
	char user[256];
	char pass[256];
	char host[512];
	char path[512];
	char filename[512];
	char host_name[512];
	char ip[20];
} url_args;

int parseURL(char* args, url_args *url);
int getIP(url_args *url);
int getFilename(url_args *url);
void printURL(url_args url);