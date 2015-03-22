    #include "../headers/connection_controller.h"
    #include "../headers/common.h"

    unsigned char udp_buffer[IP_MAXPACKET];
    unsigned char tcp_buffer[IP_MAXPACKET];
    int total_packets = 0;

    int monitor_sockets(int udp_watch_socket, int tcp_watch_socket)
    {
        int udp_data_size= 0, tcp_data_size = 0;
        struct sockaddr udp_saddr, tcp_saddr;
        int saddr_size = sizeof(tcp_saddr);
        while(true)
        {
            memset(tcp_saddr, 0, saddr_size);
            tcp_data_size = recvfrom(tcp_watch_socket, &tcp_buffer, IP_MAXPACKET, &tcp_saddr, &saddr_size);
            check_tcp_packet(&tcp_buffer, tcp_data_size, &tcp_saddr);
            
            //udp_data_size = recvfrom(udp_watch_sockets, &udp_buffer, IP_MAXPACKET, &udp_saddr, &saddr_size);
            //check_udp_packet(&udp_buffer, udp_data_size, &udp_saddr);
        }
    }

    int check_tcp_packet(unsigned char * buffer, int size, struct sockaddr * tcp_saddr)
    {
        struct ip * ip_head = (struct iphdr *) buffer;   
        int iphdr_len = ip_head->ip_hl * 4;   

        struct tcphdr * tcp_head = (struct tcpheader *) buffer + iphdr_len;

        if(check_port(tcp_head->th_sport, tcp_head->th_dport))
        {
            modify_port(buffer, TCP);
        } 

        if(check_ip(ip_head->ip_src, ip_head->ip_dst))
        {
            modify_ip(buffer, TCP);
        }
    }


    // int check_udp_packet(unsigned char * buffer, int size, struct sockaddr * udp_saddr)
    // {
    //     struct ip * ip_head = (struct iphdr *) buffer;   
    //     int iphdr_len = ip_head->ip_hl * 4;   

    //     struct udphdr * udp_head = (struct udpheader *) buffer + iphdr_len;

    //     if(check_port(udp_head->th_sport, udp_head->th_dport))
    //     {
    //         modify_port(buffer, UDP);
    //     } 

    //     if(check_ip(ip_head->ip_src, ip_head->ip_dst))
    //     {
    //         modify_ip(buffer, UDP);
    //     }
    // }

    int check_port(int sport, int dport, struct in_addr src_ip, struct in_addr dest_ip)
    {
        int i_port, i_forward_port, o_port, o_forward_port;
        for(int i = 0; i < total_rules; i++)
        {
            sscanf(incoming_rules[i], "%s:%s;%d:%d;\n", NULL, NULL, i_port, i_forward_port);
            sscanf(outgoing_rules[i], "%s:%s;%d:%d;\n", NULL, NULL, o_port, o_forward_port);
        }

        if(sport == i_port)
        {

        }

        if(dport == o_port)
        {
            return o_forward
        }
    }

    int check_ip(struct in_addr src_ip, struct in_addr dest_ip)
    {
        char ip[MAX_SIZE], ip_forward[MAX_SIZE];
        for(int i = 0; i < total_rules; i++)
        {
            sscanf(rules[i], "%s:%s;%d:%d;\n", &ip[0], &forward_to_ip[0], NULL, NULL);
        }
    }

    int modify_ip(unsigned char * buffer, int type)
    {

    }

    int modify_port(unsigned char * buffer, int type)
    {

    }