#ifndef __UTILS_HPP__
# define __UTILS_HPP__

# include <iostream>
# include <vector>
# include <map>
# include <fstream>
# include <sstream>
# include <string>

# define DEFAULT_CONF	"./conf/default.conf"
# define NONE			0
# define EXACT			1
# define PREFERENTIAL	2
# define INVALID		3
# define ON				1
# define OFF			0

int							printErr (std::string errMsg);
std::vector<std::string>	split (std::string str, char delimiter = '\n');
std::vector<std::string>	splitServerBlocks (std::string block);
std::vector<std::string>	splitLocationBlocks (std::string block);
std::pair<bool, size_t>		skipKey (std::string line, std::string str);
bool						isNumber (std::string str);
std::string					parseValue (std::string line, size_t pos);
int							strToInt (std::string str);

#endif
