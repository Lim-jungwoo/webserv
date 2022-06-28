#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include <vector>
# include <string>
# include <cstring> //memset

# include <fcntl.h>
# include <unistd.h>
# include <iostream>

# include <sys/socket.h>
# include <arpa/inet.h> //sockaddr_in
# include <sys/event.h> //kevent

# define LISTEN_BUFFER_SIZE 1024

typedef struct s_listen
{
	unsigned int	host;
	int				port;
}	t_listen;

class Server
{
	public:
		Server();
		Server(const Server& server);
		~Server();

		Server&	operator=(const Server& server);

		void	change_events(std::vector<struct kevent>& change_list, uintptr_t ident,
			int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void* udata)
		{//인자로 받은 값들을 EV_SET을 이용해 kevent구조체 변수인 temp event를 초기화시키고,
		//change_list에 temp event를 추가한다.
			struct kevent	temp_event;
			EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
			change_list.push_back(temp_event);
		}

		int	init_server_socket()
		{//에러가 발생했을 때 에러 메시지를 출력하고 -1을 리턴, 정상 작동이면 0을 리턴
		//server socket을 만들고, bind, listen, kqueue를 하고,
		//바로 change_events를 통해 event를 등록한다.
			int	server_socket = socket(PF_INET, SOCK_STREAM, 0);
			if (server_socket == -1)
			{
				std::cerr << "init server socket error\n";
				return (-1);
			}
			std::memset(&this->_server_addr, 0, sizeof(this->_server_addr));
			this->_server_addr.sin_family = AF_INET;
			this->_server_addr.sin_addr.s_addr = htonl(this->_listen.host);
			this->_server_addr.sin_port = htons(this->_listen.port);

			if (bind(server_socket, (struct sockaddr*)&this->_server_addr,
				sizeof(this->_server_addr)) == -1)
			{
				std::cerr << "bind socket error\n";
				return (-1);
			}
			if (listen(server_socket, LISTEN_BUFFER_SIZE) == -1)
			{
				std::cerr << "listen socket error\n";
				return (-1);
			}
			fcntl(server_socket, F_SETFL, O_NONBLOCK);
			this->_server_socket = server_socket;

			int	kq = kqueue();
			if (kq == -1)
			{
				std::cerr << "init kqueue error\n";
				return (-1);
			}
			this->_kq = kq;
			this->change_events(this->_change_list, this->_server_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			std::cout << "server start\n"; //일단 server 시작했음을 알리자
			return (0);
		}
	
	private:
		struct sockaddr_in	_server_addr; //server의 주소를 저장
		int					_server_socket; //server_socket의 fd를 저장
		t_listen			_listen; //listen할 host와 port를 저장
		int					_kq; //kqueue를 통해 받은 fd를 저장
		std::map<int, std::string>	_request; //request의 socket과 socket의 내용을 저장
		std::vector<struct kevent>	_change_list; //kevent를 모두 저장
		struct kevent				_event_list[LISTEN_BUFFER_SIZE]; //이벤트가 발생한 kevent를 저장
};

#endif