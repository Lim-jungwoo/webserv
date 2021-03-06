#ifndef SERVER_HPP
# define SERVER_HPP

// # include "../cgi/cgi.hpp"
# include "../response/Response.hpp"

class Server
{
	public:
	// private:
		struct sockaddr_in			_server_addr; //server의 주소를 저장
		int							_server_socket; //server_socket의 fd를 저장
		t_listen					_listen; //listen할 host와 port를 저장
		int							_kq; //kqueue를 통해 받은 fd를 저장
		std::map<int, std::string>	_request; //request의 socket과 socket의 내용을 저장
		std::vector<struct kevent>	_change_list; //kevent를 모두 저장
		struct kevent				_event_list[LISTEN_BUFFER_SIZE]; //이벤트가 발생한 kevent를 저장


		std::map<int, Response>					_response;
		std::map<int, int>							_body_condition;
		std::map<int, int>							_request_end;
		std::map<int, bool>						_is_check_request_line;
		std::map<int, size_t>						_body_start_pos;
		std::map<int, int>							_body_end;
		std::map<int, size_t>						_body_vec_size;
		std::map<int, size_t>						_body_vec_total_size;
		std::map<int, size_t>						_body_vec_start_pos;
		std::map<int, size_t>						_rn_pos;
		std::map<int, size_t>						test_body_size;

		std::string					_server_root;

		//configuation file 관련
		size_t						_client_max_body_size;
		int							_auto_index;
		std::vector<std::string>	_index;
		std::string					_config_cgi;

		std::vector<LocationBlock>				_locations;
		Cgi							_cgi;

	public:
		Server();
		Server(const Server& server);
		~Server();

		Server&	operator=(const Server& server);

		//인자로 받은 값들을 EV_SET을 이용해 kevent구조체 변수인 temp event를 초기화시키고,
		//change_list에 temp event를 추가한다.
		void	change_events(std::vector<struct kevent>& change_list, uintptr_t ident,
			int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void* udata);


		void	disconnect_request(int request_fd);

		void	check_connection(int request_fd);

		int	init_listen(const std::string& host_port);

		int	init_server_socket();

		int	event_error(int fd);

		void	request_accept();

		void	setCgiEnv(int fd)
		{
			_cgi.setBody(_response[fd]._body);
			std::cout << BLUE << "CGI BODY LENGTH: " << _cgi.getBody().length() << std::endl;
			// _cgi.setEnv("CONTENT_LENGTH", _response[fd].getContentLength());
			_cgi.setEnv("CONTENT_LENGTH", intToStr(_cgi.getBody().length()));
			std::cout << "CGI CONTENT_LENGTH: " << _cgi.getEnv()["CONTENT_LENGTH"] << std::endl;
			_cgi.setEnv("PATH_INFO", _response[fd]._path);
			std::cout << "CGI PATH_INFO: " << _cgi.getEnv()["PATH_INFO"] << std::endl;
			_cgi.setEnv("SERVER_PORT", "8000");
			_cgi.setEnv("SERVER_PROTOCOL", "HTTP/1.1");
			this->_cgi.setEnv("REDIRECT_STATUS", "200");
			if (_response[fd]._x_header != "")
			{
				_cgi.setEnv("HTTP_X_SECRET_HEADER_FOR_TEST", _response[fd]._x_header);
				std::cout << RED << "HTTP_X_SECRET_HEADER_FOR_TEST: " << _cgi.getEnv()["X_SECRET_HEADER_FOR_TEST"] << RESET << std::endl;
			}
		}

