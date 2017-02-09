# @Author: Izhar Shaikh
# @Date:   2017-02-07T14:46:06-05:00
# @Email:  izharits@gmail.com
# @Filename: Makefile
# @Last modified by:   Izhar Shaikh
# @Last modified time: 2017-02-08T22:55:10-05:00

# Add the new TARGETS here
TARGETS = calenderUpdater
CC = gcc
HEADERS = -I.
CFLAGS = -Wall -Werror -std=c99 -pthread -O2
#DEBUG_FLAGS = -g -DDEBUG

SOURCES = calenderUpdater.c producerConsumer.c emailFilter.c calenderFilter.c \
	manageBuffer.c stringPool.c eventList.c eventProcessing.c stringProcessing.c

all: clean $(TARGETS)

calenderUpdater:
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(SOURCES) $(HEADERS) -o $@

clean:
	rm -rf $(TARGETS) *.o
