#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "calenderFilter.h"


/* Creates string pool */
stringPool_t *createStringPool(int capacity)
{
  if(capacity <= 0){
      dbg_info("event date pool size must be greater than 0!\n");
      return NULL;
  }
  // Main struct
  stringPool_t *newPool = (stringPool_t *) malloc(sizeof(stringPool_t));
  if(newPool == NULL)
  {
    dbg_info("Can't create new event date pool. Memory error!\n");
    return NULL;
  }
  newPool->capacity = capacity;
  newPool->count = 0;

  // Assign memory to index pointer
  // NOTE:: this will hold pointer to $capacity 'char*' pointers
  newPool->eventDate = (char **) calloc(capacity, sizeof(char*));
  if(newPool->eventDate == NULL)
  {
    dbg_info("Can't create new eventDate index pointer in event date pool. Memory error!\n");
    return NULL;
  }
  // NOTE:: this will hold pointer to $capacity 'node_t*' pointers
  newPool->eventList = (node_t **) calloc(capacity, sizeof(node_t*));
  if(newPool->eventList == NULL)
  {
    dbg_info("Can't create new eventList index pointer in event date pool. Memory error!\n");
    return NULL;
  }
  while(capacity > 0){
    --capacity;
    newPool->eventDate[capacity] = NULL;
    newPool->eventList[capacity] = NULL;
  }
  return newPool;
}



/* resize the current pool to have double the current size */
static int resizeEventDatePool(stringPool_t **poolHandle)
{
  stringPool_t *oldPoolHandle = *poolHandle;    // Keep the current handle for now

  // Create new pool with double the capacity of current pool
  stringPool_t *newPoolHandle = createStringPool(oldPoolHandle->capacity * 2);
  if(newPoolHandle == NULL){
    dbg_info("Failed to create new pool, during resizing.\n");
    return FAIL;
  }
  newPoolHandle->count = oldPoolHandle->count;    // Copy the old count into new one

  // Copy the old event dates from old pool to new pool
  for(int count = 0; count < oldPoolHandle->count; count++)
  {
    newPoolHandle->eventDate[count] = (char *) calloc(DATE_STRING_MAX_SIZE, sizeof(char));
    if(newPoolHandle->eventDate[count] != NULL){
      strncpy(newPoolHandle->eventDate[count], oldPoolHandle->eventDate[count],
        DATE_STRING_MAX_SIZE);
      dbg_trace("Copied %s from index %d into new pool!\n",
                      oldPoolHandle->eventDate[count], count);
      // copy the eventList heads, into newPool
      newPoolHandle->eventList[count] = oldPoolHandle->eventList[count];
      dbg_trace("Copied List %p from index %d into new pool!\n",
                      (void*)oldPoolHandle->eventList[count], count);
    }
    else {
      destroyEventDatePool(&newPoolHandle);
      return FAIL;
    }
  }
  // Once we know that we have successfully copied old pool into new one,
  // destroy the old pool
  memset(oldPoolHandle, 0, sizeof(stringPool_t));
  destroyEventDatePool(&oldPoolHandle);

  // Update the newPoolHandle in caller thread
  *poolHandle = newPoolHandle;

  return SUCCESS;
}



/* Add new event date to the pool */
int addEventDateToPool(stringPool_t **pool, char *newDateString)
{
  stringPool_t *dPool = *pool;

  if(dPool == NULL)
  {
    dbg_info("Event Date Pool is Empty. Error!\n");
    return FAIL;
  }

  // If the pool is full, we need to resize it
  if(dPool->count == dPool->capacity){
    // Resize pool logic here
    dbg_trace("The pool is full! Resizing pool to new size: %d.\n", dPool->capacity*2);
    int status = resizeEventDatePool(&dPool);
    if(status == FAIL){
      dbg_info("Failed to resize the pool!\n");
      return FAIL;
    }
    dbg_trace("Pool Resized. \n\t New Pool Count: %d, Capacity: %d \n",
                  dPool->count, dPool->capacity);
  }

  char *ptr = (char *) calloc(DATE_STRING_MAX_SIZE, sizeof(char));  // 1 space for '\0'
  if(ptr == NULL){
    dbg_info("Can't allocate memory to new event date in the pool! Error!\n");
    return FAIL;
  }
  dPool->eventDate[dPool->count] = ptr;     // Add the new string pointer to current index
  dPool->eventList[dPool->count] = NULL;    // Add new list head pointer to current index

  // Copy the new string into the eventDate pool
  // NOTE:: Since we know that the date string will always be 10 charaters wide,
  // it is safe to assume that only first 10 characters are to be copied into the
  // event date string pool, null termination has already been taken care of by calloc above
  strncpy(dPool->eventDate[dPool->count], newDateString, DATE_STRING_SIZE);
  dPool->count++;                           // Increase the index count

  // Update the caller pool handle with the updated one
  *pool = dPool;

  return SUCCESS;
}


