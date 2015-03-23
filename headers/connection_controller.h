#ifndef CONN_CONTROLLER_H
#define CONN_CONTROLLER_H

#include <unistd.h>
#include <stdio.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>   //Provides declarations for tcp header
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

#define MAX_SIZE 32
#define MAX_RULE_SIZE 100
#define MAX_RULES 10000
#define IP4_HDRLEN 20         // IPv4 header length
#define TCP_HDRLEN 20         // TCP header length, excludes options data

void monitor_sockets(int tcp_watch_socket, char * ip);
void check_packet(struct iphdr * ip_head, struct tcphdr * tcp_head, char * buffer, int size);
void send_packet(char * buffer, int size, int sd);

void loadRules();
void fatalError(char * error);

#endif //CONN_CONTROLLER_H
