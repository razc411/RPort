#ifndef COMMON_H
#define COMMON_H

#include <unistd.h>
#include <stdio.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>   //Provides declarations for tcp header
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAX_SIZE 40
#define MAX_RULE_SIZE 400
#define MAX_RULES 10000

char * filename =  "rport.cfg";
char ** rules = (char*)malloc(MAX_RULES);
int total_rules = 0;

void fatalError(char * error)
{
	perror(error);
	exit(1);
}

void loadRules()
{
	char port[MAX_SIZE];
	char forward_to_port[MAX_SIZE];
	char ip[MAX_SIZE];
	char forward_to_ip[MAX_SIZE];
	FILE * fp;

	if((fp = fopen(filename, "r")) != NULL){
		while(!feof(fp)){
			rules[total_rules] = (char*) malloc(MAX_RULE_SIZE);
			fscanf(fp, "%s:%s;%s:%s\n", &ip[0], &port[0], &forward_to_ip[0], &forward_to_port[0]);
			sprintf(rules[total_rules++], "%s:%s;%s:%s\n", ip, forward_to_ip, port, forward_to_port);
		}
	}

	fclose(fp);
}

#endif