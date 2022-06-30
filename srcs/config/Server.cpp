#include "./../../includes/Server.hpp"

// constructor, destructor, assignment operator overload
Server::Server ()
	: _block(),
	_host("0.0.0.0"),
	_port(),
	_name("default_server"),
	_errPages(),
	_clntSize(1024),
	_root(),
	_locations(),
	_methods(),
	_redirect(1),
	_autoindex(ON),
	_index()
{}

Server::Server (std::string block)
	: _block(block),
	_host(),
	_port(),
	_name("default_server"),
	_errPages(),
	_clntSize(1024),
	_root(),
	_locations(),
	_methods(),
	_redirect(1),
	_autoindex(ON),
	_index()
{}

Server::Server (const Server &srv)
	: _block(srv._block),
	_host(srv._host),
	_port(srv._port),
	_name(srv._name),
	_errPages(srv._errPages),
	_clntSize(srv._clntSize),
	_root(srv._root),
	_locations(srv._locations),
	_methods(srv._methods),
	_redirect(srv._redirect),
	_autoindex(srv._autoindex),
	_index(srv._index)
{}

Server::~Server () {}

Server				&Server::operator= (const Server &srv) {
	_block = srv._block;
	_host = srv._host;
	_port = srv._port;
	_name = srv._name;
	_errPages = srv._errPages;
	_clntSize = srv._clntSize;
	_root = srv._root;
	_locations = srv._locations;
	_methods = srv._methods;
	_redirect = srv._redirect;
	_autoindex = srv._autoindex;
	_index = srv._index;

	return (*this);
}

// Server::getters
std::string					Server::getBlock() { return (_block); }
std::string					Server::getHost () { return (_host); }
std::string					Server::getPort () { return (_port); }
std::string					Server::getName () { return (_name); }
std::vector<std::string>	Server::getErrPages () { return (_errPages); }
int							Server::getClntSize () { return (_clntSize); }
std::string					Server::getRoot () { return (_root); }
std::vector<Location>		Server::getLocations () { return (_locations); }
std::vector<std::string>	Server::getMethods () { return (_methods); }
int							Server::getRedirect () { return (_redirect); }
bool						Server::getAutoindex () { return (_autoindex); }
std::vector<std::string>	Server::getIndex () { return (_index); }

// Server::setters
void						Server::setBlock(std::string block) { _block = block; }
void						Server::setHost (std::string host) { _host = host; }
void						Server::setPort (std::string port) { _port = port; }
void						Server::setName (std::string name) { _name = name; }
void						Server::setErrPages (std::vector<std::string> pages) { _errPages = pages; }
void						Server::setClntSize (int size) { _clntSize = size; }
void						Server::setRoot (std::string root) { _root = root; }
void						Server::addLocation (Location lc) { _locations.push_back(lc); }
void						Server::setMethods (std::vector<std::string> methods) { _methods = methods; }
void						Server::setRedirect (int redirect) { _redirect = redirect; }
void						Server::setAutoindex (bool autoindex) { _autoindex = autoindex; }
void						Server::setIndex (std::vector<std::string> index) { _index = index; }

// Server::parse_functions
int							Server::parseAddress () {
	std::string					address;
	std::vector<std::string>	addressVec;
	std::pair<bool, size_t>		res = skipKey(_block, "listen");

	if (res.first == false) return (printErr("Server::parseAddress()"));

	address = parseValue(_block, res.second);
	addressVec = split(address, ':');

	if (addressVec.size() == 1) {
		if (address.find(".", 0) == std::string::npos) {
			setHost("0.0.0.0");
			setPort(address);
		}
		else {
			setHost(address);
			setPort("80");
		}
	}
	else if (addressVec.size() == 2) {
		setHost(addressVec[0]);
		setPort(addressVec[1]);
	}
	else return (printErr("Server::parseAddress()"));

	return (0);
}

int							Server::parseName () {
	std::pair<bool, size_t>	res = skipKey(_block, "server_name");

	setName(parseValue(_block, res.second));

	return (0);
}

int							Server::parseErrPages () {
	std::string					errPages;
	std::pair<bool, size_t>		res = skipKey(_block, "error_page");

//	TODO: if (res.first == false) return (printErr("there are no error pages"));
	if (res.first == false) return (0);

	errPages = parseValue(_block, res.second);
	setErrPages(split(errPages, ' '));

	return (0);
}

int							Server::parseClntSize () {
	std::pair<bool, size_t>	res = skipKey(_block, "client_body_size");
	int						clntSize;

	if (res.first == false) return (0);

	clntSize = strToInt(parseValue(_block, res.second));;

	if (clntSize < 0) return (printErr("size should be positive"));

	setClntSize(clntSize);

	return (0);
}

int							Server::parseRoot () {
	std::pair<bool, size_t>	res = skipKey(_block, "root");

	if (res.first == false) return (printErr("there is no root"));

	setRoot(parseValue(_block, res.second));

	return (0);
}

int							Server::parseMethods () {
	std::string				methods;
	std::pair<bool, size_t>	res = skipKey(_block, "allow_methods");

	if (res.first == false) return (printErr("there are no methods"));

	methods = parseValue(_block, res.second);
	setMethods(split(methods, ' '));

	if (_methods.empty()) return (printErr("there are no methods"));

	for (size_t i = 0; i < _methods.size(); i++) {
		if (_methods[i] != "GET" && _methods[i] != "POST"
			&& _methods[i] != "DELETE") return (printErr("invalid method"));
	}

	return (0);
}

int							Server::parseAutoindex () {
	std::string				is;
	std::pair<bool, size_t>	res = skipKey(_block, "autoindex");

	if (res.first == false) return (0);

	is = parseValue(_block, res.second);

	if (is == "on") setAutoindex(ON);
	else if (is == "off") setAutoindex(OFF);

	return (0);
}

int							Server::parseIndex () {
	std::string					index;
	std::pair<bool, size_t>		res = skipKey(_block, "index");

	if (res.first == false) return (printErr("there is no default index page"));

	index = parseValue(_block, res.second);
	setIndex(split(index, ' '));

	return (0);
}

int							Server::parse () {
	std::vector<std::string>	locBlocks = splitLocationBlocks(_block);

	for (size_t i = 0; i < locBlocks.size(); i++) {
		addLocation(Location(locBlocks[i]));
		_locations[i].parse();
	}

	parseAddress();
//	std::cout << "host: " << getHost() << ", port: " << getPort() << std::endl;
	parseName();
//	std::cout << "server name: " << getName() << std::endl;
	parseErrPages();
//	std::cout << "error pages: " << std::endl;
//	for (size_t i = 0; i < _errPages.size(); i++)
//		std::cout << "- " << _errPages[i] << std::endl;
	parseClntSize();
//	std::cout << "client size: " << getClntSize() << std::endl;
	parseRoot();
//	std::cout << "root: " << getRoot() << std::endl;
	parseMethods();
//	std::cout << "methods: " << std::endl;
//	for (size_t i = 0; i < _methods.size(); i++)
//		std::cout << "- " << _methods[i] << std::endl;
	parseAutoindex();
//	std::cout << "autoindex: " << (_autoindex ? "on" : "off") << std::endl;
	parseIndex();
//	std::cout << "indexes: " << std::endl;
//	for (size_t i = 0; i < _index.size(); i++)
//		std::cout << "- " << _index[i] << std::endl;

	std::cout << std::endl;

	return (0);
}

// TODO
std::vector<Location>		Server::findMatchingLocations(std::string route) {
}

// TODO
Location					Server::selectLocation () {
	return (_locations[0]);
}
