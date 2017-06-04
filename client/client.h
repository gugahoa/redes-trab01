#pragma once

#include <string.h>
#include <stdint.h>
#include <pthread.h>

typedef struct TDATA_T {

        int      socket;
        size_t   nsends;
        uint64_t interval;

} tdata_t;

int connect_host(const char* host_addr, uint16_t port);
int send_msg(int socket, const char* msg, size_t size);

pthread_t dummy_msg_sender(int socket, size_t nsends, uint64_t interval);
