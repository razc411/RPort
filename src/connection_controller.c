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
#include "../headers/ipchksum.h"

char * filename =  "../rport.cfg";
char ** rules;
int total_rules = 0;
int total_packets = 0;
int host_dest[MAX_RULES];
char * this_ip = "192.168.0.10";

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

            check_packet(ip_head, tcp_head);
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
void check_packet(struct iphdr * ip_head, struct tcphdr * tcp_head)
{
    int port = 0, forward_port = 0, dport = htons(tcp_head->th_dport);
    char ip[MAX_SIZE] = {0};
    char forward_ip[MAX_SIZE] = {0};
    char src_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_head->saddr), src_ip, INET_ADDRSTRLEN);


    for(int i = 0; i < total_rules; i++)
    {
        sscanf(rules[i], "%s %s %d %d", ip, forward_ip, &port, &forward_port);

        if((dport == port) && (strcmp(src_ip, ip) == 0))
        {
            host_dest[i] = htons(tcp_head->th_sport);
            tcp_head->th_dport = htons(forward_port);
            ip_head->saddr = inet_addr(this_ip);
            ip_head->daddr = inet_addr(forward_ip);
            ip_head->protocol = IPPROTO_TCP;
            ip_head->check = 0;
            tcp_head->check = get_tcp_checksum(ip_head, tcp_head);
            printf("Packet %d : Sending packet from IP %s dest port %d to IP %s at port %d\n", total_packets, src_ip, port, forward_ip, forward_port);
            total_packets++;
            break;
        }

        if((strcmp(src_ip, forward_ip) == 0) && (host_dest[i] == htons(tcp_head->th_dport)))
        {
          ip_head->saddr = inet_addr(this_ip);
          ip_head->daddr = inet_addr(ip);
          ip_head->check = 0;
          tcp_head->th_sport = htons(forward_port);
          tcp_head->check = get_tcp_checksum(ip_head, tcp_head);
          printf("Packet %d : Sending packet with IP %s dest port %d to IP %s at port %d", total_packets, src_ip, dport, ip, dport);
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
void send_packet(char * buffer, int size, int sd)
{
    struct iphdr * ip_head = (struct iphdr *) buffer;
    struct tcphdr * tcp_head = (struct tcphdr *) (buffer + (ip_head->ihl * 4));

    struct sockaddr_in dest_addr = {0};
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = ip_head->daddr;
    dest_addr.sin_port = htons(tcp_head->th_dport);

    if(sendto(sd, buffer, size, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
    {

        fatalError("Failed to send packet.");
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
void fatalError(char * error)
{
    perror(error);
    exit(1);
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
