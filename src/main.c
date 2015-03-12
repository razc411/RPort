#include "../headers/common.h"

int main(int argc, char ** argv)
{
	int udp_watch_socket = socket(PF_INET, SOCK_RAW, IPPROTO_UDP);
	if(udp_watch_socket < 0)
	{
		fatalError("failed to make UDP socket");
	}
	
	int tcp_watch_socket = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
	if(tcp_watch_socket < 0)
	{
		fatalError("failed to make TCP socket");
	}

	loadRules(incoming_filename, incoming_rules);
	loadRules(outgoing_filename, outgoing_rules);
	
	monitor_sockets(udp_watch_socket, tcp_watch_socket);
}