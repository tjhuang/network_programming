#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>

#define MAX_MSG_LEN 16
#define MAX_BUF_LEN 32

#define SERVER_SOCK_FILE "/home/pi/unix_domain_socket/server.sock" 
#define CLIENT_SOCK_FILE "/home/pi/unix_domain_socket/client.sock" 

struct ipc_msg {
	unsigned int type;
	int len;
	char data[0];
};

struct test_send_struct {
	char msg[MAX_MSG_LEN];
};

int main()
{
	int server_fd;
	struct sockaddr_un addr;
	int ret;
	char msgbuf[MAX_BUF_LEN];
	struct ipc_msg *msg;
	struct test_send_struct st_msg;
	struct sockaddr_un from;
	socklen_t fromlen = sizeof(from);

	msg = (struct ipc_msg *) msgbuf;


	if ((server_fd = socket(PF_UNIX, SOCK_DGRAM, 0)) == -1)
	{
		printf("server socket() error\n");
		return -1;
	}


	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SERVER_SOCK_FILE);

	unlink(SERVER_SOCK_FILE);

	if (bind(server_fd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
	{
		printf("server bind error\n");
		return -1;
	}

	ret = recvfrom(server_fd, msg, MAX_BUF_LEN, 0, (struct sockaddr *) &from ,&fromlen);

	switch(msg->type)
	{
		case 0:
			strcpy(st_msg.msg, "send hello");
			msg->len = sizeof(st_msg);
			memcpy(msg->data, &st_msg, msg->len);
			ret = sendto(server_fd, msg, MAX_BUF_LEN, 0, (struct sockaddr *) &from ,fromlen);
			break;
		default:
			printf("unknown\n");
			break;
	}

	return 0;
}
