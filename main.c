#include <unistd.h>
#include <strings.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server/server.h"

int main() {
	printf("Hello, world!\n");
	int sock = create_socket(8080);
	printf("socket: %d\n", sock);
	if (socket < 0) {
		return -1;
	}

	listen_and_serve(sock);

	shutdown(sock, 2);
	return 0;
}
