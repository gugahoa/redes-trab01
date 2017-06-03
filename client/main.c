#include <stdlib.h>
#include "client.h"
#include <unistd.h>

int main(int argc, char* argv[]) {
	int sock = connect_host("localhost", 8080);
	send_random_interval(sock, 500000000);
	send_random_interval(sock, 250000000);

	sleep(2);
	close(sock);
	return 0;
}
