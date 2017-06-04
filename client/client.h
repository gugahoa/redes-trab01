#pragma once

#include <stdlib.h>
#include <stdint.h>

typedef struct TDATA_T {

        int      socket;
        int      nsends;
        uint64_t interval;

} tdata_t;

// Connect to host of name 'host_addr' through port 'port'
int connect_host(const char* host_addr, uint16_t port);

// Send message 'msg' of size 'size' through socket 'socket'
int send_msg(int socket, const char* msg, size_t size);

// Gibberish messsage sender function for the sensor emulator threads
void *dummy_sender_func(void *tdata);
