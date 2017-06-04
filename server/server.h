#pragma once

#include <stdint.h>

// Station server socket (once)
int create_socket(uint64_t);

// Listen to virtual sensor gibberish
int listen_and_serve(uint64_t);
