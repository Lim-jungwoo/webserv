#include "./../../includes/Config.hpp"

Config::Config () {}

Config::Config (Config &conf) { _server = conf._server; }

Config::~Config () {}

Config						&Config::operator= (Config &conf) { _server = conf._server; return (*this); }

std::vector<ServerBlock>	Config::getServerBlocks () { return (_server); }

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

	blocks = splitBlocks(buf, "server ");

	for (size_t i = 0; i < blocks.size(); i++) {
		addServerBlock(ServerBlock(blocks[i]));
		_server[i].parse();
	}

	return (0);
}
