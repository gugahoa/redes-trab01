#pragma once

#include <stdint.h>
#include <ncurses.h>

extern WINDOW *whandle;
extern char running;
extern char rupdate;

// Station server socket (once)
int create_socket(uint64_t);

// Listen to virtual sensor gibberish
int listen_and_serve(uint64_t);
