/**
*   Program:    RPort Forwarder
*   Author:     Ramzi Chennafi
*   Date:       March 15 2015
*   Functions:
*       void monitor_sockets(int tcp_watch_socket)
*       void process_tcp_packet(unsigned char * buffer, int size, struct sockaddr * tcp_saddr, int tcp_watch_socket)
*       void check_packet(int sport, char * src_ip, struct iphdr * ip_head, struct tcphdr * tcp_head)
*       void send_packet(unsigned char * buffer, int size, int sd)
*
*   Description
*   Contains the functions for reciving, modifying and sending incoming packets.
*/
#include "../headers/connection_controller.h"

char * filename =  "../rport.cfg";
char ** rules;
int host_sources[MAX_RULES];
int total_rules = 0;
int total_packets = 0;

/**
*   Function:   start_instance(int port, std::string host)
*   Author:     Ramzi Chennafi
*   Date:       Febuary 10 2015
*   Returns:    int - the socket created for the client
*
*   Notes
*   Starts up a client instance. Connects to the server.
*       port - port to send data to
*       host - hostname of server
*/
void monitor_sockets(int tcp_watch_socket)
{
    int tcp_data_size = 0;
    struct sockaddr tcp_saddr;
    int saddr_size = sizeof(tcp_saddr);
    unsigned char tcp_buffer[IP_MAXPACKET];
    memset(tcp_buffer, 0, IP_MAXPACKET);

    while(1)
    {
        if((tcp_data_size = recvfrom(tcp_watch_socket, tcp_buffer, IP_MAXPACKET, 0, &tcp_saddr, (socklen_t *)&saddr_size)) > 0)
        {
            process_tcp_packet(tcp_buffer, tcp_watch_socket);
            send_packet(tcp_buffer, tcp_data_size, tcp_watch_socket);
            memset(tcp_buffer, 0, IP_MAXPACKET);
        }
    }
}
/**
*   Function:   start_instance(int port, std::string host)
*   Author:     Ramzi Chennafi
*   Date:       Febuary 10 2015
*   Returns:    int - the socket created for the client
*
*   Notes
*   Starts up a client instance. Connects to the server.
*       port - port to send data to
*       host - hostname of server
*/
void process_tcp_packet(unsigned char * buffer, int tcp_watch_socket)
{
    struct iphdr * ip_head = (struct iphdr *) buffer;
    struct tcphdr * tcp_head = (struct tcphdr *) (buffer + IP4_HDRLEN);

    char srcaddr[INET_ADDRSTRLEN], daddr[INET_ADDRSTRLEN];;
    inet_ntop(AF_INET, &(ip_head->saddr), srcaddr, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip_head->daddr), daddr, INET_ADDRSTRLEN);

    check_packet(htons(tcp_head->th_dport), htons(tcp_head->th_sport), srcaddr, daddr, ip_head, tcp_head);
}
/**
*   Function:   start_instance(int port, std::string host)
*   Author:     Ramzi Chennafi
*   Date:       Febuary 10 2015
*   Returns:    int - the socket created for the client
*
*   Notes
*   Starts up a client instance. Connects to the server.
*       port - port to send data to
*       host - hostname of server
*/
void check_packet(int dport, int sport, char * src_ip, char * daddr, struct iphdr * ip_head, struct tcphdr * tcp_head)
{
    int port, forward_port;
    char ip[MAX_SIZE];
    char forward_ip[MAX_SIZE];

    for(int i = 0; i < total_rules; i++)
    {
        sscanf(rules[i], "%s %s %d %d", ip, forward_ip, &port, &forward_port);

        if((dport == port) && (strcmp(src_ip, ip) == 0))
        {
            host_sources[i] = tcp_head->th_sport;
            tcp_head->th_dport = htons(forward_port);
            ip_head->saddr = inet_addr("192.168.0.10");
            ip_head->daddr = inet_addr(forward_ip);
            printf("Packet %d : Sending packet with IP %s from port %d to IP %s at port %d\n", total_packets, ip, port, forward_ip, forward_port);
            total_packets++;
            break;
        }

        if((strcmp(src_ip, forward_ip) == 0) && (sport == forward_port))
        {
          ip_head->daddr = inet_addr(ip);
          printf("Packet %d : Sending packet with IP %s from port %d to IP %s at port %d", total_packets, src_ip, sport, ip, dport);
          total_packets++;
          break;
        }
    }
}
/**
*   Function:   start_instance(int port, std::string host)
*   Author:     Ramzi Chennafi
*   Date:       Febuary 10 2015
*   Returns:    int - the socket created for the client
*
*   Notes
*   Starts up a client instance. Connects to the server.
*       port - port to send data to
*       host - hostname of serverx
*/
void send_packet(unsigned char * buffer, int size, int sd)
{
    struct iphdr * ip_head = (struct iphdr *) buffer;
    struct tcphdr * tcp_head = (struct tcphdr *) (buffer + IP4_HDRLEN);

    char daddr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_head->daddr), daddr, INET_ADDRSTRLEN);

    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(daddr);
    dest_addr.sin_port = htons(tcp_head->th_dport);

    if(sendto(sd, buffer, size, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
    {
        fatalError("Failed to send packet.");
    }
}

void fatalError(char * error)
{
    perror(error);
    exit(1);
}

void loadRules()
{
    int port, forward_to_port;
    char ip[MAX_SIZE];
    char forward_to_ip[MAX_SIZE];
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
