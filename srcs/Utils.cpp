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

std::string	intToStr(int code)
{
	std::stringstream	ret;
	ret << code;
	return (ret.str());
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

void	print_vec(std::vector<std::string> str_vec)
{
	for (std::vector<std::string>::iterator it = str_vec.begin();
		it != str_vec.end(); it++)
	{
		write(1, (*it).c_str(), (*it).length());
		write(1, "\n", 1);
	}
}

int	compare_end(const std::string& s1, const std::string& s2)
{//s1의 끝부분에 s2가 있다면 0을 리턴, s2가 없다면 1을 리턴
	size_t	s1_end = s1.size();
	size_t	s2_end = s2.size();
	while (s2_end > 0)
	{
		s1_end--; s2_end--;
		if (s1_end < 0 || s1[s1_end] != s2[s2_end])
			return (1);
	}
	return (0);
}

std::string	find_extension(std::string& file)
{//file의 확장자를 찾는다.
	size_t	extension_start = file.find_last_of('.');
	if (extension_start == std::string::npos)
		return ("");
	std::string	extension = file.substr(extension_start + 1, file.length() - extension_start - 1);
	return (extension);
}

std::string	find_file_name(const std::string& path)
{
	size_t	file_name_start = path.find_last_of('/');
	if (file_name_start == std::string::npos)
		return (path);
	std::string	file_name = path.substr(file_name_start + 1, path.length() - file_name_start - 1);
	return (file_name);
}

std::string	find_file_type(const std::string& file)
{
	if (file.length() == 1)
		return ("");
	size_t	file_type_start = file.find_first_of('.');
	if (file_type_start == std::string::npos)
		return ("");
	std::string	file_type = file.substr(file_type_start + 1, file.length() - file_type_start - 1);
	return (file_type);
}

std::string	erase_file_type(const std::string& file)
{
	if (file.length() == 1)
		return (file);
	size_t	file_type_start = file.find_first_of('.');
	if (file_type_start == std::string::npos)
		return (file);
	std::string	pure_file_name = file.substr(0, file_type_start);
	return (pure_file_name);
}

std::string	find_header_value(const std::string& header)
{
	size_t	colon_pos = header.find(":");
	if (colon_pos == std::string::npos)
		return ("");
	std::string	value = header.substr(colon_pos + 1, header.length() - colon_pos - 1);
	value = str_trim_char(value);
	return (value);
}

std::string	str_trim_char(const std::string& str, char delete_char)
{
	std::string	ret_str = str;
	size_t		ret_start = 0;
	size_t		ret_end = 0;

	if ((ret_start = ret_str.find_first_not_of(delete_char)) != std::string::npos)
		ret_str = ret_str.substr(ret_start, ret_str.length() - ret_start);
	else
		return ("");
	if ((ret_end = ret_str.find_last_not_of(delete_char)) != std::string::npos)
		ret_str = ret_str.substr(0, ret_end + 1);
	return (ret_str);
}

std::string	str_delete_rn(const std::string& str)
{
	std::string	ret_str(str);
	size_t		r_pos = 0;

	if ((r_pos = ret_str.find("\r\n")) != std::string::npos)
		ret_str = ret_str.substr(0, r_pos);
	return (ret_str);
}

int	isStrAlpha(const std::string& str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); it++)
	{
		if (std::isalpha(*it) == 0)
			return (0);
	}
	return (1);
}

int	isStrUpper(const std::string& str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); it++)
	{
		if (std::isupper(*it) == 0)
			return (0);
	}
	return (1);
}

int	make_html(const std::string& html_name, int code,
	const std::string& code_str, const std::string& server_name)
{
	std::ofstream	html_file;
	std::string		file_content;

	html_file.open(html_name);
	if (html_file.is_open() == false)
	{
		std::cerr << "HTML FILE MAKE ERROR\n";
		return (Internal_Server_Error);
	}
	file_content += "<html>\n"
	"<head><title>" + intToStr(code) + " " + code_str + "</title></head>\n"
	"<body>\n<center><h1>" + intToStr(code) + " " + code_str + "</h1></center>\n"
	"<hr><center>" + server_name + "</center>\n"
	"</body>\n"
	"</html>";
	html_file << file_content;
	html_file.close();
	return (0);
}

// int	main()
// {
// 	std::string	str1 = "SHOW1234";
// 	std::string	str2 = "AHOQ";
// 	if (isStrUpper(str1) == 1)
// 		std::cout << "str1 is alpha\n";
// 	else
// 		std::cout << "str1 is not alpha\n";
// 	if (isStrUpper(str2) == 1)
// 		std::cout << "str2 is alpha\n";
// 	else
// 		std::cout << "str2 is not alpha\n";

// }
