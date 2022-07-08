#ifndef REQUESTHEADER_HPP
# define REQUESTHEADER_HPP

# include "EntityHeader.hpp"

class RequestHeader : public EntityHeader
{//HTTP요청에서 사용되지만 메시지의 컨텐츠와 관련이 없는 패치될 리소스나 클라리언트 자체에 대한 자세한 정보를 포함하는 헤더
	public:
		RequestHeader() { this->resetHeader(); }
		RequestHeader(const RequestHeader& rh)
		{
			(void)rh;
			this->resetHeader();
		}
		virtual ~RequestHeader() {}

		RequestHeader&	operator=(const RequestHeader& rh)
		{ (void)rh; return (*this); }

		void	resetHeader()
		{
			this->_listen.host = 0;
			this->_listen.port = 0;
			this->_host = "";
			this->_user_agent = "";
			this->_accept = "";
			this->_accept_charset = "";
			this->_accept_language = "";
			this->_accept_encoding = "";
			this->_origin = "";
			this->_authorization = "";
			this->_content_length = "";
			this->_content_type = "";
			this->_content_language = "";
			this->_content_location = "";
			this->_content_encoding = "";
			this->_body = "";
		}
		
		void	print_request_header()
		{
			std::cout << "Host: " << this->_host << std::endl;
			std::cout << "User-Agent: " << this->_user_agent << std::endl;
			std::cout << "Accept: " << this->_accept << std::endl;
			std::cout << "Accept-Charset: " << this->_accept_charset << std::endl;
			std::cout << "Accept-Lanugage: " << this->_accept_language << std::endl;
			std::cout << "Accept-Encoding: " << this->_accept_encoding << std::endl;
			std::cout << "Origin: " << this->_origin << std::endl;
			std::cout << "Authorization: " << this->_authorization << std::endl;
			std::cout << "Content-Length: " << this->_content_length << std::endl;
			std::cout << "Content-Type: " << this->_content_type << std::endl;
			std::cout << "Content-Language: " << this->_content_language << std::endl;
			std::cout << "Content-Location: " << this->_content_location << std::endl;
			std::cout << "Content-Encoding: " << this->_content_encoding << std::endl;
			std::cout << "Body: " << this->_body << std::endl;
		}
		
		void	print_request_line()
		{
			write(1, "method: ", 8);
			write(1, this->_method.c_str(), this->_method.length());
			write(1, "\npath: ", 7);
			write(1, this->_path.c_str(), this->_path.length());
			std::cout << std::endl << "is http: " << this->_is_http << std::endl;
		}

