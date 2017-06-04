#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>

int main()
{
        printf("Creating server-side socket...\n");

        int sock = create_socket(1234);

        if (sock < 0)
                return EXIT_FAILURE;

        listen_and_serve(sock);

        shutdown(sock, 2);

        return EXIT_SUCCESS;
}
