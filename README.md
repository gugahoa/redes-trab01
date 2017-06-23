#How to Run

First, build it with `make` in the root directory. It will `cd` into each
subfolder (server and client) and run their respective Makefiles. Then, just
run the server:
```
        ./Server <port-number>
```
and then, on another terminal, the client:
```
        ./Client <hostname> <port-number>
```
With _hostname_ being replaced with the host you'd like to connect to
(_localhost_, for example) and _port-number_ obviously being replaced with the
port number to bind the socket FD's to.


Up to 4 clients can run at the same time
(remember to spawn them on different terminal windows, so that the ncurses UI
work properly). Each client you run will spawn 3 threads that connect to the
server via socket and keep sending gibberish messages, (random numbers) with
different timed intervals (each), until you quit the interface.

#Group

- Danilo Zecchin Nery, 8602430
- Gustavo Henrique Oliveira Aguiar, 8936912
