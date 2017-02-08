/**
* @Author: Izhar Shaikh
* @Date:   2017-02-08T04:51:03-05:00
* @Email:  izharits@gmail.com
* @Filename: calenderFilter.c
* @Last modified by:   Izhar Shaikh
* @Last modified time: 2017-02-08T04:52:31-05:00
*/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "debugMacros.h"
#include "calenderFilter.h"



/* Process the new event create request */
void processEventCreate(stringPool_t **pool, event_t *event)
{
  stringPool_t *poolHandle = *pool;
  int dateIndex = -1;
  event_t earliestEvent;

  // Check if the date is present in the date pool, if not, add to pool
  bool status = getEventDateIndexFromPool(poolHandle, event->date, &dateIndex);
  if((status == FAIL) || (dateIndex == -1))
  {
    dbg_trace("APP: %s is not present in the pool!\n", event->date);
    dbg_trace("APP: Adding %s to the pool now.\n", event->date);
    // it's not present, so add to the pool
    bool addStatus = addEventDateToPool(&poolHandle, event->date);
    if(addStatus == SUCCESS){
      dbg_trace("APP: Successfully added %s to the date pool at index %d.\n",
                            event->date, poolHandle->count - 1);
    }
    dateIndex = poolHandle->count - 1;
  }

  // Check if the new event is already present
  if(dateIndex > -1)
  {
    dbg_trace("APP: List Head at : %p\n", poolHandle->eventList[dateIndex]);
    bool isPresent = isEventPresentInTheList(poolHandle->eventList[dateIndex], event, NULL);
    // If it is already in the list, we know this one's a duplicate, so return
    if(isPresent == true){
      dbg_info("APP: New event is already present in the list. returning.\n");
      goto RETURN;
    }
    else {
      // since it is not present, we add it to the list
      int eventPositionInList = sortedInsert(&poolHandle->eventList[dateIndex], event);
      dbg_trace("APP: Added the event at position %d, List head at: %p\n",
                eventPositionInList, poolHandle->eventList[dateIndex]);
    }
    // Once the event is added to the list, check if it is the earliest on that day
    getEarliestEventOfTheDay(poolHandle->eventList[dateIndex], &earliestEvent);
    if(true == isSameEvent(event, &earliestEvent)){
      dbg_info("APP: Requested event is the earliest event!\n");
      goto PRINT_OUTPUT;
    }
    else {
      goto RETURN;
    }
  }

PRINT_OUTPUT:
  print_output("%s:%s\n", event->date, event->location);

RETURN:
  // Update the poolHandle in the caller before going back
  *pool = poolHandle;
}


/* Process the new event delete request */
void processEventDelete(stringPool_t **pool, event_t *event)
{
  stringPool_t *poolHandle = *pool;
  int dateIndex = -1;
  int eventIndex = -1;
  event_t earliestEvent;

  // Check if the date is present in the date pool, if not, return
  bool status = getEventDateIndexFromPool(poolHandle, event->date, &dateIndex);
  if((status == FAIL) || (dateIndex == -1))
  {
    dbg_trace("APP: %s is not present in the pool! Returning!\n", event->date);
    goto RETURN;
  }
  dbg_trace("APP: List Head at : %p\n", poolHandle->eventList[dateIndex]);
  displayList(poolHandle->eventList[dateIndex]);
  //dbg_trace("Found Index: %d\n", dateIndex);

  // If we have the index, that means the date entry is present in the pool
  if(dateIndex > -1)
  {
    bool isPresent = isEventPresentInTheList(poolHandle->eventList[dateIndex], event, &eventIndex);
    dbg_trace("eventIndex: %d\n", eventIndex);
    // If it is present in the list, we will delete it
    if(isPresent == true)
    {
      dbg_trace("APP: Requested event to be deleted is found at %d position in the list.\n", eventIndex);
      // Before deleting, check if the event to be deleted is the earliestEvent for the day
      getEarliestEventOfTheDay(poolHandle->eventList[dateIndex], &earliestEvent);
      if(true == isSameEvent(event, &earliestEvent))
      {
        dbg_info("APP: Requested event to be deleted is the earliest event!\n");
        // Print to output (Location: None)
        print_output("%s:%s\n", event->date, "None");
      }
      // Delete the node from the list
      deleteNode(&poolHandle->eventList[dateIndex], eventIndex);
    }
    else {
      // since it is not present, we just return
      dbg_info("Requested event to be deleted is not found! Returning!\n");
    }
  }

RETURN:
  // Update the poolHandle in the caller before going back
  *pool = poolHandle;
}


/* Process the new event modify request */
void processEventModify(stringPool_t **pool, event_t *event)
{
  stringPool_t *poolHandle = *pool;
  int dateIndex = -1;
  int eventIndex = -1;
  bool printToOut = false;
  event_t earliestEvent, eventToModify, updatedEvent;

  // Check if the date is present in the date pool, if not, return
  bool status = getEventDateIndexFromPool(poolHandle, event->date, &dateIndex);
  if((status == FAIL) || (dateIndex == -1))
  {
    dbg_trace("APP: %s is not present in the pool! Returning!\n", event->date);
    goto RETURN;
  }
  dbg_trace("APP: List Head at : %p\n", poolHandle->eventList[dateIndex]);
  //displayList(poolHandle->eventList[dateIndex]);
  //dbg_trace("Found Index: %d\n", dateIndex);

  // If we have the index, that means the date entry is present in the pool
  if(dateIndex > -1)
  {
    bool isPresent = getEventIDWithMatchingTitleFromList(poolHandle->eventList[dateIndex],
      event, &eventIndex);
    dbg_trace("eventIndex: %d\n", eventIndex);
    // If it is present in the list, we will modify it
    if(isPresent == true)
    {
      dbg_trace("APP: Requested event to be modified is found at %d position in the list.\n", eventIndex);

      // Before modifying, check if the event to be modified is the earliestEvent for the day
      getEarliestEventOfTheDay(poolHandle->eventList[dateIndex], &earliestEvent);
      getNode(poolHandle->eventList[dateIndex], eventIndex, &eventToModify);

      if((true == isSameEvent(&eventToModify, &earliestEvent)) || (eventIndex == 1))
      {
        dbg_info("APP: Requested event to be modified is the earliest event!\n");
        // Print to output (Set the flag)
        printToOut = true;
      }
      // Modify the node with given event time and location
      // Print to output if it's the earliest
      setNode(poolHandle->eventList[dateIndex], eventIndex, event);
      if(printToOut == true){
        getNode(poolHandle->eventList[dateIndex], eventIndex, &updatedEvent);
        print_output("%s:%s\n", updatedEvent.date, updatedEvent.location);
      }
      // Sort the event list here
      sortEventList(&poolHandle->eventList[dateIndex]);
    }
    else {
      // since it is not present, we just return
      dbg_info("Requested event to be modified is not found! Returning!\n");
    }
  }

RETURN:
  // Update the poolHandle in the caller before going back
  *pool = poolHandle;
}
