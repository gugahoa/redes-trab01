#pragma once

#include <stdlib.h>
#include <ncurses.h>

typedef struct TDATA_T {

    int      my_id;
    int      sock_fd;
    long     interval;

} tdata_t;

pthread_mutex_t printlock;

// Thread error flgs
extern char cl_err[3];

// Main window handle
extern WINDOW *whandle;

// Sensor window handles
extern WINDOW *vshandle[3];

// Connect to host of name 'host_addr' through port 'port'
int cl_connect_to_host(const char* host_addr, int port);

// Gibberish messsage sender function for sensor emulation
void *cl_generate_and_send(void *tdata);
