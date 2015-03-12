#ifndef COMMON_H
#define COMMON_H

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>   //Provides declarations for udp header
#include <netinet/tcp.h>   //Provides declarations for tcp header
#include <netinet/ip.h>
#include <stdlib.h>

// The packet length
#define PCKT_LEN 8192
#define TCP 1
#define UDP 2
#define MAX_SIZE 40
#define MAX_RULE_SIZE 400
#define MAX_RULES 10000
char * incoming_filename =  "incomingrport.cfg";
char * outgoing_filename =  "outgoingrport.cfg";
char ** incoming_rules = (char*)malloc(MAX_RULES);
char ** outgoing_rules = (char*)malloc(MAX_RULES);
int total_incoming_rules = 0;
int total_outgoing_rules = 0;

// total udp header length: 8 bytes (=64 bits)
 
// Function for checksum calculation. From the RFC,
// the checksum algorithm is:
//  "The checksum field is the 16 bit one's complement of the one's
//  complement sum of all 16 bit words in the header.  For purposes of
//  computing the checksum, the value of the checksum field is zero."
unsigned short csum(unsigned short *buf, int nwords)
{       
    unsigned long sum;
    for(sum=0; nwords>0; nwords--)
            sum += *buf++;
    sum = (sum >> 16) + (sum &0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

void fatalError(char * error)
{
	perror(error);
	exit(1);
}

void loadRules(char * filename, char ** rules)
{
	char port[MAX_SIZE];
	char forward_to_port[MAX_SIZE];
	char ip[MAX_SIZE];
	char forward_to_ip[MAX_SIZE];
	FILE * fp;

	if((fp = fopen(filename, "r")) != NULL){
		while(!feof(fp)){
			rules[total_rules] = (char*) malloc(MAX_RULE_SIZE);
			fscanf(fp, "%s %s\n", &ip[0], &forward_to_ip[0]);
			fscanf(fp, "%s %s\n", &port[0], &forward_to_port[0]);
			sprintf(rules[total_rules++], "%s:%s;%s:%s\n", ip, forward_to_ip, port, forward_to_port);
		}
	}

	fclose(fp);
}

#endif