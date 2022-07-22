#ifndef SERVER_HPP
# define SERVER_HPP

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

		Response					_response;
		int							_body_condition;
		int							_request_end;
		bool						_is_check_request_line;
		size_t						_body_start_pos;
		int							_body_end;

		//configuation file 관련
		size_t						_client_max_body_size;
		int							_auto_index;
		std::vector<std::string>	_index;
		std::string					_cgi;

		std::vector<LocationBlock>				_locations;

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

		LocationBlock				selectLocationBlock (std::string requestURI) const
		{
			std::vector<LocationBlock>	locationBlocks;
			std::vector<std::string>	requestURIvec;
			size_t						max = 0,ret = 0;

			// first, split the request URI by '/'

			std::cout << "requestURI : " << requestURI << std::endl;
			
			if (requestURI[0] == '/')
				requestURIvec = split(&requestURI[1], '/');
			else
				requestURIvec = split(requestURI, '/');

			// add '/' to each of the string in the vector
			for (size_t i = 1; i < requestURIvec.size(); i++)
				requestURIvec[i] = "/" + requestURIvec[i];

			// look for locations whose modifier is '='; the request URI must match the location's URI
			for (size_t i = 0; i < _locations.size(); i++) {
				if (_locations[i].getMod() == EXACT) {
					if (_locations[i].getURI() == requestURI)
						return (_locations[i]);
					else
						break ;
				}
			}
			std::cout << BLUE << "befroe select location path : " << _response._path << std::endl;
			// look for locations who don't have modifiers or '~'; the location's URI must contain the request URI
			for (size_t i = 0; i < _locations.size(); i++) {
				if (_locations[i].getMod() == NONE || _locations[i].getMod() == PREFERENTIAL) {
					std::cout << "locations uri : " << _locations[i].getURI() << ", requestURI first : " << requestURIvec[0] << std::endl;
					if (_locations[i].getURI().find(requestURIvec[0], 0) != std::string::npos) {
						// first, check if the two URIs match
						if (_locations[i].getURI() == requestURI)
							locationBlocks.push_back(_locations[i]);
						// then, check for the level of the request URI
						else if (requestURIvec.size() == 1)
							locationBlocks.push_back(_locations[i]);
						else {
							// if the request URI has more than one slashes (nested), get the nested locations and compare their URIs
							std::vector<LocationBlock>	nested = _locations[i].getLocationBlocks();
							for (size_t j = 0; j < nested.size(); j++) {
								if (!compareURIsWithWildcard(nested[j].getURI(), requestURIvec[1], nested[j].getMod()))
									locationBlocks.push_back(nested[j]);
							}
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
			std::cout << PINK << "after select location path : " << locationBlocks[ret].getURI() << std::endl;

			return (locationBlocks[ret]);
		}

		void	locationToServer(LocationBlock location_block)
		{
			if (location_block.getIsEmpty() == true)
				return ;
			if (location_block.getURI() != "" && (_response._method == "GET" ||
				(_response._method != "GET" && _response._path != "/")))
				_response._path = location_block.getURI();
			if (location_block.getClntSize() != READ_BUFFER_SIZE)
				_client_max_body_size = location_block.getClntSize();
			if (location_block.getMethods().empty() == false)
				_response.initAllowMethod(location_block.getMethods());

			//redirect???
			// location_block.getRedirect()

			if (location_block.getRoot() != ".")
				_response._root = location_block.getRoot();
			if (location_block.getAutoindex() != DEFAULT_AUTOINDEX)
				_auto_index = location_block.getAutoindex();
			if (location_block.getIndex().empty() == false)
				_index = location_block.getIndex();
			if (location_block.getCGI() != "")
				_cgi = location_block.getCGI();
		}

		void	event_read(int fd)
		{//fd에 맞게 실행된다.
		//저장되어있는 request에 맞는 fd가 없다면 아무 행동도 하지 않는다.
			_request_end = 0;
			int	check_request_line_ret = 0;
			if (fd == _server_socket)
			// if (fd == static_cast<uintptr_t>(_server_socket))
			{//read event인데 server socket일 때는 accept이라는 뜻이므로 request_accept을 실행
				request_accept();
			}
			else if (_request.find(fd) != _request.end())
			{//일단 1024만큼만 읽는다.
				std::cout << "read start\n";
				char	buf[READ_BUFFER_SIZE] = {};
				int	n;
				if (_response._content_length != "" && _body_condition == Body_Start)
				{//저장해놓은 content_length만큼만 받도록 한다.
					if (_response._body_size >= _client_max_body_size)
					{
						std::cerr << "content length is too big to receive\n";
						_response.setCode(Payload_Too_Large);
						return ;
					}
				}
				n = ::recv(fd, buf, READ_BUFFER_SIZE - 1, 0);
				buf[n] = '\0';
				_request[fd] += buf;
				std::cout << RED <<  "============request==========\n" << _request[fd] << RESET;
				if (_response._body_size != 0
					&& _request[fd].length() - _body_start_pos >= _response._body_size)
				{//body size만큼 입력 받았을 때
					_request_end = 1;
					_request[fd] = _request[fd].substr(0, _body_start_pos + _response._body_size);
				}
				
				if (_request[fd] == "\r\n" && _is_check_request_line == 0)
				{//아무 값도 없이 빈 칸만 왔다는 뜻
					_request[fd].clear();
					std::cout << "request is empty line\n";
					return ;
				}
				if (_request[fd].find("\r\n") != std::string::npos &&
					_is_check_request_line == 0)
				{
					_is_check_request_line = 1;
					std::cout << "check request line\n";
					check_request_line_ret = _response.check_request_line(_request[fd]);
					if (check_request_line_ret == 9)
					{
						std::cout << "request's http version is HTTP/0.9\n";
						_request_end = 1;
					}
					else if (check_request_line_ret == 1)
					{
						std::cerr << "check request line error\n";
						_response._http_version = "HTTP/1.1";
						_response._content_location = "/";
						_response.setCode(Bad_Request);
						return ;
					}

					//check_request_line으로 parsing한 path를 통해서 select location block으로 적절한 location block을 찾는다.
					//path이 root랑 다를 때 Location block의 변수들을 Server로 넘겨준다.
					//만약 location을 못 찾았다면 그냥 server block의 변수를 사용하면 된다.
					if (_response._path != _response._root)
					{
						std::cout << PINK << "@@@@@@@@@@@@@select location@@@@@@@@@@@@" << RESET << std::endl;
						LocationBlock	test = selectLocationBlock(_response._path);
						if (test.getIsEmpty() == false)
						{//값을 가지고 있는 location block을 찾았을 경우에 이 location block의 변수들을 모두 넣어주면 된다.
							// test.print_location_block();
							locationToServer(test);
							// _response.printEntityHeader();
							// _response.printGeneralHeader();
						}
					}
				}
				
				if (_response._transfer_encoding == "chunked")
				{//content_length가 없고, transfer_encoding이 chunked일 때 파일의 끝을 알리는 것이 들어올 때까지 계속 recv
					if (_request[fd].at(_request[fd].length() - 1) == 4)
					// if (compare_end(_request[fd], "\0\r\n") == 0)
					{//파일의 끝을 받았을 때
						_request[fd] = _request[fd].substr(0, _request[fd].length() - 1);
						std::cout << "receive file end\n";
						_request_end = 1;
					}
					else if (_request[fd].find("0\r\n\r\n") != std::string::npos)
					{
						_request[fd] = _request[fd].substr(0, _request[fd].length() - 5);
						std::cout << "receive file end\n";
						_request_end = 1;
					}
					else if (_request[fd].find("0\r\n\r\n") == std::string::npos)
					{
						std::cout << PINK << "request can't find end of file\n" << _request[fd] << RESET << std::endl;
					}
					else
					{
						printf("request end : %d\n", _request[fd].at(_request[fd].length() - 1));
						std::cout << "request at : " << _request[fd].at(_request[fd].length() - 1) << std::endl;
						return ;
					}
				}

				if ((strncmp(_request[fd].c_str(), "POST", 4) == 0 ||
					strncmp(_request[fd].c_str(), "PUT", 3) == 0) && _body_condition == No_Body)
				{
					std::cout << "!!!!!!!!!!body exist\n";
					_body_condition = Body_Exist;
				}
				if ((_request[fd].empty() != 1 && _request[fd].find("\r\n\r\n") != std::string::npos)\
					&& _body_condition == No_Body) 
				{
					_request_end = 1;
					_body_condition = Body_End;
				}
				else if ((_request[fd].empty() != 1 && _request[fd].find("\r\n\r\n") != std::string::npos)\
					&& _body_condition == Body_Exist)
				{
					std::cout << "!!!!!!!!!!!!!!!body start\n";
					_body_condition = Body_Start;
					_body_start_pos = _request[fd].length();
				}

				//check header
				if ((_body_condition == Body_Start || _body_condition == Body_End)
					&& _body_end == 0)
				{
					std::cout << "received data from " << fd << ": " << _request[fd] << std::endl;
					if (check_request_line_ret == 1 || check_request_line_ret == 9)
						;
					else if (_response.request_split(_request[fd], _body_condition) == 1)
					{
						std::cerr << "request split error\n";
						_response.setCode(Bad_Request);
					}
					_body_end = 1;
				}

				if (n <= 0)
				{//read가 에러가 났거나, request가 0을 보내면 request와 연결을 끊는다.
					if (n < 0)
					{
						std::cerr << "request read error\n";
						_response.setCode(Internal_Server_Error);
					}
					else
						disconnect_request(fd);
				}
				else if (_request_end == 1)
				{
					if (_body_start_pos != 0)
					{
						_response._body = _request[fd].substr(_body_start_pos,
							_request[fd].length() - _body_start_pos);
					}
				}
			}
		}

		void	event_write(int fd)
		{
			if (_request.find(fd) != _request.end())
			{
				if (_response.verify_method(fd, &_response, _request_end) == 1)
					disconnect_request(fd);
				if (_request_end == 1)
				{
					std::cout << "verify_method, code :  " <<  _response._code << std::endl;
					std::cout << RED << "response path : " << _response.getPath() << RESET << std::endl;
					if (_response._connection == "close")
						disconnect_request(fd);
					else
					{
						_request[fd].clear();
						_request_end = 0;
						_body_condition = No_Body;
						_response.initRequest();
						_is_check_request_line = 0;
						_body_start_pos = 0;
						_body_end = 0;
					}
				}
			}
		}

		void	init_server_member()
		{
			_response.initRequest();
			_response.initPossibleMethod();
			_response.initErrorMap();
			_body_condition = No_Body;
			_body_end = 0;
			_body_start_pos = 0;
		}

};

#endif
