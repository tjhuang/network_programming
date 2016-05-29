#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>
#include <arpa/inet.h>


#define ECHO_PORT 18888
#define MAXLINE 512


int main()
{
	int conn_fd;
	int ret;
	struct sockaddr_in server;
	char buff[MAXLINE];
	char resp[MAXLINE];

	conn_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (conn_fd == -1)
	{
		fprintf(stderr, "Failed to create socket for echo server\n");
		return -1;
	}

	memset(&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(ECHO_PORT);

	ret = connect(conn_fd, (struct sockaddr *) &server, sizeof(struct sockaddr_in));
	if (ret < 0)
	{
		fprintf(stderr, "Failed to connect to server\n");
		return -1;
	}

	strcpy(buff, "Hello World!");
	send(conn_fd, buff, 512, 0);

	recv(conn_fd, resp, 512, 0);
	printf("Get response: %s\n", resp);

	close(conn_fd);

	return 0;
}
