#include <stdlib.h>
#include <stdbool.h>
#include "calenderFilter.h"



/* returns true if the srcEvent is earlier in time than destEvent */
int isEarlierInTime(event_t *srcEvent, event_t *destEvent)
{
  int srcEventHH=0, srcEventMM=0, destEventHH=0, destEventMM=0;
  bool isEarlierInTime = false;
  char *srcPtr=NULL, *destPtr=NULL;

  if ((srcEvent == NULL) || (destEvent == NULL)){
    dbg_info("Invalid Event Pointers!\n");
    return isEarlierInTime;
  }
  // srcPtr and destPtr will have addressess of ':' characters from the date string
  // after the first two calls to strtol
  srcEventHH = (int) strtol(srcEvent->time, &srcPtr, 10);
  destEventHH = (int) strtol(destEvent->time, &destPtr, 10);
  ++srcPtr;
  ++destPtr;
  srcEventMM = (int) strtol(srcPtr, NULL, 10);
  destEventMM = (int) strtol(destPtr, NULL, 10);

  if(srcEventHH < destEventHH){
    isEarlierInTime = true;
  }
  else if(srcEventHH == destEventHH) {
    if(srcEventMM < destEventMM){
      isEarlierInTime = true;
    }
  }
  return isEarlierInTime;
}


/* returns the earliest event of a perticular day */
int getEarliestEventOfTheDay(node_t *sortedEventList, event_t *OutEvent)
{
  if(sortedEventList == NULL){
    return FAIL;
  }
  // since we know that the event list is already sorted, the earliest event of
  // a perticular day will be at it's head position, i.e. position 1,
  // so we simply return that
  return getNode(sortedEventList, 1, OutEvent);
}


/* returns true if the requestedEvent is present in the given list */
int isEventPresentInTheList(node_t *list, event_t *requestedEvent, int *indexOut)
{
  int iterator = 1;
  if(indexOut != NULL) {*indexOut = -1;}
  bool isEventPresentInTheList = false;
  int LLength = listLength(list);     // will iterate till this bound
  event_t iteratorEvent;

  if(list == NULL){
    dbg_info("Event list is empty!\n");
    return isEventPresentInTheList;
  }

  for(iterator=1; iterator <= LLength; iterator++)
  {
    getNode(list, iterator, &iteratorEvent);
    if(true == isSameEvent(requestedEvent, &iteratorEvent))
    {
      if(indexOut != NULL) {*indexOut = iterator;}
      isEventPresentInTheList = true;
      break;
    }
  }
  if(isEventPresentInTheList == true){
    dbg_trace("Requested Event: %s,%s,%s,%s\n",
        requestedEvent->title, requestedEvent->date,
        requestedEvent->time, requestedEvent->location);
    dbg_trace("%s","\tDuplicates found at indexes: \n");
    dbg_trace("\t\t %d: %s,%s,%s,%s\n", iterator,
      iteratorEvent.title, iteratorEvent.date,
      iteratorEvent.time, iteratorEvent.location);
  }
  return isEventPresentInTheList;
}


/* returns true if the requestedEvent->title is present in the given list,
   with the index of that event in indexOut */
int getEventIDWithMatchingTitleFromList(node_t *list, event_t *requestedEvent, int *indexOut)
{
  int iterator = 1;
  if(indexOut != NULL) {*indexOut = -1;}
  bool isEventPresentInTheList = false;
  int LLength = listLength(list);     // will iterate till this bound
  event_t iteratorEvent;

  if(list == NULL){
    dbg_info("Event list is empty!\n");
    return isEventPresentInTheList;
  }

  for(iterator=1; iterator <= LLength; iterator++)
  {
    getNode(list, iterator, &iteratorEvent);
    if(true == isSameString(requestedEvent->title, iteratorEvent.title, TITLE_STRING_SIZE))
    {
      if(indexOut != NULL) {*indexOut = iterator;}
      isEventPresentInTheList = true;
      break;
    }
  }
  if(isEventPresentInTheList == true){
    dbg_trace("Requested Event Title: %s\n", requestedEvent->title);
    dbg_trace("%s","\tDuplicates found at indexes: \n");
    dbg_trace("\t\t %d: %s,%s,%s,%s\n", iterator,
      iteratorEvent.title, iteratorEvent.date,
      iteratorEvent.time, iteratorEvent.location);
  }
  return isEventPresentInTheList;
}


void displayEvent(event_t *requestedEvent)
{
  dbg_trace("Event: %s,%s,%s,%s\n",
      requestedEvent->title, requestedEvent->date,
      requestedEvent->time, requestedEvent->location);
}
