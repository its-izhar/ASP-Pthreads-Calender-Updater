# Makefile to build the assignment 1
# by Izhar Shaikh

# Add the new TARGETS here
TARGETS = pthreadsExample
CC = gcc
HEADERS = -I.
BIN_PATH = bin
CFLAGS = -Wall -Werror -std=c99 -g -O0 -DDEBUG -pthread

all: clean create_bin $(TARGETS)

#$(TARGETS):
#	$(CC) $(CFLAGS) $@.c stringPool.c $(HEADERS) -o $(BIN_PATH)/$@

pthreadsExample:
	$(CC) $(CFLAGS) pthreadsExample.c $(HEADERS) -o $(BIN_PATH)/$@

clean:
	rm -rf $(TARGETS) $(BIN_PATH) *.o

create_bin:
	mkdir $(BIN_PATH)
