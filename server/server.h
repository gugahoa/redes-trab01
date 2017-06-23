#pragma once

#include <ncurses.h>

typedef struct SERVER_TDATA {

    int port;     // Port to bind the socket to
    int sock_fd;  // Server socket file descriptor

} server_tdata;

// Server stopped
extern char sv_err;

// Main window handle
extern WINDOW *whandle;

// Sensor window handles
extern WINDOW *vshandle[12];

// Station server socket (once)
int sv_create_socket(int port);

// Listen to virtual sensor gibberish
void *sv_listen_and_serve(void *tdata);

// Shutdown server and do cleanup
void sv_shutdown(void *tdata);
