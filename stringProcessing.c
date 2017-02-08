#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "calenderFilter.h"


// Parse the given the string from the buffer to event_t data struct
void parseEvent(char *buffer, event_t *event)
{
  char eventString[BUFFERSIZE] = {0};
  const char delimiter[2] = ",";
  char *token;
  int pass = 0;

  // Make a copy of the original buffer as strtok will modify it otherwise
  strncpy(eventString, buffer, BUFFERSIZE);
  memset(event, '\0', sizeof(event_t));

  /* get the first token (which is mode in our case) */
  token = strtok(eventString, delimiter);
  event->mode = token[0];

  /* walk through other tokens */
  while( token != NULL )
  {
    ++pass;
    token = strtok(NULL, delimiter);

    if(pass == 1) {
      strcpy(event->title, token);
      event->title[TITLE_STRING_SIZE] = '\0';
    }
    else if(pass == 2) {
      strcpy(event->date, token);
      event->date[DATE_STRING_SIZE] = '\0';
    }
    else if(pass == 3) {
      strcpy(event->time, token);
      event->time[TIME_STRING_SIZE] = '\0';
    }
    else if(pass == 4) {
      strcpy(event->location, token);
      event->location[LOCATION_STRING_SIZE] = '\0';
    }
  }
}

/* returns true if size bytes of str1 are equal to str2 */
int isSameString(char *str1, char *str2, int size)
{
  return !strncmp(str1, str2, size);
}

/* returns true if the two requested events are same */
int isSameEvent(event_t *event1, event_t *event2)
{
  bool isSame = false;

  if ((event1 == NULL) || (event2 == NULL)){
    dbg_info("Invalid Event Pointers!\n");
    isSame = false;
    return isSame;
  }

  if(isSameString(event1->title, event2->title, TITLE_STRING_SIZE)
      && isSameString(event1->date, event2->date, DATE_STRING_SIZE)
      && isSameString(event1->time, event2->time, TIME_STRING_SIZE)
      && isSameString(event1->location, event2->location, LOCATION_STRING_SIZE))
      {
        //dbg_info("event1 and event2 are same!\n");
        isSame = true;
      }

  return isSame;
}


/* updates the destEvent with srcEvent  */
int updateEvent(event_t *destEvent, event_t *srcEvent)
{
  if ((srcEvent == NULL) || (destEvent == NULL)){
    dbg_info("Invalid Event Pointers!\n");
    return FAIL;
  }
  dbg_info("BEFORE:\n");
  displayEvent(destEvent);

  // Reset the destEvent time and location fields
  memset(destEvent->time, '\0', TIME_STRING_MAX_SIZE);
  memset(destEvent->location, '\0', LOCATION_STRING_MAX_SIZE);

  strncpy(destEvent->time, srcEvent->time, TIME_STRING_SIZE);
  destEvent->time[TIME_STRING_SIZE] = '\0';
  strncpy(destEvent->location, srcEvent->location, LOCATION_STRING_SIZE);
  destEvent->location[LOCATION_STRING_SIZE] = '\0';

  dbg_info("AFTER:\n");
  displayEvent(destEvent);

  return SUCCESS;
}
