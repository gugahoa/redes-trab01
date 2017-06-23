CC = gcc
EXE = server
CFLAGS = -O3 -Wall -lncurses -lpthread

all: *.c *.h
	$(CC) $^ -o $(EXE) $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(EXE)
