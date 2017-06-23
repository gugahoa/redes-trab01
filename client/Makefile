CC = gcc
EXE = client
CFLAGS = -O3 -Wall -lncurses -lpthread

all: *.c *.h
	$(CC) $^ -o $(EXE) $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(EXE)
