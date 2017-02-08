/**
 * @Author: Izhar
 * @Date:   2017-02-07T14:48:18-05:00
 * @Email:  izharits@gmail.com
 * @Filename: pthreadsExample.c
* @Last modified by:   Izhar Shaikh
* @Last modified time: 2017-02-07T22:38:27-05:00
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "debugMacros.h"
#include "pthreadsProducerConsumer.h"


/* creates a new buffer and returns the handle */
circularBuffer_t *createBuffer(int capacity)
{
  // Create new buffer
  circularBuffer_t *newBuffer = (circularBuffer_t *) calloc(1, sizeof(circularBuffer_t));
  if(newBuffer == NULL){
    dbg_info("Failed to create new buffer!\n");
    return NULL;
  }
  newBuffer->in = 0;
  newBuffer->out = 0;
  newBuffer->capacity = capacity;

  // create new handle for items
  char **itemHandle = (char **) calloc(capacity, sizeof(char *));
  if(itemHandle == NULL){
    dbg_info("Failed to create new elements handle!\n");
    free(newBuffer);
    return NULL;
  }
  newBuffer->items = itemHandle;

  // create spaces for items
  for(int i=0; i < capacity; i++)
  {
    newBuffer->items[i] = (char *) calloc(BUFFERSIZE, sizeof(char));
    if(newBuffer->items[i] == NULL)
    {
      dbg_trace("Failed to allocate memory for item %d\n", i);
      while(i > 0)
      {
        --i;
        free(newBuffer->items[i]);
      }
      return NULL;
    }
  }
  return newBuffer;
}



/* destroys the allocated buffer */
void destroyBuffer(circularBuffer_t **bufferToDestroy)
{
  circularBuffer_t *buffer = *bufferToDestroy;
  int capacity = buffer->capacity;

  if(buffer == NULL){
    dbg_info("Buffer already destroyed!");
    return;
  }
  // Free the up the items first
  while(buffer->capacity)
  {
    --buffer->capacity;
    if(NULL != buffer->items[buffer->capacity])
    {
      free(buffer->items[buffer->capacity]);
      buffer->items[buffer->capacity] = NULL;
    }
  }
  // Free up the itemsHandle now
  if(NULL != buffer->items){
    free(buffer->items);
    buffer->items = NULL;
  }
  // Free up the buffer handle now
  if(NULL != buffer){
    free(buffer);
    buffer = NULL;
  }
  *bufferToDestroy = NULL;      // Update caller handle of the buffer
  dbg_trace("Destroyed buffer with capacity of %d.\n", capacity);
}
