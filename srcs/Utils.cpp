#include "./../includes/Utils.hpp"

/* function that prints an error message and returns 1. */
int							printErr (std::string errMsg) {
	std::cerr << "Error: " << errMsg << std::endl;
	return (1);
}

/* function that splits a string by a delimiter. */
std::vector<std::string>	split (std::string str, char delimiter) {
	size_t						pos_start = 0, pos_end;
	std::vector<std::string>	ret;
	std::string					token;

	if (str.find(delimiter, 0) == std::string::npos) {
		ret.push_back(str);
		return (ret);
	}

	while ((pos_end = str.find(delimiter, pos_start)) != std::string::npos) {
		token = str.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + 1;
		ret.push_back(token);

	}
	ret.push_back(str.substr(pos_start));
	return (ret);
}

/* function that splits a config file into server blocks. */
std::vector<std::string>	splitServerBlocks (std::string block) {
	std::vector<std::string>	ret;
	int							locBlockCount = 0;					// counts location blocks
	size_t						pos = 0, blockPos = 0, locPos = 0;	// blockPos = position of "server", locPos = position of "location"
	size_t						nextBlockPos = 0;					// position of next "server"

	while (1) {
		blockPos = block.find("server ", pos) + 6;
		pos = blockPos;
		nextBlockPos = block.find("server ", pos);
		while (std::isspace(block[pos]))
			pos++;
		if (block[pos] != '{')
			return (ret);

		locPos = block.find("location", pos);
		while (locPos != std::string::npos && locPos < nextBlockPos) {
			locPos = block.find("location", locPos + 1);
			locBlockCount++;
		}
		// skips the counted location blocks
		while (locBlockCount) {
			pos = block.find("}", pos) + 1;
			locBlockCount--;
		}
		pos += 2;
		ret.push_back(block.substr(blockPos, pos - blockPos));
	}
	return (ret);
}

/* function that splits server block into location blocks. */
std::vector<std::string>	splitLocationBlocks (std::string block) {
	std::vector<std::string>	ret;
	size_t						blockPos = 0, pos = 0;
	size_t						nextLocPos = 0, closeBracketPos = 0;

	while (1) {
		blockPos = block.find("location", pos) + 8;
		pos = blockPos;
		nextLocPos = block.find("location", pos);

		closeBracketPos = block.find("}", pos);

		pos = closeBracketPos;
		if (nextLocPos < closeBracketPos)
			pos += 3;
		pos++;

		ret.push_back(block.substr(blockPos, pos - blockPos));

		if (block[pos + 1] == '}')
			break ;
	}

	return (ret);
}

/* function that returns the position after skipping [key]. (if [key] is not found, bool = false.) */
std::pair<bool, size_t>		skipKey (std::string line, std::string key) {
	size_t	pos = line.find(key, 0);
	size_t	scPos = line.find(";", pos);
	size_t	nlPos = line.find("\n", pos);

	if (pos == std::string::npos)
		return (std::make_pair(false, pos));

	if (scPos == std::string::npos || nlPos == std::string::npos || scPos > nlPos) {
		printErr("invalid server block");
		return (std::make_pair(false, pos));
	}

	pos += key.length();
	while (std::isspace(line[pos]))
		pos++;
	return (std::make_pair(true, pos));
}

/* function that checks if [str] is a number. */
bool						isNumber (std::string str) {
	for (size_t i = 0; i < str.size(); i++) {
		if (!std::isdigit(str[i]))
			return (0);
	}
	return (1);
}

/* function that parses the value. ([pos] = position returned in the above ::skipKey()) */
std::string					parseValue (std::string line, size_t pos) {
	size_t	scPos = line.find(";", pos);
	return (line.substr(pos, scPos - pos));
}

/* function that converts a string into an int. */
int							strToInt (std::string str) {
	int					ret;
	std::stringstream	ssInt(str);

	ssInt >> ret;

	return (ret);
}
