#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>

WINDOW *whandle;
char running = 1;
char rupdate = 1;

int main(int argc, char *argv[])
{
        // Standard screen
        initscr();
        refresh();

        // Some shit
        cbreak();    // Pass everything to me
        noecho();    // Disable character output
        curs_set(0); // Disable default onscreen cursor

        // Defaults
        whandle = newwin(128, 60, (LINES-128)/2, (COLS-60)/2);
        scrollok(whandle, TRUE); // yeeeeeeah

        // Enable keyboard
        keypad(whandle, TRUE);

        // Renderloop
        while (wgetch(whandle) != 'q') {

                if (rupdate) {
                        // Print boundaries
                        box(whandle, 0, 0);

                        // Print interface title
                        wattron(whandle, A_STANDOUT);
                        mvwprintw(whandle, 0, 0, " Socky Interface ");
                        wattroff(whandle, A_STANDOUT);

                        wrefresh(whandle);
                        rupdate = 0;
                }
        }

        // Goodbye server
        running = 0;

        // Goodbye ncurses
        delwin(whandle);
        endwin();

        // TODO HERE

        // Server logic
        printf("Creating server-side socket...\n");

        int sock = create_socket(1234);

        if (sock < 0)
                return EXIT_FAILURE;

        listen_and_serve(sock);

        shutdown(sock, 2);

        return EXIT_SUCCESS;
}
