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

			// look for locations who don't have modifiers or '~'; the location's URI must contain the request URI
			for (size_t i = 0; i < _locations.size(); i++) {
				if (_locations[i].getMod() == NONE || _locations[i].getMod() == PREFERENTIAL) {
					std::cout << "locations uri : " << this->_locations[i].getURI() << ", requestURI first : " << requestURIvec[0] << std::endl;
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

			return (locationBlocks[ret]);
		}

		void	locationToServer(LocationBlock location_block)
		{
			if (location_block.getIsEmpty() == true)
				return ;
			if (location_block.getURI() != "")
				this->_response._path = location_block.getURI();
			if (location_block.getClntSize() != READ_BUFFER_SIZE)
				this->_client_max_body_size = location_block.getClntSize();
			if (location_block.getMethods().empty() == false)
				this->_response.initAllowMethod(location_block.getMethods());

			//redirect???
			// location_block.getRedirect()

			if (location_block.getRoot() != ".")
				this->_response._root = location_block.getRoot();
			if (location_block.getAutoindex() != DEFAULT_AUTOINDEX)
				this->_auto_index = location_block.getAutoindex();
			if (location_block.getIndex().empty() == false)
				this->_index = location_block.getIndex();
			if (location_block.getCGI() != "")
				this->_cgi = location_block.getCGI();
		}

		void	event_read(int fd)
		{//fd에 맞게 실행된다.
		//저장되어있는 request에 맞는 fd가 없다면 아무 행동도 하지 않는다.
			this->_request_end = 0;
			int	check_request_line_ret = 0;
			if (fd == this->_server_socket)
			// if (fd == static_cast<uintptr_t>(this->_server_socket))
			{//read event인데 server socket일 때는 accept이라는 뜻이므로 request_accept을 실행
				request_accept();
			}
			else if (this->_request.find(fd) != this->_request.end())
			{//일단 1024만큼만 읽는다.
				std::cout << "read start\n";
				char	buf[READ_BUFFER_SIZE] = {};
				int	n;
				if (this->_response._content_length != "" && this->_body_condition == Body_Start)
				{//저장해놓은 content_length만큼만 받도록 한다.
					if (this->_response._body_size >= this->_client_max_body_size)
					{
						std::cerr << "content length is too big to receive\n";
						this->_response.setCode(Payload_Too_Large);
						return ;
					}
				}
				n = ::recv(fd, buf, READ_BUFFER_SIZE - 1, 0);
				buf[n] = '\0';
				this->_request[fd] += buf;
				std::cout << RED <<  "============request==========\n" << this->_request[fd] << RESET;
				if (this->_response._body_size != 0
					&& this->_request[fd].length() - this->_body_start_pos >= this->_response._body_size)
				{//body size만큼 입력 받았을 때
					this->_request_end = 1;
					this->_request[fd] = this->_request[fd].substr(0, this->_body_start_pos + this->_response._body_size);
				}
				
				if (this->_request[fd] == "\r\n" && this->_is_check_request_line == 0)
				{//아무 값도 없이 빈 칸만 왔다는 뜻
					this->_request[fd].clear();
					std::cout << "request is empty line\n";
					return ;
				}
				if (compare_end(this->_request[fd], "\r\n") == 0 &&
					this->_is_check_request_line == 0)
				{
					this->_is_check_request_line = 1;
					std::cout << "check request line\n";
					check_request_line_ret = this->_response.check_request_line(this->_request[fd]);
					if (check_request_line_ret == 9)
					{
						std::cout << "request's http version is HTTP/0.9\n";
						this->_request_end = 1;
					}
					else if (check_request_line_ret == 1)
					{
						std::cerr << "check request line error\n";
						this->_response._http_version = "HTTP/1.1";
						this->_response._content_location = "/";
						this->_response.setCode(Bad_Request);
						return ;
					}

					//check_request_line으로 parsing한 path를 통해서 select location block으로 적절한 location block을 찾는다.
					//path이 root랑 다를 때 Location block의 변수들을 Server로 넘겨준다.
					//만약 location을 못 찾았다면 그냥 server block의 변수를 사용하면 된다.
					if (this->_response._path != this->_response._root)
					{
						std::cout << PINK << "@@@@@@@@@@@@@select location@@@@@@@@@@@@" << RESET << std::endl;
						LocationBlock	test = this->selectLocationBlock(this->_response._path);
						if (test.getIsEmpty() == false)
						{//값을 가지고 있는 location block을 찾았을 경우에 이 location block의 변수들을 모두 넣어주면 된다.
							// test.print_location_block();
							this->locationToServer(test);
							this->_response.printEntityHeader();
							this->_response.printGeneralHeader();
						}
					}
				}
				
				if (this->_response._content_length == "" && this->_response._transfer_encoding == "chunked")
				{//content_length가 없고, transfer_encoding이 chunked일 때 파일의 끝을 알리는 것이 들어올 때까지 계속 recv
					if (this->_request[fd].at(this->_request[fd].length() - 1) == 4)
					// if (compare_end(this->_request[fd], "\0\r\n") == 0)
					{//파일의 끝을 받았을 때
						this->_request[fd] = this->_request[fd].substr(0, this->_request[fd].length() - 1);
						std::cout << "receive file end\n";
						this->_request_end = 1;
					}
					else if (compare_end(this->_request[fd], "0\r\n\r\n") == 0)
					{
						this->_request[fd] = this->_request[fd].substr(0, this->_request[fd].length() - 5);
						std::cout << "receive file end\n";
						this->_request_end = 1;
					}
					else
					{
						printf("request end : %d\n", this->_request[fd].at(this->_request[fd].length() - 1));
						std::cout << "request at : " << this->_request[fd].at(this->_request[fd].length() - 1) << std::endl;
						return ;
					}
				}

				if ((strncmp(this->_request[fd].c_str(), "POST", 4) == 0 ||
					strncmp(this->_request[fd].c_str(), "PUT", 3) == 0) && this->_body_condition == No_Body)
				{
					std::cout << "!!!!!!!!!!body exist\n";
					this->_body_condition = Body_Exist;
				}
				if ((this->_request[fd].empty() != 1 && compare_end(this->_request[fd], "\r\n\r\n") == 0)\
					&& this->_body_condition == No_Body) 
				{
					this->_request_end = 1;
					this->_body_condition = Body_End;
				}
				else if ((this->_request[fd].empty() != 1 && compare_end(this->_request[fd], "\r\n\r\n") == 0)\
					&& this->_body_condition == Body_Exist)
				{
					std::cout << "!!!!!!!!!!!!!!!body start\n";
					this->_body_condition = Body_Start;
					this->_body_start_pos = this->_request[fd].length();
				}

				//check header
				if ((this->_body_condition == Body_Start || this->_body_condition == Body_End)
					&& this->_body_end == 0)
				{
					std::cout << "received data from " << fd << ": " << this->_request[fd] << std::endl;
					if (check_request_line_ret == 1 || check_request_line_ret == 9)
						;
					else if (this->_response.request_split(this->_request[fd], this->_body_condition) == 1)
					{
						std::cerr << "request split error\n";
						this->_response.setCode(Bad_Request);
					}
					this->_body_end = 1;
				}

				if (n <= 0)
				{//read가 에러가 났거나, request가 0을 보내면 request와 연결을 끊는다.
					if (n < 0)
					{
						std::cerr << "request read error\n";
						this->_response.setCode(Internal_Server_Error);
					}
					else
						disconnect_request(fd);
				}
				else if (this->_request_end == 1)
				{
					if (this->_body_start_pos != 0)
					{
						this->_response._body = this->_request[fd].substr(this->_body_start_pos,
							this->_request[fd].length() - this->_body_start_pos);
					}
				}
			}
		}

		void	event_write(int fd)
		{
			std::map<int, std::string>::iterator	it = this->_request.find(fd);
			int										write_size;
			if (it != this->_request.end())
			{
				(void)write_size;
				//HTTP/0.9이거나 HTTP/1.0일 때는 종료
				
				if (this->_response._code == Bad_Request || this->_response._code == Internal_Server_Error
					|| this->_response._code == Payload_Too_Large)
				{
					if (this->_response._error_html.find(this->_response._code) != this->_response._error_html.end())
						this->_response._path = this->_response._error_html[this->_response._code];

					std::string	header = this->_response.getHeader();
					std::map<int, std::string>::iterator	it = this->_response._error_html.find(this->_response._code);
					if (it != this->_response._error_html.end())
					{
						header += this->_response.readHtml(it->second);
					}

					std::cout << YELLOW << "\n==========response=========\n" << header << RESET;
					::send(fd, header.c_str(), header.size(), 0);
					disconnect_request(fd);
				}
				if (this->_request_end == 1)
				{
					std::cout << "verify_method, code :  " <<  this->_response._code << std::endl;
					if (this->_response.verify_method(fd) == 1)
						disconnect_request(fd);
					if (this->_response._connection == "close")
						disconnect_request(fd);
					else
					{
						this->_request[fd].clear();
						this->_request_end = 0;
						this->_body_condition = No_Body;
						this->_response.initRequest();
						this->_is_check_request_line = 0;
						this->_body_start_pos = 0;
						this->_body_end = 0;
					}
				}
			}
		}

		void	init_server_member()
		{
			this->_response.initRequest();
			this->_response.initPossibleMethod();
			this->_response.initErrorMap();
			this->_body_condition = No_Body;
			this->_body_end = 0;
			this->_body_start_pos = 0;
		}

};

#endif