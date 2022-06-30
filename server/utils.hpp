// #include <sys/types.h>
#include <sys/event.h> //kevent
// #include <sys/time.h>
#include <sys/socket.h>
#include <sys/stat.h> // struct stat
#include <arpa/inet.h> //sockaddr_in
#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits> //numeric_limits

#include <map>
#include <vector>
#include <string>
#include <cstring> //memset

# define CGI_BUF_SIZE 65536

enum	ErrorCode
{
	Continue = 100,
	OK = 200,
	Created = 201,
	No_Content = 204,
	Bad_Request = 400,
	Forbidden = 403,
	Not_Found = 404,
	Method_Not_Allowed = 405,
	Payload_Too_Large = 413,
	Internal_Server_error = 500
};

std::vector<std::string>	ft_split(std::string str)
{//일단 str을 받아서 공백을 기준으로 나누고 vector<std::string>에 넣어준다.
	char	charset = ' ';
	bool	pass = false;
	str += ' ';
	std::vector<std::string>	ret;
	std::string::size_type	start = 0;
	std::string::size_type	end = 0;
	std::string				substr = "";
	for (std::string::size_type i = 0; i < str.length(); i++, end++)
	{
		if (str[i] == charset)
		{//공백일 때 end에서 start를 뺀 만큼을 vector에 저장
			if (pass == false)
			{
				if (start != 0)
					start++;
				substr = str.substr(start, end - start);
				// substr += '\0';
				ret.push_back(substr);
				start = end;
				substr.clear();
			}
			pass = true;
		}
		else
		{
			pass = false;
			continue ;
		}
	}
	return (ret);
}
std::string	get_file_content(std::string request)
{//일단 간단하게 GET path형식만 받아서 작동하는지 확인한다.
	std::vector<std::string>	str_vec = ft_split(request);
	std::vector<std::string>::iterator	it = str_vec.begin();
	std::string	path;
	std::string	file_content = "";

	std::ifstream		file;
	std::stringstream	buffer;

	if (*it == static_cast<std::string>("GET"))
	{//GET method일 때
		it++;
		path = *it;
		path[strlen(path.c_str()) - 1] = '\0';
		std::cout << "GET METHOD START path is : " << path << "h" << std::endl;
		file.open(path.c_str(), std::ifstream::in);
		if (file.is_open() == false)
		{
			std::cout << "FILE OPEN ERROR\n";
			file_content += "FILE OPEN ERROR\n";
			return (file_content);
		}
		buffer << file.rdbuf();
		file_content = buffer.str();
		
		file.close();
	}
	else
	{
		file_content = request;
	}

	return (file_content);
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