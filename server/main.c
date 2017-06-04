#include "server.h"

#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main()
{
        printf("Creating server-side socket...\n");

        int sock = create_socket(1234);
        if (sock < 0) {
                return -1;
        }

        listen_and_serve(sock);

        shutdown(sock, 2);
        return 0;
}
