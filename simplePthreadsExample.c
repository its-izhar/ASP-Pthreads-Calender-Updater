/**
 * @Author: Izhar
 * @Date:   2017-02-07T14:48:18-05:00
 * @Email:  izharits@gmail.com
 * @Filename: pthreadsExample.c
* @Last modified by:   Izhar
* @Last modified time: 2017-02-07T16:31:26-05:00
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "debugMacros.h"

#define NUM_THREADS 5
#define SLEEP_LIMIT 20

typedef struct threadData {
  int tid;
  unsigned int sleepTime;
} threadData_t;


void *printHello(void *arg)
{
  // Get the handle for the data
  threadData_t *data = (threadData_t *) arg;

  dbg_trace("Thread %d: Sleeping for %d seconds!\n", data->tid, data->sleepTime);
  sleep(data->sleepTime);
  dbg_trace("Thread %d: Woke up after %d seconds!\n", data->tid, data->sleepTime);

  pthread_exit(&data->tid);
}


int main(int argc, char const *argv[])
{
  pthread_t ThreadID[NUM_THREADS];
  threadData_t threadData[NUM_THREADS];
  int *threadStatus[NUM_THREADS];

  srand(time(NULL)); // Rendom seed

  for(int i=0; i<NUM_THREADS; i++)
  {
    dbg_trace("Creating thread: %d\n", i);

    int rVal = rand() % SLEEP_LIMIT;
    if(rVal == 0) {
            rVal = SLEEP_LIMIT/2;
    }
    threadData[i].tid = i;
    threadData[i].sleepTime = rVal;

    // Create and run threads
    int status = pthread_create(&ThreadID[i], NULL, &printHello, &threadData[i]);
    if(status != 0) {
            dbg_trace("Failed during creation of the thread: %d\n", i);
    }
  }

  for(int i=0; i<NUM_THREADS; i++)
  {
    pthread_join(ThreadID[i], (void **)&threadStatus[i]);
    dbg_trace("MAIN: Thread %d returned with EXIT_SUCCESS status!\n", **((int**)&threadStatus[i]));
  }

  return 0;
}
