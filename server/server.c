#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

int create_socket(uint64_t port)
{
        // Socket file descripton
        int sock;

        // sockaddr_in for internet address
        // sockaddr_un for local unix filepath
        struct sockaddr_in name;

        // http://pubs.opengroup.org/onlinepubs/7908799/xns/socket.html
        // AF_INET is the domain (Internet or unix file descripton)
        // SOCK_STREAM is connection type (Others: SOCK_DGRAM, SOCK_SEQPACKET)
        // 0 is the default protocol communcation
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
                perror("  Error creating socket");
                return -1;
        }

        name.sin_family = AF_INET;
        name.sin_port = htons(port);
        name.sin_addr.s_addr = htonl(INADDR_ANY);

        // http://pubs.opengroup.org/onlinepubs/7908799/xns/bind.html
        if (bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0) {
                perror("  Error binding socket");
                return -1;
        }

        return sock;
}

int listen_and_serve(int socket_fd)
{
        // http://pubs.opengroup.org/onlinepubs/7908799/xns/listen.html
        if (listen(socket_fd, 12) < 0) {
                perror("  Error listening on socket");
                return -1;
        }

        fd_set read_set;                        // TODO comment your variables
        fd_set active_set;                      // TODO comment your variables

        struct sockaddr_in client_name;         // TODO comment your variables
        unsigned client_name_size;              // TODO comment your variables

        FD_ZERO(&active_set);
        FD_SET(socket_fd, &active_set);

        printf("  Server socket: Waiting for connections...\n");

        while (1) {
                read_set = active_set;
                // http://pubs.opengroup.org/onlinepubs/007908799/xsh/select.html
                if (select(FD_SETSIZE, &read_set, NULL, NULL, NULL) < 0) {
                        perror("    Error multiplexing socket connections.");
                        return -1;
                }

                for (int isocket = 0; isocket < FD_SETSIZE; isocket++) {
                        if (!FD_ISSET(isocket, &read_set)) {
                                continue;
                        }

                        // New connection
                        if (isocket == socket_fd) {
                                client_name_size = sizeof(client_name);

                                // http://pubs.opengroup.org/onlinepubs/7908799/xns/accept.html
                                int new_connection = accept(socket_fd,
                                                (struct sockaddr *) &client_name,
                                                &client_name_size);

                                if (new_connection < 0) {
                                        perror("    Error accepting new connection.");
                                        return -1;
                                }

                                FD_SET(new_connection, &active_set);
                                printf("    Accepted new connection from host %s on bind-port %hu.\n",
                                                inet_ntoa(client_name.sin_addr),
                                                ntohs(client_name.sin_port));

                        } else {
                                // Receiving data
                                char buffer[256];
                                ssize_t nbytes;

                                // http://pubs.opengroup.org/onlinepubs/7908799/xns/recv.html
                                nbytes = recv(isocket, buffer, 255, 0);

                                if (nbytes > 0) {                               // Read success
                                        buffer[nbytes - 1] = '\0';
                                        printf("      Message of size %ld (socket fifo %d): \"%s\".\n",
                                                        nbytes, isocket, buffer);

                                        // Stop listening altogether
                                        if (strncmp(buffer, "close", 5) == 0)
                                                return 0;

                                } else if (nbytes < 0) {                        // Read failure
                                        printf("      Error receiving message: fifo %d.\n", isocket);

                                } else {                                        // Connection closed
                                        close(isocket);
                                        FD_CLR(isocket, &active_set);
                                        printf("    Connection closed: fifo %d.\n", isocket);

                                }
                        }
                }
        }
}
