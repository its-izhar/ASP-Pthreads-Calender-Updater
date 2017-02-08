/**
 * @Author: Izhar Shaikh
 * @Date:   2017-02-07T14:48:18-05:00
 * @Email:  izharits@gmail.com
 * @Filename: pthreadsExample.c
* @Last modified by:   Izhar Shaikh
* @Last modified time: 2017-02-07T22:54:10-05:00
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include "debugMacros.h"
#include "pthreadsProducerConsumer.h"


// Global varibales
// Mutex and Condition Variables
pthread_mutex_t lock;
pthread_cond_t full;
pthread_cond_t empty;

int numItems = 0;

#define MAX_STRINGS 6
char *array[MAX_STRINGS] = {"Hello", "Hi","Goodbye!", "Coffee", "Tea", "House"};


static void *producerThread(void *arg)
{
  // Get the handle for the data
  circularBuffer_t *buffer = (circularBuffer_t *) arg;

  while(1)
  {
    sleep(rand() % SLEEP_LIMIT);
    // Get the item
    int itemIndex = rand() % MAX_STRINGS;

    // Critical Section
    pthread_mutex_lock(&lock);

    dbg_info("PRODUCER: Acquired Lock!\n");
    while(numItems == buffer->capacity){
      pthread_cond_wait(&full, &lock);
    }
    dbg_trace("PRODUCER: Writing to buffer...%s\n", array[itemIndex]);
    memset(buffer->items[buffer->in], 0, BUFFERSIZE*sizeof(char));
    strncpy(buffer->items[buffer->in], array[itemIndex], BUFFERSIZE);
    buffer->in = (buffer->in + 1) % buffer->capacity;
    ++numItems;
    dbg_trace("PRODUCER: numItems: %d\n", numItems);

    if(numItems == 1)
      pthread_cond_signal(&empty);
    dbg_info("PRODUCER: Releasing Lock!\n");

    pthread_mutex_unlock(&lock);
    // End
  }
  pthread_exit(NULL);
}

static void *consumerThread(void *arg)
{
  // Get the handle for the data
  circularBuffer_t *buffer = (circularBuffer_t *) arg;

  while(1)
  {
    // Critical Section
    pthread_mutex_lock(&lock);

    dbg_info("CONSUMER: Acquired Lock!\n");
    while(numItems == 0){
      pthread_cond_wait(&empty, &lock);
    }
    dbg_info("CONSUMER: Reading from buffer...\n");
    print_output("%s\n", buffer->items[buffer->out]);
    buffer->out = (buffer->out + 1) % buffer->capacity;
    --numItems;
    dbg_trace("CONSUMER: numItems: %d\n", numItems);

    if(numItems == buffer->capacity-1)
      pthread_cond_signal(&full);
    dbg_info("CONSUMER: Releasing Lock!\n");

    pthread_mutex_unlock(&lock);
    // End
    sleep(rand() % SLEEP_LIMIT);
  }

    pthread_exit(NULL);
}


// ------------------------ main() ------------------------------
int main(int argc, char const *argv[])
{
  pthread_t ThreadID[NUM_THREADS];
  //threadData_t threadData[NUM_THREADS];

  srand(time(NULL)); // Rendom seed

  // Check and parse the command line argument
  if(argc != 2){
    dbg_info("Please enter the buffer size as command line argument!\n");
    return 0;
  }
  int queueSize = atoi((const char *) argv[1]);
  if(queueSize < 1){
    dbg_trace("Invalid Buffer Size: %d, Enter buffer size at least greater than or equal to 1.\n",
                      queueSize);
    return 0;
  }
  dbg_trace("Buffer size, given: %d\n", queueSize);

  circularBuffer_t *buffer = createBuffer(queueSize);
  if(buffer == NULL){
    dbg_info("Memory Error!\n");
    return 0;
  }
  dbg_trace("Buffer Initialized: %d, Size: %zu\n", buffer->capacity, sizeof(*buffer));

  // Initialize the Mutex & Conditions
  int mutexStatus = pthread_mutex_init(&lock, NULL);
  if(mutexStatus != 0){
    dbg_info("Mutex Init Failed!\n");
    return 0;
  }
  int fullCondStatus = pthread_cond_init(&full, NULL);
  if(fullCondStatus != 0){
    dbg_info("Full Condition Init Failed!\n");
    return 0;
  }
  int emptyCondStatus = pthread_cond_init(&empty, NULL);
  if(emptyCondStatus != 0){
    dbg_info("Empty Condition Init Failed!\n");
    return 0;
  }

  for(int i=0; i<NUM_THREADS; i++)
  {
    dbg_trace("Creating thread: %d, %s\n", i, (i==0)? "Producer.":"Consumer.");

    int status = -1;
    // Create and run threads
    if(i==0)
      status = pthread_create(&ThreadID[i], NULL, &producerThread, buffer);
    else
      status = pthread_create(&ThreadID[i], NULL, &consumerThread, buffer);

    if(status != 0) {
      dbg_trace("Failed during creation of the thread: %d\n", i);
    }
  }

  for(int i=0; i<NUM_THREADS; i++) {
    pthread_join(ThreadID[i], NULL);
  }

  // Cleanup
  pthread_mutex_destroy(&lock);
  pthread_cond_destroy(&full);
  pthread_cond_destroy(&empty);
  destroyBuffer(&buffer);

  dbg_info("MAIN: Completed!\n");
  return 0;
}
