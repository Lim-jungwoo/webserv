#include "Server.hpp"

int	main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cout << "please use ./server port number\n";
		return (0);
	}
	Server	server;
	// t_listen	listen;
	// listen.host 
	if (server.init_server_socket(atoi(argv[1])) == 1)
		return (0);
	if (server.server_start() == 1)
		return (0);
	return (0);
}