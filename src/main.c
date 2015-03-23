#include "../headers/connection_controller.h"
/**
*	Function: 	main(int, char **)
*	Author: 	Ramzi Chennafi
*	Date:		March 15 2015
*	Returns:	int, error code
*
*	Notes
*	Main entry loop for the port forwarder. Creates a raw socket, loads the forwarding rules and monitors
*	the created raw socket for incoming packets to forward. Takes the IP of the computer running it as a command
*   line argument.
*
*	Reads the rport.cfg file in the main directory for determining rules.
*/
int main(int argc, char ** argv)
{
	if(argc != 2)
	{
		printf("Requires the ip of the machine running the forwarder to proceed.\n");
	}
	char * ip = argv[1];

	int tcp_watch_socket = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
	if(tcp_watch_socket < 0)
	{
		fatalError("failed to make TCP socket");
	}

	int hdrincl=1;
	if (setsockopt(tcp_watch_socket, IPPROTO_IP, IP_HDRINCL, &hdrincl, sizeof(hdrincl))==-1) {
	  fatalError("Failed to setsockopt - IP_HDRINCL");
	}

	loadRules();

	monitor_sockets(tcp_watch_socket, ip);

	return 0;
}
