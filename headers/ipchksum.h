// thanx to http://seclists.org/lists/bugtraq/1999/Mar/0057.html
// code retrieved from http://sysnet.ucsd.edu/~cfleizac/iptcphdr.html
// I take no credit for this code, im just not the best at figuring the math
// struct tcp_pseudo /*the tcp pseudo header*/
// {
//   unsigned int src_addr;
//   unsigned int dst_addr;
//   unsigned char zero;
//   unsigned char proto;
//   unsigned short length;
// } pseudohead;
//
// unsigned short checksum(unsigned short *addr, unsigned int count) {
//
//        register long sum = 0;
//
//
//         while( count > 1 )  {
//            /*  This is the inner loop */
//                sum += * addr++;
//                count -= 2;
//        }
//            /*  Add left-over byte, if any */
//        if( count > 0 )
//                sum += * (unsigned char *) addr;
//
//            /*  Fold 32-bit sum to 16 bits */
//        while (sum>>16)
//            sum = (sum & 0xffff) + (sum >> 16);
//
//        return ~sum;
// }

// unsigned short get_tcp_checksum(struct iphdr * myip, struct tcphdr * mytcp) {
//
//         unsigned short total_len = ntohs(myip->tot_len);
//
//         int tcpopt_len = mytcp->doff*4 - 20;
//         int tcpdatalen = total_len - (mytcp->doff*4) - (myip->ihl*4);
//
//         pseudohead.src_addr=myip->saddr;
//         pseudohead.dst_addr=myip->daddr;
//         pseudohead.zero=0;
//         pseudohead.proto=IPPROTO_TCP;
//         pseudohead.length=htons(sizeof(struct tcphdr) + tcpopt_len + tcpdatalen);
//
//         int totaltcp_len = sizeof(struct tcp_pseudo) + sizeof(struct tcphdr) + tcpopt_len + tcpdatalen;
//         unsigned short * tcp = (unsigned short*) malloc(totaltcp_len);
//
//         memcpy((unsigned char *)tcp,&pseudohead,sizeof(struct tcp_pseudo));
//         memcpy((unsigned char *)tcp+sizeof(struct tcp_pseudo),(unsigned char *)mytcp,sizeof(struct tcphdr));
//         memcpy((unsigned char *)tcp+sizeof(struct tcp_pseudo)+sizeof(struct tcphdr), (unsigned char *)myip+(myip->ihl*4)+(sizeof(struct tcphdr)), tcpopt_len);
//         memcpy((unsigned char *)tcp+sizeof(struct tcp_pseudo)+sizeof(struct tcphdr)+tcpopt_len, (unsigned char *)mytcp+(mytcp->doff*4), tcpdatalen);
//
//         return checksum(tcp,totaltcp_len);
// }
struct pseudoTcpHeader {
unsigned int ip_src;
unsigned int ip_dst;
unsigned char zero;
unsigned char protocol;
unsigned short tcp_len;
};
unsigned short csum(unsigned short *buf, int nwords){
  unsigned long sum;
  for(sum = 0; nwords > 1; nwords -= 2) {
  sum += *buf++;
  }
  // add the left-over byte
  if(nwords > 0)
  sum += *(unsigned char *)buf;
  // turn the 32 bit words to 16 bit.
  while (sum >> 16) {
  sum = (sum & 0xffff) + (sum >> 16);
  }
  return (unsigned short)~sum;
}

unsigned short tcp_csum(struct iphdr *ip_header, struct tcphdr *tcp_header){
  unsigned short total_len = ntohs(ip_header->tot_len);
  unsigned short checksum;
  // tcp lengths
  int tcpopt_len = (tcp_header->doff * 4) - 20;
  int tcpdatalen = total_len - (tcp_header->doff*4) - (ip_header->ihl*4);
  // pseudo header
  struct pseudoTcpHeader pseudohead;
  int totaltcp_len = sizeof(struct pseudoTcpHeader) + sizeof(struct tcphdr) + tcpopt_len + tcpdatalen;
  unsigned short *psuedoheader_tcpsegment = (unsigned short*)malloc(totaltcp_len);
  pseudohead.ip_src = ip_header->saddr;
  pseudohead.ip_dst = ip_header->daddr;
  pseudohead.zero = 0;
  pseudohead.protocol = IPPROTO_TCP;
  pseudohead.tcp_len = htons(sizeof(struct tcphdr) + tcpopt_len + tcpdatalen);
  // put the pseudo header into memory
  memcpy((unsigned char*)psuedoheader_tcpsegment, &pseudohead, sizeof(struct pseudoTcpHeader));
  // put the tcp header into memory
  memcpy((unsigned char*)psuedoheader_tcpsegment + sizeof(struct pseudoTcpHeader), (unsigned char*)tcp_header, sizeof(struct tcphdr));
  // put the tcp options into memory
  memcpy((unsigned char*)psuedoheader_tcpsegment + sizeof(struct pseudoTcpHeader) + sizeof(struct tcphdr), (unsigned char*)ip_header + (ip_header->ihl * 4) + sizeof(struct tcphdr), tcpopt_len);
  // put the tcp data into memory
  memcpy((unsigned char*)psuedoheader_tcpsegment + sizeof(struct pseudoTcpHeader) + sizeof(struct tcphdr) + tcpopt_len, (unsigned char*)tcp_header + (tcp_header->doff * 4), tcpdatalen);
  checksum = csum(psuedoheader_tcpsegment, totaltcp_len);
  free(psuedoheader_tcpsegment);
  return checksum;
}