/* succeeds if the stringToFind is present in the pool; returns it's index via indexOut,
   fails if the stringToFind is not present in the pool; returns -1 via indexOut */
int getEventDateIndexFromPool(stringPool_t *pool, char *stringToFind, int *indexOut)
{
  int count=0;
  stringPool_t *dPool = pool;

  if((dPool == NULL) || (dPool->count <= 0) ||(dPool->eventDate == NULL)){
    dbg_info("Empty Pool. Returning.\n");
    *indexOut = -1;
    return FAIL;
  }
  if(stringToFind == NULL){
    dbg_info("Invalid stringToFind. Returning.\n");
    *indexOut = -1;
    return FAIL;
  }

  bool isPresent = FAIL;
  for(count=0; count < dPool->count; count++)
  {
    //dbg_trace("being compared with: %s, stringToFind: %s\n",
                    //dPool->eventDate[count], stringToFind);
    if(!strcmp(dPool->eventDate[count], stringToFind)){
      dbg_trace("Match found for %s at index %d\n", stringToFind, count);
      *indexOut = count;
      isPresent = SUCCESS;
      break;
    }
  }
  if(isPresent == FAIL){
    *indexOut = -1;
  }
  return isPresent;
}


/* Helper for displaying events */
static void displayEventsHelper(node_t *list)
{
  if(list == NULL){
    return;
  }
  displayList(list);
}

/* Prints the event date pool */
void displayEventDatePool(stringPool_t *pool)
{
  int count = 0;
  stringPool_t *dPool = pool;

  if((dPool == NULL) || (dPool->count <= 0) || (dPool->capacity <= 0)){
    dbg_info("Event Date Pool is Empty. Nothing to Display!\n");
    return;
  }
  dbg_trace("Pool Count: %d, Pool Capacity: %d\n", dPool->count, dPool->capacity);

  for(count = 0; count < dPool->count; count++){
    dbg_trace("Index: %d, String: %s, List Head:%p, Events: %d\n", count,
      dPool->eventDate[count], (void*)dPool->eventList[count],
      listLength(dPool->eventList[count]));
    displayEventsHelper(dPool->eventList[count]);
    //displayEventsHelper(NULL);
  }
}


/* Destroys the event date pool */
void destroyEventDatePool(stringPool_t **datePool)
{
  stringPool_t *pool = *datePool;
  int count = pool->count;
  int capacity = pool->capacity;

  if(pool == NULL){
    dbg_info("Pool already destroyed!\n");
    return;
  }
  // Free the individual string pointers first
  while(pool->count)
  {
    --pool->count;
    if(NULL != pool->eventDate[pool->count])
    {
      free(pool->eventDate[pool->count]);
      pool->eventDate[pool->count] = NULL;
    }
    if(NULL != pool->eventList[pool->count])
    {
      deleteList(&pool->eventList[pool->count]);
      pool->eventList[pool->count] = NULL;
    }
  }
  // Free the string pool handle second
  if(NULL != pool->eventDate){
    free(pool->eventDate);
    pool->eventDate = NULL;
  }
  // Free the string pool handle second
  if(NULL != pool->eventList){
    free(pool->eventList);
    pool->eventList = NULL;
  }
  // Free the event pool handle
  if(NULL != pool){
    free(pool);
    pool = NULL;
  }
  *datePool = NULL;
  dbg_trace("Destroyed event date pool with count %d and capacity %d\n",
                      count, capacity);
}
