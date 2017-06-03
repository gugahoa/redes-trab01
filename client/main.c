#include <stdlib.h>
#include "client.h"
#include <unistd.h>

int main(int argc, char* argv[]) {
	int sock = connect_host("localhost", 8080);
	for (int i = 0; i < 100; ++i) {
		send_random(sock);
	}

	close(sock);
	return 0;
}
