#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

// References:
// https://www.gnu.org/software/libc/manual/html_node/Byte-Stream-Example.html#Byte-Stream-Example
// http://pubs.opengroup.org/onlinepubs/009695399/basedefs/pthread.h.html
// http://www.geeksforgeeks.org/multithreading-c-2/

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
	// prevent overflows: choose the min between size and strlen
	size_t n = strlen(msg) > size ? size : strlen(msg);

	if (write(socket, msg, n + 1) < 0) {
		perror("Error sending message to server");
		return -1;
	}

	return 0;
}

int send_random(int socket) {
	char buffer[128];
	sprintf(buffer, "%d", rand());
	return send_msg(socket, buffer, strlen(buffer));
}

void* helper(void *vargp) {
	// Undo the bit shifting we did before creating the thread
	uint64_t socket = (uint64_t)vargp >> 32;
	uint64_t interval = ((uint64_t)vargp << 32) >> 32;
	char buffer[128];

	struct timespec req;
	req.tv_sec = 0;
	req.tv_nsec = interval; // 1 second == 1 billion nano seconds

	while (1) {
		sprintf(buffer, "socket = %d interval = %d", socket, interval);
		// TODO: change send_msg to send_random
		if (send_msg(socket, buffer, strlen(buffer)) < 0) {
			return NULL;
		}

		nanosleep(&req, NULL);
	}

	return NULL;
}

pthread_t send_random_interval(int socket, uint32_t interval) {
	// I don't know how to send more than one arg to the thread, so...
	uint64_t thread_arg = (uint64_t)interval; // The first 32 bits is the interval
	uint64_t shifted_socket = (uint64_t)socket << 32; // The last 32 bits is the socket descriptor

	// Problems: int (socket) is not always 32bits, we may lose information
	thread_arg += shifted_socket;

	pthread_t tid;
	pthread_create(&tid, NULL, helper, (void*)thread_arg);
	return tid;
}