		void	initCgiEnv(int fd)
		{
			this->_cgi.setName(_config_cgi);
			//header에 Auth-Scheme가 존재한다면 AUTH_TYPE환경변수에 header의 Authorization값을 집어넣는다.
			//AUTH_TYPE은 인증 타입.
			//만약 client가 액세스에 인증이 필요한 경우, Request Authorization header 필드의 auth-scheme 토큰을 바탕으로 값을 세팅해야 한다.
			// if (headers.find("Auth-Scheme") != headers.end() && headers["Auth-Scheme"] != "")
			// 	this->_env["AUTH_TYPE"] = headers["Authorization"];
			this->_cgi.setEnv("AUTH_TYPE", "");

			//원하는 status를 지정하여 PHP가 처리할 수 있는 status를 정한다.
			//php-cgi가 200을 처리할 수 있도록 정했다.
			this->_cgi.setEnv("REDIRECT_STATUS", "200");

			//request body의 길이, request body가 없으면 NULL값으로 세팅한다.
			//request body가 있을 때만 세팅되어야 하며, transfer-encoding이나 content-coding을 제거한 후의 값으로 세팅되어야 한다.
			this->_cgi.setEnv("CONTENT_LENGTH", this->_response[fd].getContentLength());
			//request body의 mime.type을 세팅한다.
			//만약 request에 CONTENT_TYPE헤더가 존재한다면 무조건 세팅해야 한다.
			//CONTENT_TYPE헤더가 존재하지 않으면 올바른 CONTENT_TYPE을 추론하거나, CONTENT_TYPE을 생략해야 한다.
			// this->_cgi.setEnv("CONTENT_TYPE", headers["Content-Type"]);
			this->_cgi.setEnv("CONTENT_TYPE", this->_response[fd].getContentType());

			//서버의 CGI타입과 개정레벨을 나타난다.
			//형식 CGI/revision
			// this->_cgi.setEnv("GATEWAY_INTERFACE", "CGI/1.1");

			//client에 의해 전달되는 추가 PATH정보
			//PATH_INFO를 이용하여 스크립트를 부를 수 있다.
			this->_cgi.setEnv("PATH_INFO", this->_response[fd].getPath());
			//PATH_INFO에 나타난 가상 경로(path)를 실제의 물리적인 경로로 바꾼 값
			this->_cgi.setEnv("PATH_TRANSLATED", "");

			//GET방식에서 URL의 뒤에 나오는 정보를 저장하거나 폼입력 정보를 저장한다.
			//POST방식은 제외
			this->_cgi.setEnv("QUERY_STRING", "");

			//client의 IP주소
			this->_cgi.setEnv("REMOTEaddr", "");
			//client의 호스트 이름
			// this->_cgi.setEnv("REMOTED_IDENT", headers["Authorization"]);
			this->_cgi.setEnv("REMOTE_IDENT", "");
			//서버가 사용자 인증을 지원하고, 스크립트가 확인을 요청한다면, 이것이 확인된 사용자 이름이 된다.
			// this->_cgi.setEnv("REMOTE_USER", headers["Authorization"]);
			this->_cgi.setEnv("REMOTE_USER", "");

			//GET, POST, PUT과 같은 method
			this->_cgi.setEnv("REQUEST_METHOD", this->_response[fd]._method);
			//쿼리까지 포함한 모든 주소
			//ex) http://localhost:8000/cgi-bin/ping.sh?var1=value1&var2=with%20percent%20encodig
			this->_cgi.setEnv("REQUEST_URI", "");

			//실제 스크립트 위치
			//ex) http://localhost:8000/cgi-bin/ping.sh?var1=value1&var2=with%20percent%20encodig의
			// /cgi-bin/ping.sh를 뜻한다.
			//웹에서의 스크립트 경로를 뜻한다.
			this->_cgi.setEnv("SCRIPT_NAME", "");
			//서버(로컬)에서의 스크립트 경로를 뜻한다.
			//ex) "C:/Program Files (x86)/Apache Software Foundation/Apache2.2/cgi-bin/ping.sh"
			this->_cgi.setEnv("SCRIPT_FILENAME", "");

			//서버의 호스트 이름과 DNS alias 혹은 IP주소
			// if (headers.find("Hostname") != headers.end())
			// 	this->_cgi.setEnv("SERVER_NAME", headers["Hostname"]);
			// else
			// 	this->_cgi.setEnv("SERVER_NAME", this->_cgi.setEnv("REMOTEaddr",			this->_cgi.setEnv("SERVER_NAME", this->_response[fd].getServer());
			//client request를 보내는 포트 번호
			this->_cgi.setEnv("SERVER_PORT", intToStr(this->_response[fd].getListen().port));
			//client request가 사용하는 프로토콜
			this->_cgi.setEnv("SERVER_PROTOCOL", this->_response[fd].getHttpVersion());
			// std::cout << GREEN << "cgi server version: " << this->_cgi.getEnv()["SERVER_PROTOCOL"] << RESET << std::endl;
			//웹서버의 이름과 버전을 나타낸다.
			// 형식: 이름/버전
			// this->_cgi.setEnv("SERVER_SOFTWARE", "Weebserv/1.0");
			
			// this->_cgi.setEnv(insert(config.,CgiParam().begin(), config.getCgiParam().end()));
		}

