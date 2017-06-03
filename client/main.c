#include <stdlib.h>
#include "client.h"
#include <unistd.h>

int main(int argc, char* argv[]) {
	int sock = connect_host("localhost", 8080);
	pthread_t t1 = send_random_interval(sock, 500000000);
	pthread_t t2 = send_random_interval(sock, 250000000);

	sleep(1);
	pthread_cancel(t2);
	sleep(1);
	pthread_cancel(t1);
	sleep(1);

	close(sock);
	return 0;
}
