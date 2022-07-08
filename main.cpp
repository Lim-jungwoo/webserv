#include "./includes/Config.hpp"

int	main(int argc, char **argv) {
	if (argc > 2)
		return (printErr("invalid number of arguments"));

	std::string	confFile = (argc == 1) ? DEFAULT_CONF : argv[1];

	Config	conf;

	if (conf.parse(confFile))
		return (printErr("invalid configuration file"));
	conf.parse(confFile);

	ServerBlock		srv = conf.selectServerBlock("youpi");
	std::cout << "SERVER: " << std::endl;
	std::cout << "- server name: " << srv.getName() << std::endl;

	LocationBlock	loc = srv.selectLocationBlock("/bye");

	std::cout << "LOCATION: " << std::endl;
	std::cout << "- location uri: " << loc.getURI() << std::endl;

}
