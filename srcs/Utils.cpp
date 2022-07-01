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

unsigned int	host_to_int(std::string host)
{
	size_t	sep = 0;
	unsigned int	n;
	size_t	start = 0;
	std::string	substr;
	unsigned int	ret = 0;
	if (host == "localhost")
		host = "127.0.0.1";
	for (int i = 3; i > -1; i--)
	{
		sep = host.find_first_of('.', sep);
		substr = host.substr(start, sep);
		n = std::atoi(substr.c_str());
		for (int j = 0; j < i; j++)
			n *= 256;
		ret += n;
		sep++; start = sep;
	}
	return (ret);
}

int	pathIsFile(const std::string& path)
{//파일이 REG(regular file)이면 1을 리턴하고 다른 경우에는 0을 리턴한다.
	struct stat	s;
	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFREG)
		{
			std::cout << "file is regular file\n";
			return (1);
		}
		std::cout << "file type is " << (s.st_mode & S_IFMT) << std::endl;
		std::cout << "file is not regular file\n";
	}
	
	return (0);
}

std::string	readHtml(const std::string& path, std::string* type)
{//path가 REGULAR file이면 open하여 파일 내용을 리턴한다.
//path가 REGULAR file이 아니면 error_html을 보낸다.
	std::ofstream			file;
	std::stringstream		buf;

	if (pathIsFile(path))
	{
		file.open(path.c_str(), std::ifstream::in);
		if (file.is_open() == false)
			return (ERROR_HTML);
		buf << file.rdbuf();
		file.close();
		*type = "text/html";
		return (buf.str());
	}
	else
		return (ERROR_HTML);
}

std::string	set_uri(const std::string& dirList, const std::string& dirName,
	const std::string& host, const int port)
{
	std::stringstream	ss;
	ss << "\t\t<p><a href=\"http://" + host + ":" <<\
		port << dirName + "/" + dirList + "\">" + dirList + "</a></p>\n";
	return (ss.str());
}

std::string	set_html(const std::string& path, const std::string& lang,
	const std::string& charset, const std::string& h1, const std::string& host, const int port)
{
	DIR*	dir = opendir(path.c_str());
	if (dir == NULL)
	{
		std::cerr << "Error: could not open " << path << std::endl;
		//dir을 열지 못했을 때 어떤 값을 리턴할 지 생각하자
		return "";
	}
	std::string	dirName(path);
	if (dirName[0] != '/')
		dirName = "/" + dirName;
	std::string	html = "<!DOCTYPE html>\n";
	html += "<html lang=\"" + lang + "\">\n" + \
	"\t<head>\n" + \
	"\t\t<meta charset=\"" + charset + "\">\n" + \
	"\t\t<title>" + dirName + "</title>\n" + \
	"\t</head>\n" + \
	"\t<body>\n" + \
	"\t\t<h1>" + h1 + "</h1>\n";
	struct dirent*	dirList;
	while ((dirList = readdir(dir)) != NULL)
		html += set_uri(std::string(dirList->d_name), dirName, host, port);
	html += "\t</body>\n</html>";
	closedir(dir);
	return (html);
}

int	set_error_page(const std::string& errPages, std::map<int, std::string>* errMap)
{
	std::vector<std::string>	err = split(errPages, ' ');
	//errPages를 공백으로 나눈 것을 저장
	std::string	html = *(--err.end());
	err.erase(err.end() - 1);
	if (html.find(".html") == std::string::npos)
	{
		std::cout << "There is no error page html\n";
		return (1);
	}
	for (std::vector<std::string>::iterator it = err.begin(); it != err.end(); it++)
	{
		if (isNumber(*it) == 0)
		{
			std::cout << "error code has problem\n";
			errMap->clear();
			return (1);
		}
		(*errMap)[atoi((*it).c_str())] = html;
	}
	return (0);
}