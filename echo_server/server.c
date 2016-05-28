#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>

#define ECHO_PORT 18888
#define MAXLINE 512

static int listen_fd;

void echo_handler(int sockfd)
{
	int n;
	char buf[MAXLINE];

	while (1)
	{
		n = read(sockfd, buf, MAXLINE);
		if (n > 0)
		{
			write(sockfd, buf, n);
		}
		else if (n == -1 && errno == EINTR)
		{
			fprintf(stderr, "meet interrupt\n");
			continue;
		}
		else
		{
			fprintf(stderr, "Connection closed\n");
			break;
		}
	}
}

void close_handler(int sig_id)
{
	printf("Cleanup and terminate.\n");

	close(listen_fd);

	exit(1);
}

int main()
{
	int conn_fd;
	int ret;
	int child_pid;
	socklen_t client_len = 0;
	struct sockaddr_in server_addr, client_addr;

	signal(SIGINT, close_handler);

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd == -1)
	{
		fprintf(stderr, "Failed to create socket for echo server\n");
		return -1;
	}

	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(ECHO_PORT);

	ret = bind(listen_fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in));
	if (ret < 0)
	{
		fprintf(stderr, "Failed to bind socket on port %u\n", ECHO_PORT);
		return -1;
	}

	ret = listen(listen_fd, 5);
	if (ret < 0)
	{
		fprintf(stderr, "Failed to listen error: %s (%d)\n", strerror(errno), errno);
		return -1;
	}

	while (1)
	{
		printf("Wait user connect\n");
		memset(&client_addr, 0, sizeof(struct sockaddr_in));
		conn_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len);
		if (conn_fd < 0)
		{
			fprintf(stderr, "Failed to accept connection error: %s (%d)\n", strerror(errno), errno);
			continue;
		}

		child_pid = fork();
		if (child_pid == 0)
		{
			/* child */
			close(listen_fd);

			echo_handler(conn_fd);

			exit(0);
		}
		close(conn_fd);
	}

	return 0;
}