		int	set_request_line(std::string request_line)
		{
			std::vector<std::string>	request_line_vec = split(request_line, ' ');
			//request_line에 빈칸이 없으면 그대로 request_line_vec에는 request_line이 그대로 들어간다.
			std::vector<std::string>::iterator	request_line_it = request_line_vec.begin();
			this->_method = *request_line_it;
			//allow method가 아니면 종료되도록 하자
			//일단 간단하게 임의로 allow method를 초기화해서 확인
			// print_vec(request_line_vec);
			if (this->_method != "GET" && this->_method != "POST"\
				&& this->_method != "DELETE" && this->_method != "PUT")
			{
				std::cerr << this->_method << " is not allowed\n";
				return (1);
			}
			if (request_line_vec.size() != 3)
			{
				std::cerr << "request line size is " << request_line_vec.size() << ", not 3\n";
				return (1);
			}

			request_line_it++;
			this->_path = *request_line_it;

			request_line_it++;
			if (*request_line_it != "HTTP/1.1")
			{
				this->_is_http = false;
				std::cerr << "http_version is not HTTP/1.1\n";
				return (1);
			}
			else
				this->_is_http = true;
			return (0);
		}
		int	check_body(std::string request, size_t r_pos)
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
			this->_body = body;
			return (0);
		}

		int	check_header(std::vector<std::string> header)
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
					this->_host = value;
					// std::cout << "Host: " << value << std::endl;
				}
				else if (strncmp((*it).c_str(), "User-Agent: ", 12) == 0)
				{
					n_pos = (*it).find(" ");
					value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
					this->_user_agent = value;
					// std::cout << "User-Agent : " << value << std::endl;
				}
				else if (strncmp((*it).c_str(), "Accept: ", 8) == 0)
				{
					n_pos = (*it).find(" ");
					value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
					this->_accept = value;
					// std::cout << "Accept : " << value << std::endl;
				}
				else if (strncmp((*it).c_str(), "Accept-Charset: ", 16) == 0)
				{
					n_pos = (*it).find(" ");
					value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
					this->_accept_charset = value;
					// std::cout << "Accept-Charset : " << value << std::endl;
				}
				else if (strncmp((*it).c_str(), "Accept-Language: ", 17) == 0)
				{
					n_pos = (*it).find(" ");
					value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
					this->_accept_language = value;
					// std::cout << "Accept-Language : " << value << std::endl;
				}
				else if (strncmp((*it).c_str(), "Accept-Encoding: ", 17) == 0)
				{
					n_pos = (*it).find(" ");
					value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
					this->_accept_encoding = value;
					// std::cout << "Accept-Encoding : " << value << std::endl;
				}
				else if (strncmp((*it).c_str(), "Origin: ", 8) == 0)
				{
					n_pos = (*it).find(" ");
					value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
					this->_origin = value;
					// std::cout << "Origin : " << value << std::endl;
				}
				else if (strncmp((*it).c_str(), "Authorization: ", 15) == 0)
				{
					n_pos = (*it).find(" ");
					value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
					this->_authorization = value;
					// std::cout << "Authorization : " << value << std::endl;
				}
				else if (strncmp((*it).c_str(), "Content-Length: ", 16) == 0)
				{
					n_pos = (*it).find(" ");
					value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
					this->_content_length = value;
					// std::cout << "Content-Length : " << value << std::endl;
				}
				else if (strncmp((*it).c_str(), "Content-Type: ", 14) == 0)
				{
					n_pos = (*it).find(" ");
					value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
					this->_content_type = value;
					// std::cout << "Content-Type : " << value << std::endl;
				}
				else if (strncmp((*it).c_str(), "Content-Language: ", 18) == 0)
				{
					n_pos = (*it).find(" ");
					value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
					this->_content_language = value;
					// std::cout << "Content-Language : " << value << std::endl;
				}
				else if (strncmp((*it).c_str(), "Content-Location: ", 18) == 0)
				{
					n_pos = (*it).find(" ");
					value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
					this->_content_location = value;
					// std::cout << "Content-Location : " << value << std::endl;
				}
				else if (strncmp((*it).c_str(), "Content-Encoding: ", 18) == 0)
				{
					n_pos = (*it).find(" ");
					value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
					this->_content_encoding = value;
					// std::cout << "Content-Encoding : " << value << std::endl;
				}
				else if (strncmp((*it).c_str(), "Allow: ", 7) == 0)
				{//allow도 응답헤더
					n_pos = (*it).find(" ");
					value = (*it).substr(n_pos + 1, (*it).length() - n_pos - 1);
					this->_allow = value;
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

		int	request_split(std::string request)
		{
			size_t	r_pos = 0;
			std::vector<std::string>	str_header;
			size_t	start = 0;
			std::string		str;
			std::map<std::string, std::string>	ret;
			std::string		body = "";
			std::cerr << "request===============" << request << std::endl;
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
			if (str_header.empty())
			{//요청이 한 줄만 왔을 때 set_request_line을 통해서 한 줄을 확인하고 종료
				if (set_request_line(request) == 0)
					return (0);
				std::cerr << "request header has one line but request header has error\n";
				return (1);
			}
			if (set_request_line(*(str_header).begin()) == 1)
			{
				std::cerr << "request_line has error\n";
				return (1);
			}
			if (check_body(request, r_pos) == 1)
				return (1);
			if (check_header(str_header) == 1)
				return (1);
			return (0);
		}
		
		int	set_listen(const std::string& str_host = "")
		{//에러가 발생하면 1을 리턴, 정상작동하면 0을 리턴
			if (str_host == "")
			{
				std::cerr << "host header is not exist\n";
				return (1);
			}
			std::vector<std::string>	host_port;
			unsigned int				host;
			int							port;
			host_port = split(str_host, ':');
			if (*host_port.begin() == str_host)
			{//포트는 생략할 수 있다. HTTP URL에서는 port default가 80이다.
				this->_listen.port = 80;
				if (str_host == "0.0.0.0")
				{
					this->_listen.host = 0;
					return (0);
				}
				if ((host = host_to_int(str_host)) == 0)
				{//str_host가 이상한 값을 가지고 있을 때
					return (1);
				}
				this->_listen.host = htonl(host);
				return (0);
			}

			if (isNumber(*(host_port.begin() + 1)) == 0)
			{
				std::cerr << "port is not number\n";
				return (1);
			}
			port = std::atoi((*(host_port.begin() + 1)).c_str());
			this->_listen.port = htons(port);

			if (*host_port.begin() == "0.0.0.0")
			{
				this->_listen.host = 0;
				return (0);
			}
			if ((host = host_to_int(*host_port.begin())) == 0)
			{
				return (1);
			}
			this->_listen.host = htonl(host);
			return (0);
		}

	public:
	//일단 임시로 public으로 바꾼다.
		t_listen	_listen;
		std::string	_host; //요청하려는 서버 호스트 이름과 포트 번호
		std::string	_user_agent; //현재 사용자가 어떤 클라리언트(운영체제, 브라우저 등)을 통해 요청을 보냈는지 알 수 있다
		std::string	_accept; //클라이언트가 허용할 수 있는 파일 형식(MIME TYPE)
		std::string	_accept_charset; //클라이언트가 지원가능한 문자열 인코딩 방식
		std::string	_accept_language; //클라이언트가 지원가능한 언어 나열
		std::string	_accept_encoding; //클라이언트가 해석가능한 압축 방식 지정
		std::string	_origin; //POST같은 요청을 보낼 때, 요청이 어느 주소에서 시작되었는지를 나타냄, 경로 정보는 포함하지 않고 서버 이름만 포함
		std::string	_authorization; //인증 토큰을 서버로 보낼 때 사용하는 헤더
		//형식 -> Authorization: <auth-scheme> <authorization-parameters>
		//basic authentication -> Authorization: Basic <credentials>

		//내가 만든 것 request line을 파시할 때 사용
		std::string	_method; //request method를 저장
		std::string	_path; //request의 path를 저장
		bool		_is_http; //HTTP/1.1인지 확인
		std::string	_body;

		/*
		//사용안할 것 같은 것
		std::string	_referer; //현재 페이지로 연결되는 링크가 있던 이전 웹 페이지의 주소
		std::string	_cookie; //Set-Cookie헤더와 함께 서버로부터 이전에 전송됐던 저장된 HTTP 쿠키 포함
		std::string	_if_modified_since; //여기에 쓰여진 시간 이후로 변경된 리소스를 취득하고 캐시가 만료되었을 때에만 데이터를 전송하는데 사용
		*/
};

#endif