		LocationBlock				selectLocationBlock (std::string requestURI, int fd)
		{
			std::vector<LocationBlock>	locationBlocks;
			std::vector<std::string>	requestURIvec;
			size_t						max = 0,ret = 0;

			// first, split the request URI by '/'
			std::cout << "requestURI : " << requestURI << std::endl;
			requestURIvec = split(requestURI, '/');

			// add '/' to each of the string in the vector
			for (size_t i = 1; i < requestURIvec.size(); i++)
				requestURIvec[i] = "/" + requestURIvec[i];

			// look for locations whose modifier is '='; the request URI must match the location's URI
			for (size_t i = 0; i < _locations.size(); i++) {
				if (_locations[i].getMod() == EXACT) {
					if (_locations[i].getURI() == requestURI)
					// if (requestURI.find(_locations[i].getURI()) != std::string::npos)
						return (_locations[i]);
					else
						break ;
				}
			}
			std::cout << BLUE << "before select location path : " << _response[fd]._path << RESET << std::endl;
			// look for locations who don't have modifiers or '~'; the location's URI must contain the request URI
			for (size_t i = 0; i < _locations.size(); i++)
			{
				if (_locations[i].getMod() == NONE || _locations[i].getMod() == PREFERENTIAL)
				{
					std::cout << "locations uri : " << _locations[i].getURI() << ", requestURI first : " << requestURIvec[0] << std::endl;
						// first, check if the two URIs match
					if (_locations[i].getURI() == requestURIvec[0])
					{
						std::string	change_path = "";
						if (requestURI.at(requestURI.length() - 1) == '/')
							requestURI.substr(0, requestURI.length() - 1);
						size_t	first_slash_pos = requestURI.find_first_of("/");
						if (first_slash_pos != std::string::npos)
							change_path = requestURI.substr(first_slash_pos, requestURI.length() - first_slash_pos);
						_locations[i].setPath(_locations[i].getRoot() + change_path);
						locationBlocks.push_back(_locations[i]);
					}
					// then, check for the level of the request URI
					else if (requestURIvec.size() == 1)
					{
						// std::string	change_path = "";
						// if (requestURI.at(requestURI.length() - 1) == '/')
						// 	requestURI.substr(0, requestURI.length() - 1);
						// size_t	last_slash_pos = requestURI.find_last_of("/");
						// if (last_slash_pos != std::string::npos)
						// 	change_path = requestURI.substr(last_slash_pos, requestURI.length() - last_slash_pos);
						// _locations[i].setPath(_locations[i].getRoot() + change_path);
						// locationBlocks.push_back(_locations[i]);
					}
					if (requestURIvec.size() >= 2)
					{
						// if the request URI has more than one slashes (nested), get the nested locations and compare their URIs
						std::cout << CYAN << "request uri : " << requestURI << ", location uri: " << _locations[i].getURI() << ";;\n" << RESET;
						std::vector<LocationBlock>	nested = _locations[i].getLocationBlocks();
						for (size_t j = 0; j < nested.size(); j++)
						{
							std::cout << BLUE << "nested uri: " << nested[j].getURI() << RESET << std::endl;
							if (!compareURIsWithWildcard(nested[j].getURI(), requestURIvec[1], nested[j].getMod()))
							{
								std::string	change_path = "";
								size_t	last_slash_pos = requestURI.find_last_of("/");
								if (last_slash_pos != std::string::npos)
									change_path = requestURI.substr(last_slash_pos, requestURI.length() - last_slash_pos);
								nested[j].setPath(nested[j].getRoot() + change_path);
								locationBlocks.push_back(nested[j]);
							}
						}
						if (requestURI.find(_locations[i].getURI()) != std::string::npos)
						{
							std::string	change_path = "";
							size_t	first_slash_pos = requestURI.find_first_of("/");
							if (first_slash_pos != std::string::npos)
								change_path = requestURI.substr(first_slash_pos, requestURI.length() - first_slash_pos);
							_locations[i].setPath(_locations[i].getRoot() + change_path);
							locationBlocks.push_back(_locations[i]);
						}
					}
				}
			}

			// if no location was found, return empty location
			if (locationBlocks.empty())
			{
				std::cout << PINK << "no location was found so return empty location" << RESET << std::endl;
				return (LocationBlock());
			}

			// if there are more than one locations selected, return the location with longest URI
			max = locationBlocks[ret].getURI().length();
			for (size_t i = 1; i < locationBlocks.size(); i++) {
				if (locationBlocks[i].getURI().size() > max) {
					max = locationBlocks[i].getURI().length();
					ret = i;
				}
			}

			for (std::vector<LocationBlock>::iterator it = locationBlocks.begin();
				it != locationBlocks.end(); it++)
			{
				std::cout << CYAN << "location block path: " << (*it).getPath() << RESET << std::endl;
			}
			std::cout << PINK << "ret: "<< ret << ", after select location path : " << locationBlocks[ret].getPath();
			std::cout << ", select location uri: " << locationBlocks[ret].getURI() << RESET << std::endl;

			return (locationBlocks[ret]);
		}

