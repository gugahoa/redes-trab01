#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

int create_socket(uint64_t port) {
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
		printf("Error creating socket: ", errno);

		// Relevants errno
		if (errno == EPROTOTYPE) {
			printf("socket type not supported by protocol\n");
		} else if (errno == EPROTONOSUPPORT) {
			printf("protocol not supported\n");
		} else {
			printf("errno = %d\n", errno);
		}

		return -1;
	}

	name.sin_family = AF_INET;
	name.sin_port = htons(port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);

	// http://pubs.opengroup.org/onlinepubs/7908799/xns/bind.html
	if (bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0) {
		printf("Error binding socket: ");

		// Relevants errno
		if (errno == EADDRINUSE) {
			printf("specified address (%d) is in use\n", INADDR_ANY);
		} else if (errno == EAFNOSUPPORT) {
			printf("address not supported by address family\n");
		} else {
			printf("errno = %d\n", errno);
		}

		return -1;
	}

	return sock;
}

int listen_and_serve(int socket_fd) {
	// http://pubs.opengroup.org/onlinepubs/7908799/xns/listen.html
	if (listen(socket_fd, 12) < 0) {
		printf("Error listening on socket: ");
		if (errno == EBADF) {
			printf("socket argument is not valid file descriptor\n");
		} else if (errno == EDESTADDRREQ) {
			printf("socket not bound\n");
		} else if (errno == EINVAL) {
			printf("socket already listening\n");
		} else if (errno == ENOTSOCK) {
			printf("socket argument is not a socket\n");
		} else {
			printf("errno = %d\n", errno);
		}

		return -1;
	}

	fd_set active_set, read_set;

	struct sockaddr_in client_name;
	int client_name_size;

	FD_ZERO(&active_set);
	FD_SET(socket_fd, &active_set);

	while (1) {
		printf("Multiplexing connections...\n");
		read_set = active_set;
		// http://pubs.opengroup.org/onlinepubs/007908799/xsh/select.html
		if (select(FD_SETSIZE, &read_set, NULL, NULL, NULL) < 0) {
			printf("Error multiplexing socket connections: ");

			if (errno == EBADF) {
				printf("one or more not valid open socket file descriptor in set\n");
			} else if (errno == EINVAL) {
				printf("lots of possible errors, look for EINVAL in the doc\n");
			} else {
				printf("errno = %d\n", errno);
			}

			return -1;
		}

		for (int i = 0; i < FD_SETSIZE; ++i) {
			if (!FD_ISSET(i, &read_set)) {
				continue;
			}

			// New connection
			if (i == socket_fd) {
				printf("Received new connection\n");
				client_name_size = sizeof(client_name);

				// http://pubs.opengroup.org/onlinepubs/7908799/xns/accept.html
				int new_connection = accept(socket_fd, 
						(struct sockaddr *) &client_name,
						&client_name_size);

				if (new_connection < 0) {
					printf("Error receiving connection: ");

					if (errno == ECONNABORTED) {
						printf("connection was aboted\n");
					} else if (errno == EINVAL) {
						printf("socket is not accepting new connection\n");
					} else {
						printf("errno = %d\n", errno);
					}

					return -1;
				}

				printf("Received connection from host %s, port %hd\n",
						inet_ntoa(client_name.sin_addr),
						ntohs(client_name.sin_port));
				FD_SET(new_connection, &active_set);
			} else {
				// Receiving data
				char buffer[256];
				size_t nbytes;

				// http://pubs.opengroup.org/onlinepubs/7908799/xns/recv.html
				nbytes = recv(i, buffer, 255, 0);
				if (nbytes < 0) {
					printf("Error reading from socket: ");
					
					if (errno == ECONNRESET) {
						printf("connection closed by peer\n");
					} else if (errno == EINTR) {
						printf("recv was interrupted by signal\n");
					} else if (errno == ETIMEDOUT) {
						printf("connection timed out\n");
					} else {
						printf("errno = %d\n", errno);
					}
				} else if (nbytes > 0) {
					buffer[nbytes - 1] = '\0';
					printf("Received message of size %d: `%s`\n", nbytes, buffer);

					if (strncmp(buffer, "close", 5) == 0) {
						return 0;
					}
				} else {
					// nbytes == 0, connection closed
					close(i);
					FD_CLR(i, &active_set);
				}
			}
		}
	}
}
