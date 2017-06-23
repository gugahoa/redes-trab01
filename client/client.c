#include "client.h"

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <netdb.h>

char cl_err[3];
WINDOW *whandle;
WINDOW *vshandle[3];

// References:
// https://www.gnu.org/software/libc/manual/html_node/Byte-Stream-Example.html#Byte-Stream-Example
// http://pubs.opengroup.org/onlinepubs/009695399/basedefs/pthread.h.html
// http://www.geeksforgeeks.org/multithreading-c-2/

int cl_connect_to_host(const char* host_addr, int port)
{
    // Create sock_fd fd
    int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        printf("  Error creating sock_fd.\n");
        return -1;
    }

    // Find hostname
    printf("  Engaging host %s... ", host_addr);

    struct hostent *host_info;      // Host name store struct
    host_info = gethostbyname(host_addr);

    if (host_info == NULL) {
        printf("Unkown host %s\n.", host_addr);
        return -1;
    }

    struct sockaddr_in host_name;   // Host name full info struct
    host_name.sin_family = AF_INET;
    host_name.sin_port = htons(port);
    host_name.sin_addr = *(struct in_addr*) host_info->h_addr;
    printf("Done.\n");

    // Connect
    printf("  Connecting to %s on bind-port %hu, fifo %d... ",
            inet_ntoa(host_name.sin_addr),
            ntohs(host_name.sin_port), sock_fd);

    if (connect(sock_fd, (struct sockaddr*) &host_name, sizeof(host_name)) < 0) {
        printf("Error trying to connect.\n");
        return -1;
    }

    printf("Done.\n");
    return sock_fd;
}

void* cl_generate_and_send(void *tdata)
{
    tdata_t *data = (tdata_t *)tdata;

    char buffer[128];               // Data send buffer
    struct timespec req;            // Thread time requirement

    req.tv_sec = 0;                 // No full second, only fractions
    req.tv_nsec = data->interval;   // 1 second == 1 billion nano seconds

    while (1) {
        // The message we're sending...
        snprintf(buffer, 20, "%d", rand());
        size_t buffsize = strlen(buffer);

        if (write(data->sock_fd, buffer, buffsize) < 0) {
            pthread_mutex_lock(&printlock);
            wattron(vshandle[data->my_id], A_STANDOUT);
            wprintw(vshandle[data->my_id], "Fatal error.\n");
            wprintw(vshandle[data->my_id], "Comms aborted.\n");
            wattroff(vshandle[data->my_id], A_STANDOUT);
            wrefresh(vshandle[data->my_id]);
            pthread_mutex_unlock(&printlock);

            sleep(3);
            cl_err[data->my_id] = 1;
            pthread_exit(tdata);

        } else {
            pthread_mutex_lock(&printlock);
            wprintw(vshandle[data->my_id], "%s\n", buffer);
            wrefresh(vshandle[data->my_id]);
            pthread_mutex_unlock(&printlock);
        }

        nanosleep(&req, NULL);
    }

    pthread_exit(tdata);
}
