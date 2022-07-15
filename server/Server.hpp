#ifndef SERVER_HPP
# define SERVER_HPP

# include "../response/Response.hpp"

class Server
{
	public:
		Server() {}
		Server(const Server& server);
		~Server() {}

		Server&	operator=(const Server& server);

		void	change_events(std::vector<struct kevent>& change_list, uintptr_t ident,
			int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void* udata)
		{//인자로 받은 값들을 EV_SET을 이용해 kevent구조체 변수인 temp event를 초기화시키고,
		//change_list에 temp event를 추가한다.
			struct kevent	temp_event;
			EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
			change_list.push_back(temp_event);
		}

		void	disconnect_request(int request_fd)
		{
			std::cout << "request disconnected: " << request_fd << std::endl;
			close(request_fd);
			this->_request.erase(request_fd);
			this->_request_end = 0;
			this->_body_condition = No_Body;
			this->_response.resetRequest();
			this->_is_check_request_line = 0;
			this->_body_start_pos = 0;
			this->_body_end = 0;
		}

		void	check_connection(int request_fd)
		{
			if (this->_response._connection == "close")
				this->disconnect_request(request_fd);
		}

		int	init_listen(const std::string& host_port)
		{
			if (this->_response.setListen(host_port) == 1)
			{
				std::cerr << "init listen error\n";
				return (1);
			}
			this->_listen.host = this->_response._listen.host;
			this->_listen.port = this->_response._listen.port;
			return (0);
		}

		int	init_server_socket()
		{//에러가 발생했을 때 에러 메시지를 출력하고 1을 리턴, 정상 작동이면 0을 리턴
		//server socket을 만들고, bind, listen, kqueue를 하고,
		//바로 change_events를 통해 event를 등록한다.
			int	server_socket = socket(PF_INET, SOCK_STREAM, 0);
			if (server_socket == -1)
			{
				std::cerr << "init server socket error\n";
				return (1);
			}
			std::memset(&this->_server_addr, 0, sizeof(this->_server_addr));
			this->_server_addr.sin_family = AF_INET;
			this->_server_addr.sin_addr.s_addr = this->_listen.host;
			this->_server_addr.sin_port = this->_listen.port;
			std::cout << "host : " << this->_listen.host << ", port : " << this->_listen.port << std::endl;
			if (bind(server_socket, (struct sockaddr*)&this->_server_addr,
				sizeof(this->_server_addr)) == -1)
			{
				std::cerr << "bind socket error\n";
				return (1);
			}
			if (listen(server_socket, LISTEN_BUFFER_SIZE) == -1)
			{
				std::cerr << "listen socket error\n";
				return (1);
			}
			fcntl(server_socket, F_SETFL, O_NONBLOCK);
			this->_server_socket = server_socket;

			int	kq = kqueue();
			if (kq == -1)
			{
				std::cerr << "init kqueue error\n";
				return (1);
			}
			this->_kq = kq;
			this->change_events(this->_change_list, this->_server_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			std::cout << "server start\n"; //일단 server 시작했음을 알리자
			return (0);
		}

		int	event_error(int fd)
		{
			if (fd == this->_server_socket)
			{//server_socket이 에러라면 server를 종료하기 위해 -1을 리턴한다.
				std::cerr << "server start but server socket error\n";
				return (1);
			}
			else
			{//request socket이 에러라면 request의 연결을 끊는다.
				std::cerr << "server start but client socket error\n";
				disconnect_request(fd);
			}
			return (0);
		}

		void	request_accept()
		{//accept이 실패했다고 서버를 종료하는 것은 아닌 것 같다.
			int	request_socket;
			if ((request_socket = accept(this->_server_socket, NULL, NULL)) == -1)
			{//accept이 실패했을 때
				std::cerr << "server start but request socket accept error\n";
				return ;
			}
			std::cout << "accept new request: " << request_socket << std::endl;
			fcntl(request_socket, F_SETFL, O_NONBLOCK);

			change_events(this->_change_list, request_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			change_events(this->_change_list, request_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
			this->_request[request_socket] = "";
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
				char	buf[READ_BUFFER_SIZE] = {};
				int	n;
				if (this->_response._content_length != "" && this->_body_condition == Body_Start)
				{//저장해놓은 content_length만큼만 받도록 한다.
					if (this->_response._body_size >= READ_BUFFER_SIZE)
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
					if (this->_response.verify_method(fd) == 1)
						disconnect_request(fd);
					if (this->_response._connection == "close")
						disconnect_request(fd);
					else
					{
						this->_request[fd].clear();
						this->_request_end = 0;
						this->_body_condition = No_Body;
						this->_response.resetRequest();
						this->_is_check_request_line = 0;
						this->_body_start_pos = 0;
						this->_body_end = 0;
					}
				}
			}
		}

		int	server_start()
		{
			int				new_events;
			struct kevent*	curr_event;
			this->_body_condition = No_Body;
			this->_body_end = 0;
			this->_body_start_pos = 0;
			this->_response.resetRequest();
			this->_response.initPossibleMethod();
			this->_response.initAllowMethod();
			this->_response.initErrorHtml();
			if (this->init_server_socket() == 1)
				return (1);
			while (1)
			{
				new_events = kevent(this->_kq, &this->_change_list[0], this->_change_list.size(),
					this->_event_list, LISTEN_BUFFER_SIZE, NULL);
				//kevent로 change_list에 등록되어 있는 event를 등록한다.
				if (new_events == -1)
				{
					std::cerr << "kevent error\n";
					return (1);
				}
				this->_change_list.clear();
				//change_list에 등록되어 있는 event를 삭제한다.

				for (int i = 0; i < new_events; i++)
				{//event가 발생한 개수가 new_events이므로, 개수만큼 반복한다.
					
					curr_event = &this->_event_list[i];
					if (curr_event->flags & EV_ERROR)
					{//curr_event가 error일 때
						if (event_error(curr_event->ident) == 1)
						{
							std::cerr << "kevent error occured so server is stop\n";
							return (1);
						}
					}
					else if (curr_event->filter == EVFILT_READ)
					{//curr_event가 read일 때
						// write(1, "read", 4);
						event_read(curr_event->ident);
					}
					else if (curr_event->filter == EVFILT_WRITE)
					{
						// write(1, "write", 5);
						event_write(curr_event->ident);
					}
				}
			}
			return (0);
		}
	
	private:
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
		

};

#endif