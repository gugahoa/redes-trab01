#include "server.h"

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>

char sv_err;
WINDOW *whandle;
WINDOW *vshandle[12];

int sv_create_socket(int port)
{
    // Socket file descripton
    int sock_fd;

    // sockaddr_in for internet address
    // sockaddr_un for local unix filepath
    struct sockaddr_in name;

    // http://pubs.opengroup.org/onlinepubs/7908799/xns/socket.html
    // AF_INET is the domain (internet or unix file descripton)
    // SOCK_STREAM is connection type (others: SOCK_DGRAM, SOCK_SEQPACKET)
    // 0 is the default protocol communcation
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        printf("  Error creating socket");
        return -1;
    }

    name.sin_family = AF_INET;
    name.sin_port = htons(port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);

    // http://pubs.opengroup.org/onlinepubs/7908799/xns/bind.html
    if (bind(sock_fd, (struct sockaddr *)&name, sizeof(name)) < 0) {
        printf("  Error binding socket");
        return -1;
    }

    return sock_fd;
}

void *sv_listen_and_serve(void *tdata)
{
    // Grab socket number from thread data
    int sock_fd = ((server_tdata *)tdata)->sock_fd;

    // http://pubs.opengroup.org/onlinepubs/7908799/xns/listen.html
    if (listen(sock_fd, 12) < 0) {
        wattron(whandle, A_STANDOUT);
        mvwprintw(whandle, 1, 72, "Listening error. Aborting.");
        wattroff(whandle, A_STANDOUT);
        wrefresh(whandle);

        sleep(3);
        sv_err = 1;
        pthread_exit(tdata);
    }

    fd_set read_set;                 // ready file descriptor set
    fd_set active_set;               // active file descriptor set
    struct sockaddr_in client_name;  // client name addr struct
    unsigned client_name_size;       // client name length

    FD_ZERO(&active_set);
    FD_SET(sock_fd, &active_set);

    // Pre-provide cleanup routines
    pthread_cleanup_push(sv_shutdown, tdata);

    // TODO HERE
    while (1) {
        wattron(whandle, A_STANDOUT);
        mvwprintw(whandle, 1, 72, "Ready and listening.");
        wattroff(whandle, A_STANDOUT);
        wrefresh(whandle);

        read_set = active_set;
        // http://pubs.opengroup.org/onlinepubs/007908799/xsh/select.html
        if (select(FD_SETSIZE, &read_set, NULL, NULL, NULL) < 0) {
            wattron(whandle, A_STANDOUT);
            mvwprintw(whandle, 1,  72,   "Error multiplexing connections. Aborting.");
            wattroff(whandle, A_STANDOUT);
            wrefresh(whandle);

            sleep(3);
            sv_err = 1;
            pthread_exit(tdata);
        }

        for (int isocket = 0; isocket < FD_SETSIZE; isocket++) {
            if (!FD_ISSET(isocket, &read_set)) {
                continue;
            }

            // New connection
            if (isocket == sock_fd) {
                client_name_size = sizeof(client_name);

                // http://pubs.opengroup.org/onlinepubs/7908799/xns/accept.html
                int new_connection = accept(sock_fd,
                        (struct sockaddr *) &client_name,
                        &client_name_size);

                if (new_connection < 0) {
                    wattron(whandle, A_STANDOUT);
                    mvwprintw(whandle, 1, 72, "Connection error. Aborting.");
                    wattroff(whandle, A_STANDOUT);
                    wrefresh(whandle);

                    sleep(3);
                    sv_err = 1;
                    pthread_exit(tdata);
                }

                FD_SET(new_connection, &active_set);
                //wattron(whandle, A_STANDOUT);
                //mvwprintw(whandle, 2, 72, "Last connection from %s on port %hu.",
                //        inet_ntoa(client_name.sin_addr),
                //        ntohs(client_name.sin_port));
                //wattroff(whandle, A_STANDOUT);
                //wrefresh(whandle);

            } else {
                // Receiving data
                char buffer[256];
                ssize_t nbytes;

                // http://pubs.opengroup.org/onlinepubs/7908799/xns/recv.html
                nbytes = recv(isocket, buffer, 255, 0);

                // Read success
                if (nbytes > 0) {
                    buffer[nbytes] = '\0';
                    //printf("      Message of size %ld (sock_fd fifo %d): \"%s\".\n",
                    //        nbytes, isocket, buffer);
                    wprintw(vshandle[isocket-4], "%s\n", buffer);
                    wrefresh(vshandle[isocket-4]);

                // Read failure
                } else if (nbytes < 0) {
                    //printf("      Error receiving message: fifo %d.\n", isocket);
                    wprintw(vshandle[isocket-4], "Receive error!\n");
                    wrefresh(vshandle[isocket-4]);

                // Connection closed
                } else {
                    close(isocket);
                    FD_CLR(isocket, &active_set);
                    //printf("    Connection closed: fifo %d.\n", isocket);
                    wprintw(vshandle[isocket-4], "Connection closed!\n");
                    wrefresh(vshandle[isocket-4]);
                }
            }
        }
    }

    pthread_cleanup_pop(1);
    pthread_exit(tdata);
}

void sv_shutdown(void *tdata)
{
    // Grab socket number from thread data
    int sock_fd = ((server_tdata *)tdata)->sock_fd;

    // Disable further send and receive ops
    shutdown(sock_fd, SHUT_RDWR);

    // Print fatal message
    wattron(whandle, A_STANDOUT);
    mvwprintw(whandle, 1, 72, "All activity ceased. Press `q' to quit.");
    wattroff(whandle, A_STANDOUT);
    wrefresh(whandle);
}
