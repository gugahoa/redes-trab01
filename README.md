#How to Run

First, build it with `make` in the root directory.
Then, just run the server:
```
        ./Server
```
And, ideally on another terminal, the client:
```
        ./Client <hostname>
```
With _hostname_ being replaced with the host you'd like to connect to
(_localhost_, for example). Each client you run will spawn 3 threads
that connect to the server via socket and send 15 gibberish messages
with differnt timed intervals (each).
