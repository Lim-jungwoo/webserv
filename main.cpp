#include "./includes/Config.hpp"

int	main(int argc, char **argv) {
	if (argc > 2)
		return (printErr("invalid number of arguments"));

	std::string	confFile = (argc == 1) ? DEFAULT_CONF : argv[1];

	Config	conf;

<<<<<<< HEAD
	if (conf.parse(confFile))
		return (printErr("invalid configuration file"));
=======
	conf.parse(confFile);

	Server		srv = conf.selectServer("test.com");
	Location	loc = srv.selectLocation();

	std::cout << "server name: " << srv.getName() << std::endl;
>>>>>>> ffde9148b2142388403fd100328b992929e0602e
}
