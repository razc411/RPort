/**
*   Program:    RPort Forwarder
*   Author:     Ramzi Chennafi
*   Date:       March 15 2015
*   Functions:
*       void monitor_sockets(int tcp_watch_socket)
*       void check_packet(struct iphdr * ip_head, struct tcphdr * tcp_head, char * buffer, int size)
*       void fatalError(char * error)
*       void send_packet(unsigned char * buffer, int size, int sd)
*       void loadRules()
*
*   Description
*   Contains the functions for reciving, modifying and sending incoming packets.
*/
#include "../headers/connection_controller.h"
#include "../headers/ipchksum.h"

char * filename =  "../rport.cfg";
char ** rules;
int total_rules = 0;
int total_packets = 0;
unsigned short host_dest[MAX_RULES];
char * this_ip;
/**
*   Function:   monitor_sockets(int tcp_watch_socket)
*   Author:     Ramzi Chennafi
*   Date:       March 15 2015
*   Returns:    void
*
*   Notes
*   Monitors the raw sockets for incoming tcp datagrams to be forwarded.
*       tcp_watch_socket - raw socket to be watched
*/
void monitor_sockets(int tcp_watch_socket, char * ip)
{
    int tcp_data_size = 0;
    this_ip = ip;
    char tcp_buffer[IP_MAXPACKET];
    memset(tcp_buffer, 0, IP_MAXPACKET);

    while(1)
    {
        if((tcp_data_size = recvfrom(tcp_watch_socket, tcp_buffer, IP_MAXPACKET, 0, 0, 0)) > 0)
        {
            struct iphdr * ip_head = (struct iphdr *) tcp_buffer;
            struct tcphdr * tcp_head = (struct tcphdr *) (tcp_buffer + (ip_head->ihl * 4));
            if(ip_head->protocol != IPPROTO_TCP)
            {
              continue;
            }

            check_packet(ip_head, tcp_head, tcp_buffer, tcp_data_size);
            send_packet(tcp_buffer, tcp_data_size, tcp_watch_socket);
            memset(tcp_buffer, 0, IP_MAXPACKET);
        }
    }
}
/**
*   Function:   check_packet(struct iphdr * ip_head, struct tcphdr * tcp_head, char * buffer, int size)
*   Author:     Ramzi Chennafi
*   Date:       March 15 2015
*   Returns:    void
*
*   Notes
*   Monitors the raw sockets for incoming tcp datagrams to be forwarded.
*       ip_head - a pointer to the ip header of the packet to check
*       tcp_hejad - a pointer to the tcp header of the packet to check
*       buffer - a pointer to the buffer containing the entire packet
*       size - the size of the entire packet
*/
void check_packet(struct iphdr * ip_head, struct tcphdr * tcp_head, char * buffer, int size)
{
    int port = 0, forward_port = 0, dport = htons(tcp_head->th_dport);
    char ip[MAX_SIZE] = {0};
    char forward_ip[MAX_SIZE] = {0};
    char src_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_head->saddr), src_ip, INET_ADDRSTRLEN);

    for(int i = 0; i < total_rules; i++)
    {
        sscanf(rules[i], "%s %s %d %d", ip, forward_ip, &port, &forward_port);

        if(dport == port)
        {
            host_dest[i] = tcp_head->th_sport;
            tcp_head->th_dport = htons(forward_port);
            ip_head->saddr = inet_addr(this_ip);
            ip_head->daddr = inet_addr(forward_ip);
            ip_head->check = 0;
            printf("Packet %d : Sending packet from IP %s dest port %d to IP %s at port %d\n", total_packets, src_ip, port, forward_ip, forward_port);
            total_packets++;
            break;
        }

        if((strcmp(src_ip, forward_ip) == 0))
        {
          ip_head->saddr = inet_addr(this_ip);
          ip_head->daddr = inet_addr(ip);
          ip_head->check = 0;
          tcp_head->th_sport = htons(forward_port);
          tcp_head->th_dport = host_dest[i];
          printf("Packet %d : Sending packet with IP %s dest port %d to IP %s at port %d\n", total_packets, src_ip, dport, ip, dport);
          total_packets++;
          break;
        }
    }
    tcp_head->check = 0;
    tcp_head->check = tcp_csum(ip_head, tcp_head);
}
/**
*   Function:   send_packet(char * buffer, int size, int sd)
*   Author:     Ramzi Chennafi
*   Date:       March 15 2015
*   Returns:    void
*
*   Notes
*   Sends TCP data out on a raw socket.
*       sd - the socket to send the data out on
*       buffer - a pointer to the buffer containing the tcp packet
*       size - the size of the tcp packet contained in the buffer
*/
void send_packet(char * buffer, int size, int sd)
{
    struct iphdr * ip_head = (struct iphdr *) buffer;
    struct tcphdr * tcp_head = (struct tcphdr *) (buffer + (ip_head->ihl * 4));

    struct sockaddr_in dest_addr = {0};
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = ip_head->daddr;
    dest_addr.sin_port = tcp_head->th_dport;

    if(sendto(sd, buffer, size, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
    {
        fatalError("Failed to send packet.");
    }
}
/**
*   Function:   fatalError(char * error)
*   Author:     Ramzi Chennafi
*   Date:       March 15 2015
*   Returns:    void
*
*   Notes
*   Prints the most recent error and exits the program.
*       error - a point to a cstring containing the additional message to be displayed with the error.
*/
void fatalError(char * error)
{
    perror(error);
    exit(1);
}
/**
*   Function:   loadRules()
*   Author:     Ramzi Chennafi
*   Date:       March 15 2015
*   Returns:    void
*
*   Notes
*   Loads the forwarding rules for the program from the config file and sets up iptables to
*   work with the program.
*       **adds each rule from file to the rules array.
*/
void loadRules()
{
    int port = 0, forward_to_port = 0;
    char ip[MAX_SIZE] = {0};
    char forward_to_ip[MAX_SIZE] = {0};
    FILE * fp;
    rules = (char**)malloc(MAX_RULES);

    if((fp = fopen(filename, "r")) != NULL){
        while(!feof(fp)){
            rules[total_rules] = (char*) malloc(MAX_RULE_SIZE);

            fscanf(fp, "%s %d %s %d\n", &ip[0], &port, &forward_to_ip[0], &forward_to_port);
            sprintf(rules[total_rules++], "%s %s %d %d", ip, forward_to_ip, port, forward_to_port);
            printf("Added rule %s\n", rules[total_rules-1]);
        }
    }

    system("iptables -A OUTPUT -p tcp --tcp-flags RST RST -j DROP");

    fclose(fp);
}
