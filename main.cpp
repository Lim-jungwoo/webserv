#include "./includes/Config.hpp"
#include "includes/Utils.hpp"

int	main(int argc, char **argv) {
	if (argc > 2)
		return (printErr("invalid number of arguments"));

	std::string	confFile = (argc == 1) ? DEFAULT_CONF : argv[1];

	Config	conf;

	if (conf.parse(confFile))
		return (printErr("invalid configuration file"));

	std::cout << "parse done" << std::endl;

	std::vector<ServerBlock>	srvs = conf.getServerBlocks();

	LocationBlock	loc = srvs[0].selectLocationBlock("/helloa");

	std::cout << "LOCATION: " << std::endl;
	std::cout << "- location uri: " << loc.getURI() << std::endl;
}
