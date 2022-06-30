#include "./../../includes/Config.hpp"

Config::Config () {}

Config::Config (Config &conf) { _server = conf._server; }

Config::~Config () {}

Config						&Config::operator= (Config &conf) { _server = conf._server; return (*this); }

void						Config::addServer (Server serv) { _server.push_back(serv); }

/* function that parses the configuration file */
int							Config::parse (std::string file) {
	std::string					buf;
	std::ifstream				f(file);
	std::ostringstream			ss;
	std::vector<std::string>	blocks;

	if (!f)
		return (1);

	ss << f.rdbuf();
	buf = ss.str();

	blocks = splitServerBlocks(buf);
	for (size_t i = 0; i < blocks.size(); i++) {
		addServer(Server(blocks[i]));
		_server[i].parse();
	}
	return (0);
}

/* functions that finds the servers whose address/name match the request (TODO: CHECK) */ 
std::vector<Server>			Config::findMatchingServers (std::string request, std::string *host, std::string *port) {
	std::vector<Server>			ret;
	std::vector<std::string>	addr = split(request, ':');

	*host = addr[0];
	*port = "80";
	if (addr.size() == 2)
		*port = addr[1];

	for (size_t i = 0; i < _server.size(); i++) {
		if (_server[i].getPort() == *port)
			ret.push_back(_server[i]);
	}

	return (ret);
}

/* function that selects the appropriate server for the request (TODO: CHECK) */
Server						Config::selectServer (std::string request) {
	std::string					host, port;
	std::vector<Server>			ret = findMatchingServers(request, &host, &port);

	if (ret.size() == 1)
		return (ret[0]);

	for (size_t i = 0; i < ret.size(); i++) {
		if (ret[i].getName() == host)
			return (ret[i]);
	}

	for (size_t i = 0; i < ret.size(); i++) {
		if (ret[i].getName() == "default_server")
			return (ret[i]);
	}
	return (Server());
}
