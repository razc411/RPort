// thanx to http://seclists.org/lists/bugtraq/1999/Mar/0057.html
// code retrieved from http://sysnet.ucsd.edu/~cfleizac/iptcphdr.html
// I take no credit for this code, im just not the best at figuring the math
struct tcp_pseudo /*the tcp pseudo header*/
{
  unsigned int src_addr;
  unsigned int dst_addr;
  unsigned char zero;
  unsigned char proto;
  unsigned short length;
} pseudohead;

long checksum(unsigned short *addr, unsigned int count) {

       register long sum = 0;


        while( count > 1 )  {
           /*  This is the inner loop */
               sum += * addr++;
               count -= 2;
       }
           /*  Add left-over byte, if any */
       if( count > 0 )
               sum += * (unsigned char *) addr;

           /*  Fold 32-bit sum to 16 bits */
       while (sum>>16)
           sum = (sum & 0xffff) + (sum >> 16);

       return ~sum;
}

long get_tcp_checksum(struct iphdr * myip, struct tcphdr * mytcp) {

        unsigned short total_len = ntohs(myip->tot_len);

        int tcpopt_len = mytcp->doff*4 - 20;
        int tcpdatalen = total_len - (mytcp->doff*4) - (myip->ihl*4);

        pseudohead.src_addr=myip->saddr;
        pseudohead.dst_addr=myip->daddr;
        pseudohead.zero=0;
        pseudohead.proto=IPPROTO_TCP;
        pseudohead.length=htons(sizeof(struct tcphdr) + tcpopt_len + tcpdatalen);

        int totaltcp_len = sizeof(struct tcp_pseudo) + sizeof(struct tcphdr) + tcpopt_len + tcpdatalen;
        unsigned short * tcp = (unsigned short*) malloc(totaltcp_len);

        memcpy((unsigned char *)tcp,&pseudohead,sizeof(struct tcp_pseudo));
        memcpy((unsigned char *)tcp+sizeof(struct tcp_pseudo),(unsigned char *)mytcp,sizeof(struct tcphdr));
        memcpy((unsigned char *)tcp+sizeof(struct tcp_pseudo)+sizeof(struct tcphdr), (unsigned char *)myip+(myip->ihl*4)+(sizeof(struct tcphdr)), tcpopt_len);
        memcpy((unsigned char *)tcp+sizeof(struct tcp_pseudo)+sizeof(struct tcphdr)+tcpopt_len, (unsigned char *)mytcp+(mytcp->doff*4), tcpdatalen);

        return checksum(tcp,totaltcp_len);
}
