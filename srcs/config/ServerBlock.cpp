#include "./../../includes/ServerBlock.hpp"

ServerBlock::ServerBlock ()
	: _block(""),
	_host("*"),
	_port("80"),
	_default(false),
	_name(""),
	_errPages(),
	_clntSize(MiBToBits("1m")),
	_root("html"),
	_locations(),
	_methods(),
	_redirect(1),
	_autoindex(ON),
	_index()
{}

ServerBlock::ServerBlock (std::string block)
	: _block(block),
	_host("*"),
	_port("80"),
	_default(false),
	_name(""),
	_errPages(),
	_clntSize(MiBToBits("1m")),
	_root("html"),
	_locations(),
	_methods(),
	_redirect(1),
	_autoindex(ON),
	_index()
{}

ServerBlock::ServerBlock (const ServerBlock &srv)
	: _block(srv._block),
	_host(srv._host),
	_port(srv._port),
	_default(srv._default),
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

ServerBlock::~ServerBlock () {}

ServerBlock					&ServerBlock::operator= (const ServerBlock &srv) {
	_block = srv._block;
	_host = srv._host;
	_port = srv._port;
	_default = srv._default;
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

std::string					ServerBlock::getBlock() const { return (_block); }
std::string					ServerBlock::getHost () const { return (_host); }
std::string					ServerBlock::getPort () const { return (_port); }
bool						ServerBlock::getDefault () const { return (_default); }
std::string					ServerBlock::getName () const { return (_name); }
std::vector<std::string>	ServerBlock::getErrPages () const { return (_errPages); }
int							ServerBlock::getClntSize () const { return (_clntSize); }
std::string					ServerBlock::getRoot () const { return (_root); }
std::vector<LocationBlock>	ServerBlock::getLocationBlocks () const { return (_locations); }
std::vector<std::string>	ServerBlock::getMethods () const { return (_methods); }
int							ServerBlock::getRedirect () const { return (_redirect); }
bool						ServerBlock::getAutoindex () const { return (_autoindex); }
std::vector<std::string>	ServerBlock::getIndex () const { return (_index); }

void						ServerBlock::setBlock(std::string block) { _block = block; }
void						ServerBlock::setHost (std::string host) { _host = host; }
void						ServerBlock::setPort (std::string port) { _port = port; }
void						ServerBlock::setDefault (bool dflt) { _default = dflt; }
void						ServerBlock::setName (std::string name) { _name = name; }
void						ServerBlock::setErrPages (std::vector<std::string> pages) { _errPages = pages; }
void						ServerBlock::setClntSize (int size) { _clntSize = size; }
void						ServerBlock::setRoot (std::string root) { _root = root; }
void						ServerBlock::addLocationBlock (LocationBlock lc) { _locations.push_back(lc); }
void						ServerBlock::setMethods (std::vector<std::string> methods) { _methods = methods; }
void						ServerBlock::setRedirect (int redirect) { _redirect = redirect; }
void						ServerBlock::setAutoindex (bool autoindex) { _autoindex = autoindex; }
void						ServerBlock::setIndex (std::vector<std::string> index) { _index = index; }

int							ServerBlock::parseAddress () {
	std::string					address;
	std::vector<std::string>	addressVec;
	std::pair<bool, size_t>		res = skipKey(_block, "listen");

	if (res.first == false)
		return (0);

	address = parseValue(_block, res.second);
	if (address.find(" ", 0) != std::string::npos) {
		addressVec = split(address, ' ');
		if (addressVec.size() > 2)
			return (printErr("wrong address field"));
		if (addressVec[1] == "default_server")
			_default = true;
	}
	addressVec = split(addressVec[0], ':');

	if (addressVec.size() == 1) {
		if (addressVec[0].find(".", 0) == std::string::npos)
			setPort(addressVec[0]);
		else
			setHost(addressVec[0]);
	}
	else if (addressVec.size() == 2) {
		setHost(addressVec[0]);
		setPort(addressVec[1]);
	}
	else
		return (printErr("in ServerBlock::parseAddress()"));

	return (0);
}

int							ServerBlock::parseName () {
	std::string				names;
	std::pair<bool, size_t>	res = skipKey(_block, "server_name");

	if (res.first == false)
		return (0);

	names = parseValue(_block, res.second);

	setName(split(names, ' ')[0]);

	return (0);
}

int							ServerBlock::parseErrPages () {
	std::string				errPages;
	std::pair<bool, size_t>	res = skipKey(_block, "error_page");

	if (res.first == false)
		return (0);

	errPages = parseValue(_block, res.second);
	setErrPages(split(errPages, ' '));

	return (0);
}

int							ServerBlock::parseClntSize () {
	std::pair<bool, size_t>	res = skipKey(_block, "client_max_body_size");
	int						clntSize;

	if (res.first == false)
		return (0);

	clntSize = MiBToBits(parseValue(_block, res.second));

	if (clntSize < 0)
		return (printErr("size should be positive"));

	setClntSize(clntSize);

	return (0);
}

int							ServerBlock::parseRoot () {
	std::pair<bool, size_t>	res = skipKey(_block, "root");

	if (res.first == false)
		return (0);

	setRoot(parseValue(_block, res.second));

	return (0);
}

int							ServerBlock::parseMethods () {
	std::string				methods;
	std::pair<bool, size_t>	res = skipKey(_block, "limit_except");

	if (res.first == false)
		return (0);

	methods = parseValue(_block, res.second);
	setMethods(split(methods, ' '));

	if (_methods.empty())
		return (0);

	for (size_t i = 0; i < _methods.size(); i++) {
		if (_methods[i] != "GET" && _methods[i] != "POST" && _methods[i] != "DELETE")
			return (printErr("invalid method"));
	}

	return (0);
}

int							ServerBlock::parseAutoindex () {
	std::string				is;
	std::pair<bool, size_t>	res = skipKey(_block, "autoindex");

	if (res.first == false)
		return (0);

	is = parseValue(_block, res.second);

	if (is == "on")
		setAutoindex(ON);
	else if (is == "off")
		setAutoindex(OFF);

	return (0);
}

int							ServerBlock::parseIndex () {
	std::string				index;
	std::pair<bool, size_t>	res = skipKey(_block, "index");

	if (res.first == false) {
		std::vector<std::string>	idx;
		idx.push_back("index.html");
		setIndex(idx);
		return (0);
	}

	index = parseValue(_block, res.second);
	setIndex(split(index, ' '));

	return (0);
}

int							ServerBlock::parse () {
/*	std::vector<std::string>	locBlocks = splitBlocks(_block, "location ");
//	std::vector<std::string>	locBlocks = splitLocationBlocks(_block);

	for (size_t i = 0; i < locBlocks.size(); i++) {
		std::cout << "location block: " << locBlocks[i] << std::endl;
		addLocationBlock(LocationBlock(locBlocks[i]));
		_locations[i].parse();
	}*/

	parseAddress();
	parseName();
	parseErrPages();
	parseClntSize();
	parseRoot();
	parseMethods();
	parseAutoindex();
	parseIndex();
/*	std::cout << "host: " << getHost() << ", port: " << getPort() << std::endl;
	std::cout << "server name: " << getName() << std::endl;
	std::cout << "error pages: " << std::endl;
	for (size_t i = 0; i < _errPages.size(); i++)
		std::cout << "- " << _errPages[i] << std::endl;
	std::cout << "client size: " << getClntSize() << std::endl;
	std::cout << "root: " << getRoot() << std::endl;
	std::cout << "methods: " << std::endl;
	for (size_t i = 0; i < _methods.size(); i++)
		std::cout << "- " << _methods[i] << std::endl;
	std::cout << "autoindex: " << (_autoindex ? "on" : "off") << std::endl;
	std::cout << "indexes: " << std::endl;
	for (size_t i = 0; i < _index.size(); i++)
		std::cout << "- " << _index[i] << std::endl;
	std::cout << std::endl;*/

	return (0);
}

LocationBlock				ServerBlock::selectLocationBlock (std::string requestURI) const {
	std::vector<LocationBlock>	locationBlocks;
	size_t						max = 0;
	size_t						ret = 0;

	for (size_t i = 0; i < _locations.size(); i++) {
		if (_locations[i].getMod() == EXACT) {
			if (_locations[i].getURI() == requestURI)
				return (_locations[i]);
		}
	}

	for (size_t i = 0; i < _locations.size(); i++) {
		if (_locations[i].getMod() == NONE || _locations[i].getMod() == PREFERENTIAL) {
			if (requestURI.find(_locations[i].getURI(), 0) != std::string::npos)
				locationBlocks.push_back(_locations[i]);
		}
	}

	if (locationBlocks.empty())
		return (LocationBlock());

	max = locationBlocks[ret].getURI().size();
	for (size_t i = 1; i < locationBlocks.size(); i++) {
		if (locationBlocks[i].getURI().size() > max) {
			max = locationBlocks[i].getURI().size();
			ret = i;
		}
	}

	return (locationBlocks[ret]);
}
