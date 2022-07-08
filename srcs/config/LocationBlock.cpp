#include "./../../includes/LocationBlock.hpp"

LocationBlock::LocationBlock ()
	: _block(),
	_mod(NONE),
	_uri(),
	_clntSize(1024),
	_methods(),
	_redirect(),
	_root("."),
	_autoindex(ON),
	_index()
{}

LocationBlock::LocationBlock (std::string block)
	: _block(block),
	_mod(NONE),
	_uri(),
	_clntSize(1024),
	_methods(),
	_redirect(),
	_root("."),
	_autoindex(ON),
	_index()
{}

LocationBlock::LocationBlock (const LocationBlock &lb)
	: _block(lb._block),
	_mod(lb._mod),
	_uri(lb._uri),
	_clntSize(lb._clntSize),
	_methods(lb._methods),
	_redirect(lb._redirect),
	_root(lb._root),
	_autoindex(lb._autoindex),
	_index(lb._index)
{}

LocationBlock::~LocationBlock() {}

LocationBlock	&LocationBlock::operator= (const LocationBlock &lb) {
	_block = lb._block;
	_mod = lb._mod;
	_uri = lb._uri;
	_clntSize = lb._clntSize;
	_methods = lb._methods;
	_redirect = lb._redirect;
	_root = lb._root;
	_autoindex = lb._autoindex;
	_index = lb._index;

	return (*this);
}

std::string					LocationBlock::getBlock () const { return (_block); }
int							LocationBlock::getMod () const { return (_mod); }
std::string					LocationBlock::getURI () const { return (_uri); }
int							LocationBlock::getClntSize () const { return (_clntSize); }
std::vector<std::string>	LocationBlock::getMethods () const { return (_methods); }
int							LocationBlock::getRedirect () const { return (_redirect); }
std::string					LocationBlock::getRoot () const { return (_root); }
bool						LocationBlock::getAutoindex () const { return (_autoindex); }
std::vector<std::string>	LocationBlock::getIndex () const { return (_index); }
std::vector<LocationBlock>	LocationBlock::getLocationBlocks () const { return (_locations); }

void						LocationBlock::setMod (int mod) { _mod = mod; }
void						LocationBlock::setURI (std::string uri) { _uri = uri; }
void						LocationBlock::setClntSize (int clntSize) { _clntSize = clntSize; }
void						LocationBlock::setMethods (std::vector<std::string> methods) { _methods = methods; }
void						LocationBlock::setRedirect (int redirection) { _redirect = redirection; }
void						LocationBlock::setRoot (std::string root) { _root = root; }
void						LocationBlock::setAutoindex (bool autoindex) { _autoindex = autoindex; }
void						LocationBlock::setIndex (std::vector<std::string> index) { _index = index; }
void						LocationBlock::addLocationBlock (LocationBlock lc) { _locations.push_back(lc); }

int							LocationBlock::parseModMatch () {
	size_t	pos = 0, bracketPos = _block.find("{", 0);;
	size_t	end = _block.find("\n", 0);

	while (std::isspace(_block[pos]))
		pos++;

	if (_block[pos] == '/')
		setMod(NONE);
	if (_block[pos] == '=') {
		setMod(EXACT);
		pos++;
	}
	if (_block[pos] == '^' && _block[pos] == '~') {
		setMod(PREFERENTIAL);
		pos += 2;
	}
	while (std::isspace(_block[pos]))
		pos++;

	if (pos > end)
		return (printErr("invalid location block"));

	setURI(_block.substr(pos, bracketPos - 1 - pos));

	return (0);
}

int							LocationBlock::parseClntSize () {
	std::pair<bool, size_t>	res = skipKey(_block, "client_body_size");
	int						clntSize;

	if (res.first == false)
		return (0);

	clntSize = strToInt(parseValue(_block, res.second));;

	if (clntSize < 0)
		return (printErr("size should be positive"));

	setClntSize(clntSize);

	return (0);
}

int							LocationBlock::parseMethods () {
	std::string				methods;
	std::pair<bool, size_t>	res = skipKey(_block, "allow_methods");

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

int							LocationBlock::parseRoot () {
	std::pair<bool, size_t>	res = skipKey(_block, "root");

	if (res.first == false)
		return (0);

	setRoot(parseValue(_block, res.second));

	return (0);
}

int							LocationBlock::parseAutoindex () {
	std::string				is;
	std::pair<bool, size_t>	res = skipKey(_block, "autoindex");

	if (res.first == false)
		return (0);

	is = parseValue(_block, res.second);

	if (is == "on") setAutoindex(ON);
	else if (is == "off") setAutoindex(OFF);

	return (0);
}

int							LocationBlock::parseIndex () {
	std::string				index;
	std::pair<bool, size_t>	res = skipKey(_block, "index");

	if (res.first == false)
		return (0);

	index = parseValue(_block, res.second);
	setIndex(split(index, ' '));

	return (0);
}

int							LocationBlock::parse () {
	parseModMatch();
	parseClntSize();
	parseMethods();
	parseAutoindex();
	parseIndex();

/*	std::cout << "modifier: " << _mod << ", uri: " << _uri << std::endl;
	std::cout << "client size: " << _clntSize << std::endl;
	std::cout << "methods: " << std::endl;
	for (size_t i = 0; i < _methods.size(); i++)
		std::cout << "- " << _methods[i] << std::endl;
	parseRoot();
	std::cout << "root: " << _root << std::endl;
	std::cout << "autoindex: " << (_autoindex ? "on" : "off") << std::endl;
	std::cout << "indexes: " << std::endl;
	for (size_t i = 0; i < _index.size(); i++)
		std::cout << "- " << _index[i] << std::endl;
	std::cout << std::endl;*/

	return (0);
}
