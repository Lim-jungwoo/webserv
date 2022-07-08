#ifndef RESPONSE_HPP
# define RESPONSE_HPP

// # include "../header/ResponseHeader.hpp"
# include "../header/RequestHeader.hpp"

class Response
{
	public:
		Response() {}
		Response(const Response& response) { (void)response; }
		virtual ~Response() {}

		Response&	operator=(const Response& response) { (void)response; return (*this); }

		int	verify_method(std::string& request, int fd)
		{//body는 여기서 찾아서 인자로 보내주자
		//일단 간단하게 body는 path 뒤에 공백 다음
		//요청마다 header를 만들어야 하고 에러가 발생했을 때에 errormap을 적절히 불러와야 한다.
			//request의 method를 확인한다.
			std::vector<std::string>	str_vec = split(request, ' ');
			std::vector<std::string>::iterator	it = str_vec.begin();
			std::string	path;
			std::string	body;
		
			if (*it == static_cast<std::string>("GET"))
			{
				it++;
				return (getMethod(*it, fd));
			}
			else if (*it == static_cast<std::string>("POST"))
			{

			}
			else if (*it == static_cast<std::string>("DELETE"))
			{
				it++;
				return (deleteMethod(*it, fd));
			}
			else if (*it == static_cast<std::string>("PUT"))
			{
				it++;
				body = *(it + 1);
				return (putMethod(*it, fd, body));
			}
			else
			{
				std::cout << *it << " is no executable method\n";
			}
			return (0);
		}

		int	getMethod(std::string& path, int fd)
		{
			std::string			file_content = "";
			std::ifstream		file;
			std::stringstream	buffer;
			int					write_size;

			std::cout << "GET METHOD PATH IS " << path << std::endl;
			file.open(path.c_str(), std::ifstream::in);
			if (file.is_open() == false)
			{
				std::cout << "FILE OPEN ERROR\n";
				this->_code = Not_Found;
				return (1);
			}
			buffer << file.rdbuf();
			file_content = buffer.str();
			file.close();
			if ((write_size = ::send(fd, file_content.c_str(),
				file_content.size(), 0)) == -1)
			{
				std::cerr << "GET METHOD SEND ERROR\n";
				this->_code = Internal_Server_error;
				return (1);
			}
			this->_code = OK;
			return (0);
		}
		int	postMethod(std::string& path, int fd)
		{
			(void)path; (void)fd;
			return (0);
		}
		int	putMethod(std::string& path, int fd, std::string& body)
		{
			//ofstream형은 open mode를 따로 작성하지 않으면
			//기존에 파일이 존재했다면 삭제하는 trunc, 출력 전용을 뜻하는 out모드로 open된다.
			(void)fd;
			std::ofstream	file;
			if (pathIsFile(path))
			{//file이 이미 존재하고 regular file일 때 파일을 갱신한다.
				std::cout << "put file is already exist\n";
				//이렇게만 작성해도 기존에 파일의 내용은 삭제된다.
				file.open(path.c_str());
				file << body;
				file.close();
				this->_code = OK;
				return (0);
			}
			else
			{//file이 존재하지 않거나, file이 regular file이 아닐 때 작동
				file.open(path.c_str());
				if (file.is_open() == false)
				{
					this->_code = Not_Found;
					return (1);
					// return (403);
				}
				file << body;
				file.close();
			}
			this->_code = Created;
			// return (201);
			return (0);
		}
		int	deleteMethod(std::string& path, int fd)
		{//일단 delete가 제대로 되지 않을 때는 생각하지 않고 무조건 0을 리턴하는 것으로 했다.
			(void)fd;
			if (pathIsFile(path))
			{
				if (remove(path.c_str()) == 0)
					this->_code = OK;
				else
				{
					std::cerr << "file is regular file but remove fail\n";
					this->_code = Forbidden;
				}
			}
			else
			{
				std::cerr << "file is not regular file so remove fail\n";
				this->_code = 404;
			}
			// if (_code == 403 || _code == 404)
			// {
			// 	_response = this->readHtml(_errorMap[_code]);
			// }
			//헤더를 만들어서 저장
			return (0);
		}

	private:
		int	_code;
};

#endif