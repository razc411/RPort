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
    
    while(true)
    {
        memset(tcp_saddr, 0, saddr_size);
        tcp_data_size = recvfrom(tcp_watch_socket, &tcp_buffer, IP_MAXPACKET, &tcp_saddr, &saddr_size);
        process_tcp_packet(&tcp_buffer, tcp_data_size, tcp_watch_socket);
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
void process_tcp_packet(unsigned char * buffer, int size, int tcp_watch_socket)
{
    struct iphdr * ip_head = (struct iphdr *) buffer;   
    struct tcphdr * tcp_head = (struct tcpheader *) buffer + (ip_head->ip_hl * 4);

    check_packet(tcp_head->th_sport, inet_ntoa(ip_head->ip_src), ip_head, tcp_head);
    send_packet(buffer, size, tcp_watch_socket)
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
void check_packet(int sport, char * src_ip, struct iphdr * ip_head, struct tcphdr * tcp_head)
{
    int port, forward_port;
    char * ip;
    char * forward_ip;

    for(int i = 0; i < total_rules; i++)
    {
        sscanf(rules[i], "%s:%s;%d:%d;\n", ip, forward_ip, port, forward_port);

        if((sport == port) && (src_ip == ip))
        {
            tcp_head->th_dport = forward_port;
            ip_head->ip_dst = inet_addr(forward_ip);
            printf("Sending packet with IP %s from port %d to IP %s at port %d", port, ip, forward_ip, forward_port);
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
*       host - hostname of server
*/
void send_packet(unsigned char * buffer, int size, int sd)
{
    struct iphdr * ip_head = (struct iphdr *) buffer; 
    struct tcphdr * tcp_head = (struct tcpheader *) buffer + (ip_head->ip_hl * 4);

    if(sendto(sd, buffer, size, 0, (struct sockaddr *)ip_head->ip_dst, sizeof(*ip_head)) < 0)
    {
        fatalError("Failed to send packet.");
    }
}

