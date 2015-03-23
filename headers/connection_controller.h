#include "common.h"

unsigned char tcp_buffer[IP_MAXPACKET];
int total_packets = 0;

void monitor_sockets(int tcp_watch_socket);
void process_tcp_packet(unsigned char * buffer, int size, int tcp_watch_socket);
void check_packet(int sport, char * src_ip, struct iphdr * ip_head, struct tcphdr * tcp_head);
void send_packet(unsigned char * buffer, int size, int sd);