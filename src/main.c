#include "../headers/connection_controller.h"
/**
*	Function: 	main(int, char **)
*	Author: 	Ramzi Chennafi
*	Date:		Feb 21 2015
*	Returns:	int, error code
*
*	Notes
*	Main entry loop for the program, checks for arguments and begins the program according
*	to the arguments. Can establish 3 different types of servers (EDGE_SERVER, LEVEL_SERVER, LEVEL_SERVER_NO_THREAD)
*   and a client. Below are the arguments, these are optional.
*       p <port>        - port for the server to be established on or the client to send to.     DEFAULT: 5001
*       t <num threads> - number of threads for the threaded servers to run on.                  DEFAULT: 4
*       h <hostname>    - host to send data to for the client.                                   DEFAULT: LOCALHOST
*       d <data size>   - the amount of data in bytes for each client to send.                   DEFAULT: 1000000
*       c <num clients> - the number of clients to create :                                      DEFAULT: 1
*
*       1 of these arguments is required to run
*       e - runs the edge triggered threaded server.
*       w - runs the level triggered non threaded server.
*       l - runs the level triggered threaded server.
*       C - runs the client.
*/
int main(int argc, char ** argv)
{
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

	monitor_sockets(tcp_watch_socket);

	return 0;
}