		void	locationToServer(LocationBlock location_block, int fd)
		{
			if (location_block.getIsEmpty() == true)
				return ;
			if (location_block.getURI() != "" && (_response[fd]._method == "GET" ||
				(_response[fd]._method != "GET" && _response[fd]._path != "/")))
				_response[fd]._path = location_block.getPath();
			if (location_block.getClntSize() != 0)
			{
				_client_max_body_size = location_block.getClntSize();
				std::cout << "client max body size: " << _client_max_body_size << std::endl;
			}
			if (location_block.getMethods().empty() == false)
				_response[fd].initAllowMethod(location_block.getMethods());

			std::cout << GREEN << "location block root: " << location_block.getRoot() << RESET << std::endl;
			if (location_block.getRoot() != ".")
			{
				this->_response[fd]._root = location_block.getRoot();
				size_t	uri_pos = 0;
				if ((uri_pos = this->_response[fd].getPath().find(location_block.getURI())) != std::string::npos)
				{
					std::cout << CYAN << "location uri: " << location_block.getURI() << RESET << std::endl;
					// this->_response[fd].setPath(this->_response[fd].getPath().substr(uri_pos + location_block.getURI().length(),
					// 	this->_response[fd].getPath().length() - uri_pos - location_block.getURI().length()));
					// this->_response[fd].setPath(this->_response[fd].getRoot() + this->_response[fd].getPath());
					this->_response[fd].setPath(location_block.getPath());
					std::cout << RED << "select location and path change : " << this->_response[fd].getPath() << RESET << std::endl;
				}
			}
			if (location_block.getAutoindex() != DEFAULT_AUTOINDEX)
				_auto_index = location_block.getAutoindex();
			if (location_block.getIndex().empty() == false)
				_index = location_block.getIndex();
			if (location_block.getCGI() != "")
			{
				this->_config_cgi = location_block.getCGI();
				std::cout << YELLOW << "cgi: " << this->_config_cgi << RESET << std::endl;
				this->_cgi.setCgiExist(true);
				this->initCgiEnv(fd);
			}
		}

