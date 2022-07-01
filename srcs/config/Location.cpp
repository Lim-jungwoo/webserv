#include "./../../includes/Location.hpp"

Location::Location ()
	: _block(),
	_mod(NONE),
	_match(),
	_clntSize(1024),
	_methods(),
	_redirect(),
	_root(),
	_autoindex(ON),
	_index()
{}

Location::Location (std::string block)
	: _block(block),
	_mod(NONE),
	_match(),
	_clntSize(1024),
	_methods(),
	_redirect(),
	_root(),
	_autoindex(ON),
	_index()
{}

Location::Location (const Location &lb)
	: _block(lb._block),
	_mod(lb._mod),
	_match(lb._match),
	_clntSize(lb._clntSize),
	_methods(lb._methods),
	_redirect(lb._redirect),
	_root(lb._root),
	_autoindex(lb._autoindex),
	_index(lb._index)
{}

Location::~Location() {}

Location	&Location::operator= (const Location &lb) {
	_block = lb._block;
	_mod = lb._mod;
	_match = lb._match;
	_clntSize = lb._clntSize;
	_methods = lb._methods;
	_redirect = lb._redirect;
	_root = lb._root;
	_autoindex = lb._autoindex;
	_index = lb._index;

	return (*this);
}

// Location::getters
std::string					Location::getBlock () { return (_block); }
int							Location::getMod () { return (_mod); }
std::string					Location::getMatch () { return (_match); }
int							Location::getClntSize () { return (_clntSize); }
std::vector<std::string>	Location::getMethods () { return (_methods); }
int							Location::getRedirect () { return (_redirect); }
std::string					Location::getRoot () { return (_root); }
bool						Location::getAutoindex () { return (_autoindex); }
std::vector<std::string>	Location::getIndex () { return (_index); }
std::vector<Location>		Location::getLocations () { return (_locations); }

// Location::setters
void						Location::setMod (int mod) { _mod = mod; }
void						Location::setMatch (std::string match) { _match = match; }
void						Location::setClntSize (int clntSize) { _clntSize = clntSize; }
void						Location::setMethods (std::vector<std::string> methods) { _methods = methods; }
void						Location::setRedirect (int redirection) { _redirect = redirection; }
void						Location::setRoot (std::string root) { _root = root; }
void						Location::setAutoindex (bool autoindex) { _autoindex = autoindex; }
void						Location::setIndex (std::vector<std::string> index) { _index = index; }
void						Location::addLocation (Location lc) { _locations.push_back(lc); }

// Location::parse_functions
int							Location::parseModMatch () {
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

	if (pos > end) return (printErr("invalid location block"));

	setMatch(_block.substr(pos, bracketPos - 1 - pos));

	return (0);
}

int							Location::parseClntSize () {
	std::pair<bool, size_t>	res = skipKey(_block, "client_body_size");
	int						clntSize;

	if (res.first == false) return (0);

	clntSize = strToInt(parseValue(_block, res.second));;

	if (clntSize < 0) return (printErr("size should be positive"));

	setClntSize(clntSize);

	return (0);
}

int							Location::parseMethods () {
	std::string				methods;
	std::pair<bool, size_t>	res = skipKey(_block, "allowed_methods");

//	TODO: if (res.first == false) return (printErr("there are no methods"));
	if (res.first == false) return (0);

	methods = parseValue(_block, res.second);
	setMethods(split(methods, ' '));

//	TODO: if (_methods.empty()) return (printErr("there are no methods"));
	if (_methods.empty()) return (0);

	for (size_t i = 0; i < _methods.size(); i++) {
		if (_methods[i] != "GET" && _methods[i] != "POST"
			&& _methods[i] != "DELETE") return (printErr("invalid method"));
	}

	return (0);
}

int							Location::parseRoot () {
	std::pair<bool, size_t>	res = skipKey(_block, "root");

//	TODO: if (res.first == false) return (printErr("there is no root"));
	if (res.first == false) return (0);

	setRoot(parseValue(_block, res.second));

	return (0);
}

int							Location::parseAutoindex () {
	std::string				is;
	std::pair<bool, size_t>	res = skipKey(_block, "autoindex");

	if (res.first == false) return (0);

	is = parseValue(_block, res.second);

	if (is == "on") setAutoindex(ON);
	else if (is == "off") setAutoindex(OFF);

	return (0);
}

int							Location::parseIndex () {
	std::string				index;
	std::pair<bool, size_t>	res = skipKey(_block, "index");

//	TODO: if (res.first == false) return (printErr("there is no default index page"));
	if (res.first == false) return (0);

	index = parseValue(_block, res.second);
	setIndex(split(index, ' '));

	return (0);
}

int							Location::parse () {
	parseModMatch();
//	std::cout << "modifier: " << _mod << ", match: " << _match << std::endl;
	parseClntSize();
//	std::cout << "client size: " << _clntSize << std::endl;
	parseMethods();
//	std::cout << "methods: " << std::endl;
//	for (size_t i = 0; i < _methods.size(); i++)
//		std::cout << "- " << _methods[i] << std::endl;
	parseRoot();
//	std::cout << "root: " << _root << std::endl;
	parseAutoindex();
//	std::cout << "autoindex: " << (_autoindex ? "on" : "off") << std::endl;
	parseIndex();
//	std::cout << "indexes: " << std::endl;
//	for (size_t i = 0; i < _index.size(); i++)
//		std::cout << "- " << _index[i] << std::endl;
	std::cout << std::endl;

	return (0);
}
