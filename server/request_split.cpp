#include "../header/RequestHeader.hpp"

void	print_request_header(RequestHeader& request_header)
{
	std::cout << "Host: " << request_header._host << std::endl;
	std::cout << "User-Agent: " << request_header._user_agent << std::endl;
	std::cout << "Accept: " << request_header._accept << std::endl;
	std::cout << "Accept-Charset: " << request_header._accept_charset << std::endl;
	std::cout << "Accept-Lanugage: " << request_header._accept_language << std::endl;
	std::cout << "Accept-Encoding: " << request_header._accept_encoding << std::endl;
	std::cout << "Origin: " << request_header._origin << std::endl;
	std::cout << "Authorization: " << request_header._authorization << std::endl;
	std::cout << "Transfer-Encoding: " << request_header._transfer_encoding << std::endl;
	std::cout << "Content-Length: " << request_header._content_length << std::endl;
	std::cout << "Content-Type: " << request_header._content_type << std::endl;
	std::cout << "Content-Language: " << request_header._content_language << std::endl;
	std::cout << "Content-Location: " << request_header._content_location << std::endl;
	std::cout << "Content-Encoding: " << request_header._content_encoding << std::endl;
	std::cout << "Body: " << request_header._body << std::endl;
}

void	print_request_line(RequestHeader& request_header)
{
	write(1, "method: ", 8);
	write(1, request_header._method.c_str(), request_header._method.length());
	write(1, "\npath: ", 7);
	write(1, request_header._path.c_str(), request_header._path.length());
	printf("\nis http: %d\n", request_header._is_http);
}

int	set_request_line(std::string request_line, RequestHeader& request_header)
{
	std::vector<std::string>	request_line_vec = split(request_line, ' ');
	std::vector<std::string>::iterator	request_line_it = request_line_vec.begin();
	request_header._method = *request_line_it;
	//allow method가 아니면 종료되도록 하자
	//일단 간단하게 임의로 allow method를 초기화해서 확인
	if (request_header._method != "GET" && request_header._method != "POST"\
		&& request_header._method != "DELETE" && request_header._method != "PUT")
	{
		std::cout << "method is not allowed\n";
		return (1);
	}

	request_line_it++;
	request_header._path = *request_line_it;

	request_line_it++;
	if (*request_line_it != "HTTP/1.1")
	{
		request_header._is_http = false;
		std::cout << "http_version is not HTTP/1.1\n";
		return (1);
	}
	else
		request_header._is_http = true;
	//HTTP/1.1이 아니면 종료되도록 하자
	// print_request_line(request_header);
	return (0);
}

int	check_body(std::string request, RequestHeader& request_header, size_t r_pos)
{
	std::string	body = "";
	size_t			start = r_pos;
	if (r_pos == request.length() || r_pos == std::string::npos)
	{
		body = "";
	}
	else if ((r_pos = request.find('\r', start)) != std::string::npos)
	{
		if (r_pos == start)
		{
			if (request.at(r_pos + 1) == '\n')
			{
				std::cerr << "There are two empty line\n";
				return (1);
			}
		}
		body = request.substr(start, r_pos - start);
	}
	else
	{
		std::cerr << "body has not \\r\\n\n";
		return (1);
	}
	if (r_pos + 2 < request.length() && r_pos != std::string::npos)
	{
		std::cerr << "there is some data after body\n";
		return (1);
	}
	request_header._body = body;
	return (0);
}

