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
	int client_fd;
	int len;

	struct sockaddr_un addr;
	int ret;
	char msgbuf[MAX_BUF_LEN];
	struct ipc_msg *msg;
	struct test_send_struct *st_msg;

	msg = (struct ipc_msg *) msgbuf;


	if ((client_fd = socket(PF_UNIX, SOCK_DGRAM, 0)) == -1)
	{
		printf("client socket() error\n");
		return -1;
	}


	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, CLIENT_SOCK_FILE);

	unlink(CLIENT_SOCK_FILE);

	if (bind(client_fd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
	{
		printf("client bind error\n");
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SERVER_SOCK_FILE);

	if (connect(client_fd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
	{
		printf("client connect error\n");
		return -1;
	}

	msg->type = 0;

	if (send(client_fd, msg, sizeof(msg), 0) == -1)
	{
		printf("error\n");
		return -1;
	}

	if (len = recv(client_fd, msg, MAX_BUF_LEN, 0) < 0)
	{
		printf("error\n");
		return -1;
	}

	if (msg->len >=MAX_MSG_LEN)
	{
		st_msg = (struct test_send_struct *) msg->data;
		printf("st_msg is %s\n", st_msg->msg);
	}

	return 0;
}
