#include "Server.hpp"

Server::Server() {}
Server::Server(const Server& server) { (void)server; }
Server::~Server() {}

Server&	Server::operator=(const Server& server) { (void)server; return *this; }

void	Server::change_events(std::vector<struct kevent>& change_list, uintptr_t ident,
			int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void* udata)
{
	struct kevent	temp_event;
	EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
	change_list.push_back(temp_event);
}

void	Server::disconnect_request(int request_fd)
{
	std::cout << "request disconnected: " << request_fd << std::endl;
	_request[request_fd].clear();
	close(request_fd);
	this->_request.erase(request_fd);
	this->_request_end[request_fd] = 0;
	this->_body_condition[request_fd] = No_Body;
	this->_response[request_fd].initRequest();
	this->_is_check_request_line[request_fd] = 0;
	this->_body_start_pos[request_fd] = 0;
	this->_body_end[request_fd] = 0;
	this->_body_vec_start_pos[request_fd] = 0;
	this->_body_vec_size[request_fd] = 0;
	this->_rn_pos[request_fd] = 0;
	test_body_size[request_fd] = 0;
	_response[request_fd]._body_vec.clear();
	this->_cgi.setCgiExist(false);
	_response[request_fd].total_response.clear();
	_response[request_fd].setRemainSend(false);
	_client_max_body_size = 0;
	_response[request_fd]._body_size = 0;
	_body_vec_total_size[request_fd] = 0;
	_response[request_fd]._body.clear();
	_response[request_fd]._root = _server_root;
}

void	Server::check_connection(int request_fd)
{
	if (this->_response[request_fd]._connection == "close")
		this->disconnect_request(request_fd);
}

int	Server::init_listen(const std::string& host_port)
{
	(void)host_port;
	// if (this->_response.setListen(host_port) == 1)
	// {
	// 	std::cerr << "init listen error\n";
	// 	return (1);
	// }
	// this->_listen.host = this->_response._listen.host;
	// this->_listen.port = this->_response._listen.port;
	this->_listen.host = (unsigned int)0;
	this->_listen.port = htons(8000);
	std::cout << "init listen host : " << this->_listen.host;
	std::cout << ", port : " << this->_listen.port << std::endl;
	return (0);
}

int	Server::init_server_socket()
{//????????? ???????????? ??? ?????? ???????????? ???????????? 1??? ??????, ?????? ???????????? 0??? ??????
//server socket??? ?????????, bind, listen, kqueue??? ??????,
//?????? change_events??? ?????? event??? ????????????.
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
	return (0);
}

int	Server::event_error(int fd)
{
	if (fd == this->_server_socket)
	{//server_socket??? ???????????? server??? ???????????? ?????? -1??? ????????????.
		std::cerr << "server start but server socket error\n";
		return (1);
	}
	else
	{//request socket??? ???????????? request??? ????????? ?????????.
		std::cerr << "server start but client socket error\n";
		disconnect_request(fd);
	}
	return (0);
}

void	Server::request_accept()
{
	int	request_socket;
	if ((request_socket = accept(this->_server_socket, NULL, NULL)) == -1)
	{//accept??? ???????????? ???
		std::cerr << "server start but request socket accept error\n";
		return ;
	}
	std::cout << "accept new request: " << request_socket << std::endl;
	fcntl(request_socket, F_SETFL, O_NONBLOCK);

	change_events(this->_change_list, request_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	change_events(this->_change_list, request_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	std::cout << "request is readable and writable\n";
	this->_request[request_socket] = "";
}

