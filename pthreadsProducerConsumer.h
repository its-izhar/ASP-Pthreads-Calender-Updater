/**
 * @Author: Izhar Shaikh
 * @Date:   2017-02-07T14:48:18-05:00
 * @Email:  izharits@gmail.com
 * @Filename: pthreadsExample.c
* @Last modified by:   Izhar Shaikh
* @Last modified time: 2017-02-07T21:44:06-05:00
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <ctype.h>
#include "debugMacros.h"

#define NUM_THREADS 2
#define SLEEP_LIMIT 5
#define BUFFERSIZE  10

typedef struct threadData {
  int tid;
  unsigned int sleepTime;
} threadData_t;

typedef struct buffer {
  int in;
  int out;
  int capacity;
  char **items;
} circularBuffer_t;

// Buffer Functions
circularBuffer_t *createBuffer(int capacity);
void destroyBuffer(circularBuffer_t **bufferToDestroy);
