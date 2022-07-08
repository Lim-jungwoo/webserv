#include "./../../includes/Config.hpp"

Config::Config () {}

Config::Config (Config &conf) { _server = conf._server; }

Config::~Config () {}

Config						&Config::operator= (Config &conf) { _server = conf._server; return (*this); }

int							Config::checkServerBlocks () const {
}

void						Config::addServerBlock (ServerBlock serv) { _server.push_back(serv); }

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
		addServerBlock(ServerBlock(blocks[i]));
		_server[i].parse();
	}

	checkServerBlocks();
	return (0);
}

std::vector<ServerBlock>	Config::findMatchingServerBlocks (std::string request, std::string *host, std::string *port) const {
	std::vector<ServerBlock>	ret;
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

ServerBlock						Config::selectServerBlock (std::string request) const {
	std::string					host, port;
	std::vector<ServerBlock>	ret = findMatchingServerBlocks(request, &host, &port);

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
	return (ServerBlock());
}