		void	event_read(int fd)
		{//fd에 맞게 실행된다.
		//저장되어있는 request에 맞는 fd가 없다면 아무 행동도 하지 않는다.
			_request_end[fd] = 0;
			std::cout << "EVENT READ WITH FD [" << fd << "]" << std::endl;
			int	check_request_line_ret = 0;
			if (fd == _server_socket)
			// if (fd == static_cast<uintptr_t>(_server_socket))
			{//read event인데 server socket일 때는 accept이라는 뜻이므로 request_accept을 실행
				request_accept();
			}
			else if (_request.find(fd) != _request.end())
			{//일단 1024만큼만 읽는다.
				// std::cout << "read start\n";
				char	buf[READ_BUFFER_SIZE] = {};
				int	n;
				if (_response[fd]._content_length != "" && _body_condition[fd] == Body_Start)
				{//저장해놓은 content_length만큼만 받도록 한다.
					if (_response[fd]._body_size >= _client_max_body_size && _client_max_body_size != 0)
					{
						std::cerr << "content length is too big to receive\n";
						_response[fd].setCode(Payload_Too_Large);
						return ;
					}
				}
				n = ::recv(fd, buf, READ_BUFFER_SIZE - 1, 0);
				buf[n] = '\0';
				_request[fd] += buf;
				if (_response[fd]._body_size != 0
					&& _request[fd].length() - _body_start_pos[fd] >= _response[fd]._body_size)
				{//body size만큼 입력 받았을 때
					std::cout << PINK << "body size: " << _response[fd]._body_size << ", body start pos: " << _body_start_pos[fd] << RESET << std::endl;
					_request_end[fd] = 1;
					_request[fd] = _request[fd].substr(0, _body_start_pos[fd] + _response[fd]._body_size);
				}
				
				if (_request[fd] == "\r\n" && _is_check_request_line[fd] == 0)
				{//아무 값도 없이 빈 칸만 왔다는 뜻
					_request[fd].clear();
					std::cout << "request is empty line\n";
					return ;
				}
				if (_request[fd].find("\r\n") != std::string::npos &&
					_is_check_request_line[fd] == 0)
				{
					if (_request[fd].at(0) == 'r' && _request[fd].at(1) == '\n')
						_request[fd] = _request[fd].substr(2, _request[fd].length() - 2);
					if (_request[fd].at(0) == '\n')
						_request[fd] = _request[fd].substr(1, _request[fd].length() - 1);
					_is_check_request_line[fd] = 1;
					std::cout << "check request line\n";
					
					check_request_line_ret = _response[fd].check_request_line(_request[fd]);
					if (check_request_line_ret == 9)
					{
						std::cout << "request's http version is HTTP/0.9\n";
						_request_end[fd] = 1;
					}
					else if (check_request_line_ret == 1)
					{
						std::cerr << "check request line error\n";
						std::cout << RED << "@@@@@@@@request@@@@@@\n" << _request[fd] << RESET << std::endl;
						_response[fd]._http_version = "HTTP/1.1";
						_response[fd]._content_location = "/";
						_response[fd].setCode(Bad_Request);
						return ;
					}
					//check_request_line으로 parsing한 path를 통해서 select location block으로 적절한 location block을 찾는다.
					//path이 root랑 다를 때 Location block의 변수들을 Server로 넘겨준다.
					//만약 location을 못 찾았다면 그냥 server block의 변수를 사용하면 된다.
					if (this->_response[fd]._path != this->_response[fd]._root && this->_response[fd]._path != "/")
					{
						std::cout << PINK << "@@@@@@@@@@@@@select location@@@@@@@@@@@@" << RESET << std::endl;
						std::cout << "response root: " << _response[fd]._root << ", response path: " << _response[fd]._path << std::endl;
						LocationBlock	test = this->selectLocationBlock(this->_response[fd]._path, fd);
						if (test.getIsEmpty() == false)
						{//값을 가지고 있는 location block을 찾았을 경우에 이 location block의 변수들을 모두 넣어주면 된다.
							this->_response[fd].setPath(test.getPath());
							this->locationToServer(test, fd);
						}
						else
						{
							std::cout << CYAN << "path: " << this->_response[fd].getPath() << std::endl;
							std::cout << "path last: " << this->_response[fd].getPath().at(this->_response[fd].getPath().length() - 1) << std::endl;
							if (this->_response[fd].getPath().at(this->_response[fd].getPath().length() - 1) == '/')
								this->_response[fd].setPath(this->_response[fd].getPath().substr(0, this->_response[fd].getPath().length() - 1));
							if (this->_response[fd].getPath().length() != 0 && this->_response[fd].getPath().at(0) == '/')
								this->_response[fd].setPath(this->_response[fd].getRoot() + this->_response[fd].getPath());
							else if (this->_response[fd].getPath().length() != 0)
								this->_response[fd].setPath(this->_response[fd].getRoot() + "/" + this->_response[fd].getPath());
							else
								this->_response[fd].setPath(this->_response[fd].getRoot() + this->_response[fd].getPath());
							std::cout << "path: " << this->_response[fd].getPath() << RESET << std::endl;
						}
					}
					else if ((this->_response[fd].getPath() == "/" && this->_response[fd]._method == "GET") ||
						this->_response[fd].getPath() != "/")
					{
						std::cout << RED << "path: " << this->_response[fd].getPath() << std::endl;
						std::cout << "path last: " << this->_response[fd].getPath().at(this->_response[fd].getPath().length() - 1) << std::endl;
						if (this->_response[fd].getPath().at(this->_response[fd].getPath().length() - 1) == '/')
							this->_response[fd].setPath(this->_response[fd].getPath().substr(0, this->_response[fd].getPath().length() - 1));
						if (this->_response[fd].getPath().length() != 0 && this->_response[fd].getPath().at(0) == '/')
							this->_response[fd].setPath(this->_response[fd].getRoot() + this->_response[fd].getPath());
						else if (this->_response[fd].getPath().length() != 0)
							this->_response[fd].setPath(this->_response[fd].getRoot() + "/" + this->_response[fd].getPath());
						else
							this->_response[fd].setPath(this->_response[fd].getRoot() + this->_response[fd].getPath());
						std::cout << "path: " << this->_response[fd].getPath() << RESET << std::endl;
					}
				}

				if ((strncmp(this->_request[fd].c_str(), "POST", 4) == 0 ||
					strncmp(this->_request[fd].c_str(), "PUT", 3) == 0) && this->_body_condition[fd] == No_Body)
					this->_body_condition[fd] = Body_Exist;

				size_t	rnrn_pos;
				if (this->_request[fd].empty() == false && (rnrn_pos = this->_request[fd].find("\r\n\r\n")) != std::string::npos)
				{
					if (this->_body_condition[fd] == No_Body)
					{
						this->_request_end[fd] = 1;
						this->_body_condition[fd] = Body_End;
					}
					else if (this->_body_condition[fd] == Body_Exist)
					{
						this->_body_condition[fd] = Body_Start;
						this->_body_start_pos[fd] = rnrn_pos + 4;
					}
				}

				//check header
				if ((_body_condition[fd] == Body_Start || _body_condition[fd] == Body_End)
					&& _body_end[fd] == 0)
				{
					if (check_request_line_ret == 1 || check_request_line_ret == 9)
						;
					else if (_response[fd].request_split(_request[fd], _body_condition[fd]) == 1)
					{
						std::cerr << "request split error\n";
						_response[fd].setCode(Bad_Request);
					}
					_body_end[fd] = 1;
				}

				if (this->_response[fd]._body_size != 0 && this->_response[fd]._content_length != ""
					&& this->_request[fd].length() - this->_body_start_pos[fd] >= this->_response[fd]._body_size)
				{//body size만큼 입력 받았을 때
					std::cout << PINK << "body size: " << _response[fd]._body_size << ", body start pos: " << _body_start_pos[fd] << RESET << std::endl;
					this->_request_end[fd] = 1;
					this->_request[fd] = this->_request[fd].substr(0, this->_body_start_pos[fd] + this->_response[fd]._body_size);
				}
				if (this->_response[fd]._transfer_encoding == "chunked")
				{//content_length가 없고, transfer_encoding이 chunked일 때 파일의 끝을 알리는 것이 들어올 때까지 계속 recv
					while (_rn_pos[fd] < this->_request[fd].length() && this->_response[fd]._path != "/")
					{
						if (_rn_pos[fd] <= this->_body_start_pos[fd])
						{
							_rn_pos[fd] = this->_body_start_pos[fd];
							_body_vec_start_pos[fd] = _rn_pos[fd];
						}
						size_t	body_size = this->_request[fd].find("\r\n", _rn_pos[fd]);
						size_t	end_request = this->_request[fd].find("0\r\n\r\n");
						std::cout << RED << "rn_pos: " << _rn_pos[fd] << RESET << std::endl;
						std::cout << BLUE << "end_request : " << end_request << ", body_size: " << body_size << RESET << std::endl;

						if (body_size != std::string::npos && this->_body_vec_size[fd] == 0)
						{
							std::string	body_size_str = this->_request[fd].substr(_rn_pos[fd],
								body_size - _rn_pos[fd]);
							this->_body_vec_size[fd] = hexToDecimal(body_size_str);
							std::cout << YELLOW << "body vec size: " << _body_vec_size[fd] << RESET << std::endl;
							_body_vec_total_size[fd] += this->_body_vec_size[fd];
							if (_body_vec_total_size[fd] > _client_max_body_size && _client_max_body_size != 0)
								_response[fd].setCode(Payload_Too_Large);
							this->_body_vec_start_pos[fd] = body_size + 2;
							_rn_pos[fd] = _body_vec_start_pos[fd] + this->_body_vec_size[fd] + 2;
						}
						else if (body_size == std::string::npos)
						{
							std::cout << CYAN << "there is no left body size\n" << RESET;
							break ;
						}
						if (this->_request[fd].length() > this->_body_vec_start_pos[fd] + this->_body_vec_size[fd] &&
							this->_body_vec_start_pos[fd] != 0)
						{
							std::string	_body_element = this->_request[fd].substr(this->_body_vec_start_pos[fd],
								this->_body_vec_size[fd]);
							this->_response[fd]._body_vec.push_back(_body_element);
							this->_body_vec_start_pos[fd] += this->_body_vec_size[fd];
							this->_body_vec_size[fd] = 0;
						}
					}

					if (this->_request[fd].find("0\r\n\r\n") != std::string::npos &&
						(this->_body_vec_start_pos[fd] == 0 ||
						(this->_body_vec_start_pos[fd] != 0 &&
						this->_body_vec_size[fd] == 0)))
					{
						this->_request[fd] = this->_request[fd].substr(0, this->_request[fd].length() - 5);
						std::cout << PINK << "receive file end\n" << RESET << std::endl;
						this->_request_end[fd] = 1;
					}
					
				}
				if (_request[fd].length() > 100)
				{
					// std::cout << GREEN << "=======request======\n" << _request[fd].substr(0, 100) << "..." << _request[fd].substr(_request[fd].length() - 10, 10) << RESET << std::endl;
					std::cout << YELLOW << "request fd length: " << _request[fd].length() << RESET << std::endl;
				}

				if (n <= 0)
				{//read가 에러가 났거나, request가 0을 보내면 request와 연결을 끊는다.
					if (n < 0)
					{
						std::cerr << "request read error\n";
						_response[fd].setCode(Internal_Server_Error);
					}
					else
						disconnect_request(fd);
				}
				else if (_request_end[fd] == 1)
				{
					std::cout << RED << "#######request end!!!!!\n" << RESET << std::endl;
					if (this->_body_start_pos[fd] != 0 && this->_body_start_pos[fd] < this->_request[fd].length())
					{
						if (this->_response[fd]._transfer_encoding == "chunked" &&
							this->_response[fd]._body_vec.empty() == false)
						{
							for (std::vector<std::string>::iterator it = _response[fd]._body_vec.begin();
								it != _response[fd]._body_vec.end(); it++)
							{
								this->_response[fd]._body += *it;
								test_body_size[fd] += (*it).length();
							}
							// if (test_body_size[fd] > _client_max_body_size && _client_max_body_size != 0)
							// 	_response[fd].setCode(Payload_Too_Large);
							_response[fd]._content_length = sizetToStr(test_body_size[fd]);
							std::cout << GREEN << "test body size: " << test_body_size[fd] << RESET << std::endl;
							setCgiEnv(fd);
						}
						else
						{
							this->_response[fd]._body = this->_request[fd].substr(this->_body_start_pos[fd],
								this->_request[fd].length() - this->_body_start_pos[fd]);
							if (_response[fd]._body.length() > _client_max_body_size)
								_response[fd].setCode(Payload_Too_Large);
						}
					}
				}
			}
		}

