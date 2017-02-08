# @Author: Izhar Shaikh
# @Date:   2017-02-07T14:46:06-05:00
# @Email:  izharits@gmail.com
# @Filename: Makefile
# @Last modified by:   Izhar Shaikh
# @Last modified time: 2017-02-07T23:01:45-05:00

# Add the new TARGETS here
TARGETS = simplePthreadsExample pthreadsProducerConsumer
CC = gcc
HEADERS = -I.
BIN_PATH = bin
CFLAGS = -Wall -Werror -std=c99 -g -O0 -DDEBUG -pthread

all: clean create_bin $(TARGETS)

simplePthreadsExample:
	$(CC) $(CFLAGS) simplePthreadsExample.c $(HEADERS) -o $(BIN_PATH)/$@

pthreadsProducerConsumer:
	$(CC) $(CFLAGS) pthreadsProducerConsumer.c manageBuffer.c $(HEADERS) -o $(BIN_PATH)/$@

clean:
	rm -rf $(TARGETS) $(BIN_PATH) *.o

create_bin:
	mkdir $(BIN_PATH)
