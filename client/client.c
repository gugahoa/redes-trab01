#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

// References:
// https://www.gnu.org/software/libc/manual/html_node/Byte-Stream-Example.html#Byte-Stream-Example
// http://pubs.opengroup.org/onlinepubs/009695399/basedefs/pthread.h.html
// http://www.geeksforgeeks.org/multithreading-c-2/

int connect_host(const char* host_addr, uint16_t port)
{
        // Create socket fd
        int sock = socket(PF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
                perror("  Error creating socket.");
                return -1;
        }

        // Find hostname
        printf("  Engaging host %s... ", host_addr);

        struct hostent *host_info;      // TODO comment your variables
        struct sockaddr_in host_name;   // TODO comment your variables

        host_info = gethostbyname(host_addr);
        if (host_info == NULL) {
                printf("Unkown host %s\n.", host_addr);
                return -1;
        }

        host_name.sin_family = AF_INET;
        host_name.sin_port = htons(port);
        host_name.sin_addr = *(struct in_addr*) host_info->h_addr;
        printf("Done.\n");

        // Connect
        printf("  Connecting to %s on bind-port %hu, fifo %d... ",
                        inet_ntoa(host_name.sin_addr),
                        ntohs(host_name.sin_port), sock);
        if (connect(sock, (struct sockaddr*) &host_name, sizeof(host_name)) < 0) {
                printf("Error trying to connect.\n");
                return -1;
        }

        printf("Done.\n");
        return sock;
}

int send_msg(int socket, const char* msg, size_t size)
{
        // prevent overflows: choose the min between size and strlen
        size_t n = strlen(msg) > size ? size : strlen(msg);

        printf("    Writing to socket fifo %d: \"%s\".\n", socket, msg);

        if (write(socket, msg, n + 1) < 0) {
                printf("    Error writing message on socket.");
                return -1;
        }

        return 0;
}

void* dummy_msg_sender_func(void *tdata)
{
        tdata_t *data = (tdata_t *)tdata;

        struct timespec req;            // TODO comment your variables
        req.tv_sec = 0;                 // No full second, only fractions
        req.tv_nsec = data->interval;   // 1 second == 1 billion nano seconds
        size_t nsends = data->nsends;   // Send this amount of messages

        while (nsends > 0) {
                char buffer[128];
                sprintf(buffer, "%d", rand());

                if (send_msg(data->socket, buffer, strlen(buffer)) < 0)
                        return NULL;

                nanosleep(&req, NULL);
                nsends--;
        }

        pthread_exit(tdata);
}

pthread_t dummy_msg_sender(int socket, size_t nsends, uint64_t interval)
{
        // assemble thread data
        tdata_t *data = malloc(sizeof(*data));  // Data to be used by worker bread
        data->socket = socket;
        data->nsends = nsends;
        data->interval = interval;

        // create worker thread
        pthread_t tid;                          // Generate unique thread identifier
        pthread_create(&tid, NULL, dummy_msg_sender_func, (void*)data);

        // TODO Deallocate thread data. Ideally, inline this function.

        // return identifier
        return tid;
}