		void	event_write(int fd)
		{
			// std::cout << "EVENT WRITE WITH FD [" << fd << "]" << std::endl;
			if (_request.find(fd) != _request.end())
			{
				if (this->_request_end[fd] == 1)
				{
					// std::cout << RED << "EVENT WRITE WITH FD [" << fd << "]" << std::endl;
					std::cout << GREEN << "=======request======\n" << _request[fd] << RESET << std::endl;
					// if (_request[fd].length() > 100)
					// 	std::cout << GREEN << "=======request======\n" << _request[fd].substr(0, 100) << "..." << _request[fd].substr(_request[fd].length() - 10, 10) << RESET << std::endl;
					int	verify_method_ret;
					verify_method_ret = _response[fd].verify_method(fd, &_response[fd], _request_end[fd], _cgi);
					if (verify_method_ret == 1)
						disconnect_request(fd);
					if (verify_method_ret == 2)
					{
						// disconnect_request(fd);
						_request[fd].clear();
						_request_end[fd] = 0;
						_body_condition[fd] = No_Body;
						_response[fd].initRequest();
						_is_check_request_line[fd] = 0;
						_body_start_pos[fd] = 0;
						_body_end[fd] = 0;
						this->_body_vec_size[fd] = 0;
						this->_body_vec_start_pos[fd] = 0;
						this->_rn_pos[fd] = 0;
						test_body_size[fd] = 0;
						_response[fd]._body_vec.clear();
						this->_cgi.setCgiExist(false);
						_response[fd].total_response.clear();
						_response[fd].setRemainSend(false);
						_client_max_body_size = 0;
						_response[fd]._body_size = 0;
						_body_vec_total_size[fd] = 0;
						_response[fd]._body.clear();
						_response[fd]._root = _server_root;
					}
					if (this->_response[fd].getRemainSend() == false)
					{
						std::cout << "verify_method, code :  " <<  _response[fd]._code << std::endl;
						std::cout << RED << "response path : " << _response[fd].getPath() << RESET << std::endl;
						std::cout << CYAN << "body length: " << _response[fd]._body.length() << RESET << std::endl;
					}
					if (_response[fd]._connection == "close")
						disconnect_request(fd);
					else if (this->_response[fd].getRemainSend() == false)
					{
						std::cout << GREEN << "response has no error so reset values\n" << RESET;
						_request[fd].clear();
						_request_end[fd] = 0;
						_body_condition[fd] = No_Body;
						_response[fd].initRequest();
						_is_check_request_line[fd] = 0;
						_body_start_pos[fd] = 0;
						_body_end[fd] = 0;
						this->_body_vec_size[fd] = 0;
						this->_body_vec_start_pos[fd] = 0;
						this->_rn_pos[fd] = 0;
						test_body_size[fd] = 0;
						_response[fd]._body_vec.clear();
						this->_cgi.setCgiExist(false);
						_response[fd].total_response.clear();
						_response[fd].setRemainSend(false);
						_client_max_body_size = 0;
						_response[fd]._body_size = 0;
						_body_vec_total_size[fd] = 0;
						_response[fd]._body.clear();
						_response[fd]._root = _server_root;
					}
					// std::cout << RED << "why server stop????\n" << RESET << std::endl;
				}
			}
		}

		void	init_server_member()
		{
			for (std::map<int, Response>::iterator it = _response.begin(); it != _response.end(); it++)
			{
				int	fd = (*it).first;
				_response[fd].initRequest();
				_response[fd].initPossibleMethod();
				_response[fd].initErrorMap();
				_body_condition[fd] = No_Body;
				_body_end[fd] = 0;
				_body_start_pos[fd] = 0;
			}
		}

};

#endif
