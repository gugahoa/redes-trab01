#include "client.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int main(int argc, char* argv[])
{
    if (argc < 3) {
        printf("Usage is ./client <host-address> <port-number>\n");
        printf("  where <host-address> may be a valid public IP address or\n");
        printf("  \"localhost\", and <port-number> is a 16bit integer corresponding\n");
        printf("  to communication port to bind the sender socket file descriptor to.\n");
        return EXIT_FAILURE;
    }

    const char *hostname = argv[1];  // connect to this guy right here
    int port = atoi(argv[2]);        // bind socket to this port here

    // Plug to host and retrieve descriptors
    int sock1;
    int sock2;
    int sock3;
    if ((sock1 = cl_connect_to_host(hostname, port)) < 0)
        return EXIT_FAILURE;
    if ((sock2 = cl_connect_to_host(hostname, port)) < 0)
        return EXIT_FAILURE;
    if ((sock3 = cl_connect_to_host(hostname, port)) < 0)
        return EXIT_FAILURE;

    // Standard screen
    initscr();
    refresh();

    // ncurses properties
    cbreak();              // Pass everything to me
    noecho();              // Disable character output
    curs_set(0);           // Disable default onscreen cursor
    keypad(stdscr, TRUE);  // Capture all keyboard events

    // Instantiate windows
    whandle = newwin(18, 64, 0, 1);
    vshandle[0] = newwin(11, 20, 6,  3);
    vshandle[1] = newwin(11, 20, 6,  23);
    vshandle[2] = newwin(11, 20, 6,  43);

    for (int ifs = 0; ifs < 3; ifs++)
        scrollok(vshandle[ifs], TRUE);

    // Welcoming info
    mvwprintw(whandle, 1, 1, "[CLIENT] Try to use a wide terminal. Press any key to continue.");
    wrefresh(whandle);

    // Waiting block (this will also allow the thread to run a while)
    wgetch(whandle);
    wclear(whandle);

    // Print boundaries
    box(whandle, 0, 0);

    // Print interface title
    mvwprintw(whandle, 1,  2,   "Socky client interface v1.1. Press `q' to quit.");
    mvwprintw(whandle, 5,  2,   "PSensor1");
    mvwprintw(whandle, 5,  22,  "PSensor2");
    mvwprintw(whandle, 5,  42,  "PSensor3");
    mvwprintw(whandle, 3,  2,   "Status: ");

    // Take it easy
    wattron(whandle, A_STANDOUT);
    mvwprintw(whandle, 3, 11, "Setting up...");
    wattroff(whandle, A_STANDOUT);
    wrefresh(whandle);
    sleep(1);

    // Sensor emulator thread 1 data
    tdata_t *data1 = malloc(sizeof(*data1));
    data1->my_id = 0;
    data1->sock_fd = sock1;
    data1->interval = 999999999;

    // Sensor emulator thread 2 data
    tdata_t *data2 = malloc(sizeof(*data2));
    data2->my_id = 1;
    data2->sock_fd = sock2;
    data2->interval = 800000000;

    // Sensor emulator thread 3 data
    tdata_t *data3 = malloc(sizeof(*data3));
    data3->my_id = 2;
    data3->sock_fd = sock3;
    data3->interval = 600000000;

    cl_err[0] = 0;
    cl_err[1] = 0;
    cl_err[2] = 0;
    pthread_t client[3];

    pthread_mutex_init(&printlock, NULL);
    pthread_create(&(client[0]), NULL, cl_generate_and_send, (void*)data1);
    pthread_create(&(client[1]), NULL, cl_generate_and_send, (void*)data2);
    pthread_create(&(client[2]), NULL, cl_generate_and_send, (void*)data3);

    pthread_mutex_lock(&printlock);
    wattron(whandle, A_STANDOUT);
    mvwprintw(whandle, 3, 11, "Sending data to central.");
    wattroff(whandle, A_STANDOUT);
    wrefresh(whandle);
    pthread_mutex_unlock(&printlock);

    // Blocking loop
    while (wgetch(whandle) != 'q');

    // Murder the thread
    for (int iworker = 0; iworker < 3; iworker++) {
        if (!cl_err[iworker])
            pthread_cancel(client[iworker]);
    }

    // Re-join threads
    pthread_join(client[0], NULL);
    pthread_join(client[1], NULL);
    pthread_join(client[2], NULL);
    pthread_mutex_destroy(&printlock);

    // Close descriptors
    close(sock1);
    close(sock2);
    close(sock3);

    // Free thread data
    free(data1);
    free(data2);
    free(data3);

    // Finalize ncurses context
    for (int ifs = 0; ifs < 3; ifs++)
        delwin(vshandle[ifs]);
    delwin(whandle);
    endwin();

    return EXIT_SUCCESS;
}
