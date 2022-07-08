#include "Server.hpp"

int	main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cout << "please use ./server host:port\n";
		return (0);
	}
	Server	server;
	if (server.init_listen(argv[1]) == 1)
		return (0);
	if (server.server_start() == 1)
		return (0);
	return (0);
}