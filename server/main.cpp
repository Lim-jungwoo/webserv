#include "Server.hpp"

int	main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << "please use ./server host port\n";
		return (0);
	}
	Server	server;
	t_listen	listen;
	listen.host = host_to_int(argv[1]);
	listen.port = atoi(argv[2]);
	server.init_listen(listen);
	if (server.init_server_socket() == 1)
		return (0);
	if (server.server_start() == 1)
		return (0);
	return (0);
}