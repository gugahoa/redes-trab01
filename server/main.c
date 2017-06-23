#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage is ./server <port-number>\n");
        printf("  Where <port-number> is a 16bit integer corresponding to the\n");
        printf("  communication port to bind the listener socket descriptor to.\n");
        return EXIT_FAILURE;
    }

    // Standard screen
    initscr();
    refresh();

    // ncurses properties
    cbreak();              // Pass everything to me
    noecho();              // Disable character output
    curs_set(0);           // Disable default onscreen cursor
    keypad(stdscr, TRUE);  // Capture all keyboard events

    // Instantiate windows
    whandle = newwin(32, 126, 0, 1);
    vshandle[0]  = newwin(11, 20, 6,  3);
    vshandle[1]  = newwin(11, 20, 6,  23);
    vshandle[2]  = newwin(11, 20, 6,  43);
    vshandle[3]  = newwin(11, 20, 6,  65);
    vshandle[4]  = newwin(11, 20, 6,  85);
    vshandle[5]  = newwin(11, 20, 6,  105);
    vshandle[6]  = newwin(11, 20, 20, 3);
    vshandle[7]  = newwin(11, 20, 20, 23);
    vshandle[8]  = newwin(11, 20, 20, 43);
    vshandle[9]  = newwin(11, 20, 20, 65);
    vshandle[10] = newwin(11, 20, 20, 85);
    vshandle[11] = newwin(11, 20, 20, 105);

    for (int ivs = 0; ivs < 12; ivs++)
        scrollok(vshandle[ivs], TRUE);

    // Welcoming info
    mvwprintw(whandle, 1, 1, "[SERVER] Try to use a wide terminal. Press any key to continue.");
    wrefresh(whandle);

    // Waiting block (this will also allow the thread to run a while)
    wgetch(whandle);
    wclear(whandle);

    // Print boundaries
    box(whandle, 0, 0);

    // Print interface title
    mvwprintw(whandle, 1,  2,   "Socky server interface v1.1. Press `q' to quit.");
    mvwprintw(whandle, 3,  2,   "VSensor1");
    mvwprintw(whandle, 5,  2,   "Sensor1.1");
    mvwprintw(whandle, 5,  22,  "Sensor1.2");
    mvwprintw(whandle, 5,  42,  "Sensor1.3");
    mvwprintw(whandle, 3,  64,  "VSensor2");
    mvwprintw(whandle, 5,  64,  "Sensor2.1");
    mvwprintw(whandle, 5,  84,  "Sensor2.2");
    mvwprintw(whandle, 5,  104, "Sensor2.3");
    mvwprintw(whandle, 17, 2,   "VSensor3");
    mvwprintw(whandle, 19, 2,   "Sensor1.1");
    mvwprintw(whandle, 19, 22,  "Sensor1.2");
    mvwprintw(whandle, 19, 42,  "Sensor1.3");
    mvwprintw(whandle, 17, 64,  "VSensor4");
    mvwprintw(whandle, 19, 64,  "Sensor2.1");
    mvwprintw(whandle, 19, 84,  "Sensor2.2");
    mvwprintw(whandle, 19, 104, "Sensor2.3");
    mvwprintw(whandle, 1,  64,  "Status: ");

    // Take it easy
    wattron(whandle, A_STANDOUT);
    mvwprintw(whandle, 1, 72, "Setting up...");
    wattroff(whandle, A_STANDOUT);
    wrefresh(whandle);
    sleep(1);

    // Ready server data
    int port = atoi(argv[1]);              // port to bind listener to
    int sock_fd = sv_create_socket(port);  // bind socket to port and get fd

    if (sock_fd < 0) {
        delwin(whandle);
        for (int ivs = 0; ivs < 12; ivs++)
            delwin(vshandle[ivs]);
        return EXIT_FAILURE;
    }

    server_tdata *svdata = malloc(sizeof(*svdata));
    svdata->sock_fd = sock_fd;
    svdata->port = port;

    // Start server thread
    sv_err = 0;
    pthread_t server;
    pthread_create(&server, NULL, sv_listen_and_serve, (void *)svdata);

    // Blocking loop
    while (wgetch(whandle) != 'q');

    // Murder the thread
    if (!sv_err)
        pthread_cancel(server);
    pthread_join(server, NULL);
    free(svdata);

    // Finalize ncurses context
    for (int ivs = 0; ivs < 12; ivs++)
        delwin(vshandle[ivs]);
    delwin(whandle);
    endwin();

    // Exit like whatever
    return EXIT_SUCCESS;
}
