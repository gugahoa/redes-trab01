#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
        if (argc < 2) {
                printf("Usage is ./client <host-address>\n");
                printf("  where <host-address> may be a valid\n");
                printf("  public IP address or \"localhost\".\n");
                return -1;
        }

        int sock1 = connect_host(argv[1], 1234);
        int sock2 = connect_host(argv[1], 1234);
        int sock3 = connect_host(argv[1], 1234);
        pthread_t sender1 = dummy_msg_sender(sock1, 10, 750000000);
        pthread_t sender2 = dummy_msg_sender(sock2, 10, 500000000);
        pthread_t sender3 = dummy_msg_sender(sock3, 10, 250000000);

        sleep(5);

        pthread_join(sender1, NULL);
        pthread_join(sender2, NULL);
        pthread_join(sender3, NULL);
        close(sock1);
        close(sock2);
        close(sock3);

        return 0;
}