int	check_header(std::vector<std::string> header, RequestHeader& request_header)
{
	for (std::vector<std::string>::iterator	it = header.begin() + 1;
		it != header.end(); it++)
	{
		size_t	n_pos = 0;
		std::string	value = "";
		if (strncmp((*it).c_str(), "Host: ", 6) == 0)
		{//Host의 값을 찾아서 값을 넣어준다.
			n_pos = (*it).find(" ");
			value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
			request_header._host = value;
			// std::cout << "Host: " << value << std::endl;
		}
		else if (strncmp((*it).c_str(), "User-Agent: ", 12) == 0)
		{
			n_pos = (*it).find(" ");
			value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
			request_header._user_agent = value;
			// std::cout << "User-Agent : " << value << std::endl;
		}
		else if (strncmp((*it).c_str(), "Accept: ", 8) == 0)
		{
			n_pos = (*it).find(" ");
			value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
			request_header._accept = value;
			// std::cout << "Accept : " << value << std::endl;
		}
		else if (strncmp((*it).c_str(), "Accept-Charset: ", 16) == 0)
		{
			n_pos = (*it).find(" ");
			value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
			request_header._accept_charset = value;
			// std::cout << "Accept-Charset : " << value << std::endl;
		}
		else if (strncmp((*it).c_str(), "Accept-Language: ", 17) == 0)
		{
			n_pos = (*it).find(" ");
			value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
			request_header._accept_language = value;
			// std::cout << "Accept-Language : " << value << std::endl;
		}
		else if (strncmp((*it).c_str(), "Accept-Encoding: ", 17) == 0)
		{
			n_pos = (*it).find(" ");
			value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
			request_header._accept_encoding = value;
			// std::cout << "Accept-Encoding : " << value << std::endl;
		}
		else if (strncmp((*it).c_str(), "Origin: ", 8) == 0)
		{
			n_pos = (*it).find(" ");
			value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
			request_header._origin = value;
			// std::cout << "Origin : " << value << std::endl;
		}
		else if (strncmp((*it).c_str(), "Authorization: ", 15) == 0)
		{
			n_pos = (*it).find(" ");
			value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
			request_header._authorization = value;
			// std::cout << "Authorization : " << value << std::endl;
		}
		else if (strncmp((*it).c_str(), "Transfer-Encoding: ", 19) == 0)
		{
			n_pos = (*it).find(" ");
			value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
			request_header._transfer_encoding = value;
			// std::cout << "Transfer-Encoding : " << value << std::endl;
		}
		else if (strncmp((*it).c_str(), "Content-Length: ", 16) == 0)
		{
			n_pos = (*it).find(" ");
			value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
			request_header._content_length = value;
			// std::cout << "Content-Length : " << value << std::endl;
		}
		else if (strncmp((*it).c_str(), "Content-Type: ", 14) == 0)
		{
			n_pos = (*it).find(" ");
			value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
			request_header._content_type = value;
			// std::cout << "Content-Type : " << value << std::endl;
		}
		else if (strncmp((*it).c_str(), "Content-Language: ", 18) == 0)
		{
			n_pos = (*it).find(" ");
			value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
			request_header._content_language = value;
			// std::cout << "Content-Language : " << value << std::endl;
		}
		else if (strncmp((*it).c_str(), "Content-Location: ", 18) == 0)
		{
			n_pos = (*it).find(" ");
			value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
			request_header._content_location = value;
			// std::cout << "Content-Location : " << value << std::endl;
		}
		else if (strncmp((*it).c_str(), "Content-Encoding: ", 18) == 0)
		{
			n_pos = (*it).find(" ");
			value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
			request_header._content_encoding = value;
			// std::cout << "Content-Encoding : " << value << std::endl;
		}
		else if (strncmp((*it).c_str(), "Location: ", 10) == 0)
		{//location도 응답 헤더
			n_pos = (*it).find(" ");
			value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
			request_header._location = value;
			// std::cout << "Location : " << value << std::endl;
		}
		else if (strncmp((*it).c_str(), "Last-Modified: ", 15) == 0)
		{//last-modified는 응답헤더
			n_pos = (*it).find(" ");
			value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
			request_header._last_modified = value;
			// std::cout << "Last-Modified : " << value << std::endl;
		}
		else if (strncmp((*it).c_str(), "Allow: ", 7) == 0)
		{//allow도 응답헤더
			n_pos = (*it).find(" ");
			value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
			request_header._allow = value;
			// std::cout << "Allow: " << value << std::endl;
		}
		else
		{//이상한 header값이므로 error 처리
			std::cout << "request header has strange value\n";
			return (1);
		}
	}
	return (0);
}

int	request_split(std::string request, RequestHeader& request_header)
{
	size_t	r_pos = 0;
	std::vector<std::string>	str_header;
	size_t	start = 0;
	std::string		str;
	std::map<std::string, std::string>	ret;
	std::string		body = "";
	while ((r_pos = request.find('\n', start)) != std::string::npos)
	{//\r을 계속 찾아서 그것을 기준으로 vector에 넣어주자.
		if (request.at(start) == '\r')
		{
			if (start + 1 == r_pos)
			{
				r_pos += 1;
				start = r_pos;
				break ;
			}
		}
		str = request.substr(start, r_pos - start - 1);
		str_header.push_back(str);
		r_pos += 1;
		start = r_pos;
	}
	// print_vec(str_header);

	if (check_body(request, request_header, r_pos) == 1)
	{
		return (1);
	}
	
	//request line을 parsing
	if (set_request_line(*(str_header).begin(), request_header) == 1)
	{
		std::cout << "request_line has error\n";
		return (1);
	}
	if (check_header(str_header, request_header) == 1)
	{
		return (1);
	}
	print_request_header(request_header);

	return (0);
}

int	main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cout << "please input test rn name\n";
		return (1);
	}
	RequestHeader	request_header;
	FILE*	file = fopen(argv[1], "r");
	fseek(file, 0, SEEK_END);
	size_t	file_size = ftell(file);
	char*	buf = new char[file_size + 1];
	memset(buf, 0, file_size + 1);
	fseek(file, 0, SEEK_SET);
	fread(buf, file_size, 1, file);
	
	std::string	str(buf);
	request_split(str, request_header);
	return (0);
}