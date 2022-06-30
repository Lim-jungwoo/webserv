#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>

#define BUF_LEN 1024

int	main(int argc, char *argv[])
{
	int	client_socket;
	struct sockaddr_in	server_addr;
	char*	haddr;
	char	buf[BUF_LEN + 1];
	int		port;
	

	if (argc == 3)
	{
		port = atoi(argv[2]);
	}
	else
	{
		printf("usage: webclient server_addr [port_number]");
		return (-1);
	}

	haddr = argv[1];
	if ((client_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("can not create socket\n");
		return (-1);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(haddr);
	server_addr.sin_port = htons(port);

	if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("can not connect");
		return (-1);
	}

	while (1)
	{
		fputs("Input message(Q to quit): ", stdout);
		fgets(buf, BUF_LEN, stdin);
		if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
			break ;
		
		int	str_len = write(client_socket, buf, strlen(buf));
		int	recv_len = 0;
		int	recv_cnt = 0;
		while (recv_len < str_len)
		{
			recv_cnt = read(client_socket, &buf[recv_len], BUF_LEN - 1);
			if (recv_cnt == -1)
			{
				write(1, "read error\n", 11);
				return (1);
			}
			recv_len += recv_cnt;
		}
		buf[recv_len] = 0;
		printf("Message from server\n%s\nmessage size : %d\n", buf, recv_len);
	}
	close(client_socket);
}