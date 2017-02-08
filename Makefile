# @Author: Izhar Shaikh
# @Date:   2017-02-07T14:46:06-05:00
# @Email:  izharits@gmail.com
# @Filename: Makefile
# @Last modified by:   Izhar Shaikh
# @Last modified time: 2017-02-08T05:18:23-05:00

# Add the new TARGETS here
TARGETS = simplePthreadsExample pthreadsProducerConsumer
CC = gcc
HEADERS = -I.
BIN_PATH = bin
CFLAGS = -Wall -Werror -std=c99 -pthread -Og -g -DDEBUG

all: clean create_bin $(TARGETS)

simplePthreadsExample:
	$(CC) $(CFLAGS) simplePthreadsExample.c $(HEADERS) -o $(BIN_PATH)/$@

pthreadsProducerConsumer:
	$(CC) $(CFLAGS) pthreadsProducerConsumer.c emailFilter.c calenderFilter.c \
	manageBuffer.c stringPool.c eventList.c eventProcessing.c stringProcessing.c \
	$(HEADERS) -o $(BIN_PATH)/$@

clean:
	rm -rf $(TARGETS) $(BIN_PATH) *.o

create_bin:
	mkdir $(BIN_PATH)
