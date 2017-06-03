#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <string.h>
#include <stdint.h>

int connect_host(const char*, uint16_t);
int send_msg(int, const char*, size_t);
int send_random(int);
void send_random_interval(int, uint32_t);

#endif
