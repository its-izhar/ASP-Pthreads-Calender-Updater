/**
 * @Author: Izhar Shaikh
 * @Date:   2017-02-07T14:48:18-05:00
 * @Email:  izharits@gmail.com
 * @Filename: pthreadsExample.c
* @Last modified by:   Izhar Shaikh
* @Last modified time: 2017-02-08T22:12:13-05:00
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include "debugMacros.h"
#include "calenderFilter.h"


// Global varibales
// Mutex and Condition Variables
pthread_t ThreadID[NUM_THREADS];
pthread_mutex_t lock;
pthread_cond_t full;
pthread_cond_t empty;

int numItems = 0;
bool producerDone = false;

// declare that the producer and consumer thread functions are extern
extern void *producerThread(void *);
extern void *consumerThread(void *);

// ------------------------ main() ------------------------------
int main(int argc, char const *argv[])
{
  // Rendom seed for random generator
  srand(time(NULL));

  // Check and parse the command line argument
  if(argc != 2){
    dbg_info("Please enter the buffer size as command line argument!\n");
    return 0;
  }
  int inSize = atoi((const char *) argv[1]);
  if(inSize < 1){
    dbg_trace("Invalid Buffer Size: %d,\
     Enter buffer size at least greater than or equal to 1.\n",
                      inSize);
    return 0;
  }
  dbg_trace("Buffer size, given: %d\n", inSize);

  circularBuffer_t *buffer = createBuffer(inSize);
  if(buffer == NULL){
    dbg_info("Memory Error!\n");
    return 0;
  }
  dbg_trace("Buffer Initialized: %d.\n",buffer->capacity);

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

  // Get the function pointers to producer and consumer thread
  threadFunction_t emailFilter = &producerThread;
  threadFunction_t calenderFilter = &consumerThread;

  // Create and run threads
  for(int i=0; i<NUM_THREADS; i++)
  {
    dbg_trace("Creating thread: %d, %s\n", i, (i==0)? "Producer.":"Consumer.");
    int status = -1;
    if(i==0){
      status = pthread_create(&ThreadID[i], NULL, emailFilter, buffer);
    }
    else {
      status = pthread_create(&ThreadID[i], NULL, calenderFilter, buffer);
    }
    if(status != 0) {
      dbg_trace("Failed during creation of the thread: %d, %s\n", i,
                    (i==0)? "Producer.":"Consumer");
    }
  }

  // Wait until threads finish their jobs
  for(int i=0; i<NUM_THREADS; i++) {
    pthread_join(ThreadID[i], NULL);
    dbg_trace("MAIN: Thread %d [%s] terminated successfully!\n", i,
            (i==0)? "Producer":"Consumer");
  }
  // Cleanup
  pthread_mutex_destroy(&lock);
  pthread_cond_destroy(&full);
  pthread_cond_destroy(&empty);
  destroyBuffer(&buffer);

  dbg_info("MAIN: Completed!\n");
  return 0;
}
