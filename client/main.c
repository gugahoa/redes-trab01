#include "client.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int main(int argc, char* argv[])
{
        if (argc < 2) {
                printf("Usage is ./client <host-address> <n-messages>\n");
                printf("  where <host-address> may be a valid\n");
                printf("  public IP address or \"localhost\",\n");
                printf("  and <n-messages> is a 32bit integer\n");
                printf("  corresponding to the number of\n");
                printf("  gibberish messages to send.\n");
                return EXIT_FAILURE;
        }

        // Plug to host and retrieve descriptors
        int sock1 = connect_host(argv[1], 1234);
        int sock2 = connect_host(argv[1], 1234);
        int sock3 = connect_host(argv[1], 1234);

        // Sensor emulator thread 1 data
        tdata_t *data1 = malloc(sizeof(*data1));
        data1->socket = sock1;
        data1->nsends = 15;
        data1->interval = 750000000;

        // Sensor emulator thread 2 data
        tdata_t *data2 = malloc(sizeof(*data2));
        data2->socket = sock2;
        data2->nsends = 15;
        data2->interval = 500000000;

        // Sensor emulator thread 3 data
        tdata_t *data3 = malloc(sizeof(*data3));
        data3->socket = sock3;
        data3->nsends = 15;
        data3->interval = 250000000;

        pthread_t t1;  // Physical sensor emulator 1
        pthread_t t2;  // Physical sensor emulator 2
        pthread_t t3;  // Physical sensor emulator 3
        pthread_create(&t1, NULL, dummy_sender_func, (void*)data1);
        pthread_create(&t2, NULL, dummy_sender_func, (void*)data2);
        pthread_create(&t3, NULL, dummy_sender_func, (void*)data3);

        // Re-join threads
        pthread_join(t1, NULL);
        pthread_join(t2, NULL);
        pthread_join(t3, NULL);

        // Free thread data
        free(data1);
        free(data2);
        free(data3);

        // Close descriptors
        close(sock1);
        close(sock2);
        close(sock3);

        return EXIT_SUCCESS;
}
