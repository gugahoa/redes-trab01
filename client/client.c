#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

int populate_sockaddr(struct sockaddr_in* name, const char* host, uint16_t port) {
	struct hostent *hostinfo;

	name->sin_family = AF_INET;
	name->sin_port = htons(port);
	hostinfo = gethostbyname(host);
	if (hostinfo == NULL) {
		printf("Unkown host %s\n", host);
		return -1;
	}

	name->sin_addr = *(struct in_addr*) hostinfo->h_addr;
	return 0;
}

int connect_host(const char* addr, uint16_t port)  {
	int sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("Error creating socket");
		return -1;
	}

	struct sockaddr_in servername;
	if (populate_sockaddr(&servername, addr, port) != 0) {
		return -1;
	}

	if (connect(sock, (struct sockaddr*) &servername, sizeof(servername)) < 0) {
		perror("Error trying to connect");
		return -1;
	}
	return sock;
}

int send_msg(int socket, const char* msg, size_t size) {
	size_t n = strlen(msg) > size ? size : strlen(msg);

	if (write(socket, msg, n + 1) < 0) {
		perror("Error sending message to server");
		return -1;
	}

	return 0;
}

void send_random(int socket) {
	char buffer[128];
	sprintf(buffer, "%d", rand());
	send_msg(socket, buffer, strlen(buffer));
}
