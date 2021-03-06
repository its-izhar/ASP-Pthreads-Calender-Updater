/**
 * @Author: Izhar Shaikh
 * @Date:   2017-02-07T14:48:18-05:00
 * @Email:  izharits@gmail.com
 * @Filename: pthreadsExample.c
* @Last modified by:   Izhar Shaikh
* @Last modified time: 2017-02-08T23:17:03-05:00
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include "debugMacros.h"
#include "calenderFilter.h"


// Global varibales
// Mutex and Condition Variables
extern pthread_t ThreadID[NUM_THREADS];
extern pthread_mutex_t lock;
extern pthread_cond_t full;
extern pthread_cond_t empty;

extern int numItems;
extern bool producerDone;


/* Producer */
void *producerThread(void *arg)
{
  int minBound = BUFFERSIZE - 10;   // 35
  char *inBuffer = NULL;
  char *validString = NULL;
  size_t inBufferLength = 0;
  ssize_t readSize;
  bool isValidString = false;
  int totalStringsCount = 0, validStringsCount = 0;
  int subjectWordSkipSpace = SUBJECT_WORD_SKIP_SIZE; // To Skip "Subject:" String

  // Get the handle for the data
  circularBuffer_t *buffer = (circularBuffer_t *) arg;
  // Read a line from stdin and process it
  while((readSize = getline(&inBuffer, &inBufferLength, stdin)) != -1)
  {
    // Replace newline
    if( inBuffer[readSize - 1] == '\n' ) {
      inBuffer[readSize - 1] = '\0';
    }
    ++totalStringsCount;
    dbg_trace("PRODUCER: String [size: %zu]: %s\n", strlen(inBuffer), inBuffer);

    // Check for valid strings based on the format
    isValidString = false;
    if(readSize >= minBound) {
      if (isValidEmailFormat(inBuffer, readSize)) {
        ++validStringsCount;
        isValidString = true;
        //print_output("%s\n", inBuffer + subjectWordSkipSpace);
      }
    }
    // If the string is less than minimum bound or not valid, just read the next one
    if(isValidString == false){
      validString = NULL;
      memset(inBuffer, 0, inBufferLength); // Reset the buffer to get next line
      continue;
    }
    // Get the handle to the valid string
    validString = inBuffer + subjectWordSkipSpace;

    // ---- Enter: Critical Section
    pthread_mutex_lock(&lock);
      dbg_info("PRODUCER: Acquired Lock!\n");
      while(numItems == buffer->capacity){
        pthread_cond_wait(&full, &lock);
      }
      dbg_trace("PRODUCER: Writing to buffer...%s\n", validString);
      memset(buffer->items[buffer->in], 0, BUFFERSIZE*sizeof(char));
      strncpy(buffer->items[buffer->in], validString, BUFFERSIZE);
      // NOTE:: Enable the following line to check producer's output
      //print_output("%s\n", validString);
      buffer->in = (buffer->in + 1) % buffer->capacity;
      ++numItems;
      dbg_trace("PRODUCER: numItems: %d\n", numItems);

      if(numItems == 1)
        pthread_cond_signal(&empty);
      dbg_info("PRODUCER: Releasing Lock!\n");
    pthread_mutex_unlock(&lock);
    // ---- Exit: Critical Section

    // Reset the buffer, before proceeding
    memset(inBuffer, 0, inBufferLength);
  }
  // Free the inBuffer
  free(inBuffer);
  inBuffer = NULL;
  // Printing the count of total strings and the valid strings
  dbg_trace("PRODUCER: Total Strings: %d, Valid Strings: %d\n",
            totalStringsCount, validStringsCount);

  // Ask the consumer to finish
  pthread_mutex_lock(&lock);
    dbg_info("PRODUCER: Asked the consumer to finish!\n");
    producerDone = true;
    pthread_cond_signal(&empty);
  pthread_mutex_unlock(&lock);

  dbg_info("PRODUCER: EXIT!\n");
  pthread_exit(NULL);
}



/* Consumer */
void *consumerThread(void *arg)
{
  char outBuffer[BUFFERSIZE] = { 0 };    // Buffer to hold a line of the string
  event_t newEvent;                      // Event to hold new event entry
  stringPool_t *eventPool = NULL;        // Event Pool according to dates
  int totalStringsCount = 0;

  // create the string pool
  eventPool = createStringPool(POOL_CAPACITY);
  // Reset the outBuffer
  memset(outBuffer, 0, BUFFERSIZE);
  // Get the handle for the data
  circularBuffer_t *buffer = (circularBuffer_t *) arg;

  while(1)
  {
    // Enter: Critical Section
    pthread_mutex_lock(&lock);
      dbg_info("CONSUMER: Acquired Lock!\n");
      while(numItems == 0 && producerDone == false ){
        pthread_cond_wait(&empty, &lock);
      }
      if(numItems == 0 && producerDone == true){
        dbg_info("CONSUMER: Released Lock!\n");
        pthread_mutex_unlock(&lock);
        break;
      }
      dbg_info("CONSUMER: Reading from buffer...\n");
      //print_output("%s\n", buffer->items[buffer->out]);
      strncpy(outBuffer, buffer->items[buffer->out], BUFFERSIZE);
      outBuffer[BUFFERSIZE-1] = '\0';          // Force the end of the string
      buffer->out = (buffer->out + 1) % buffer->capacity;
      --numItems;
      dbg_trace("CONSUMER: numItems: %d\n", numItems);

      if(numItems == buffer->capacity-1)
        pthread_cond_signal(&full);
      dbg_info("CONSUMER: Releasing Lock!\n");
    pthread_mutex_unlock(&lock);
    // Exit: Critical Section

    // Processing begins
    ++totalStringsCount;
    dbg_trace("CONSUMER: String [size: %d]: %s\n", (int) strlen(outBuffer), outBuffer);
    // We assume that the given string is valid, so we parse
    parseEvent(outBuffer, &newEvent);

    // Process Events
    switch(newEvent.mode)
    {
      case EVENT_CREATE:
        processEventCreate(&eventPool, &newEvent);
        break;

      case EVENT_DELETE:
        processEventDelete(&eventPool, &newEvent);
        break;

      case EVENT_MODIFY:
        processEventModify(&eventPool, &newEvent);
        break;

      default:
        dbg_info("CONSUMER: Unknown Mode! Error!\n");
    }

    // Clear the outBuffer before reading next string
    memset(&newEvent, 0, sizeof(event_t));
    memset(outBuffer, 0, BUFFERSIZE);
    dbg_info("\n");
  }
  // Printing the count of total strings and the valid strings
  dbg_trace("CONSUMER: Total Strings: %d\n\n", totalStringsCount);
  // Display the contents of the string pool
  displayEventDatePool(eventPool);
  // destroy the pool
  destroyEventDatePool(&eventPool);

  dbg_info("CONSUMER: EXIT!\n");
  pthread_exit(NULL);
}
