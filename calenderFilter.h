/**
* @Author: Izhar Shaikh
* @Date:   2017-02-08T03:15:44-05:00
* @Email:  izharits@gmail.com
* @Filename: calenderFilter.h
* @Last modified by:   Izhar Shaikh
* @Last modified time: 2017-02-08T04:52:03-05:00
*/


#ifndef __CALENDER_FILTER__
#define __CALENDER_FILTER__

#include "debugMacros.h"

#define       MODE_STRING_SIZE            1
#define       TIME_STRING_SIZE            5
#define       DATE_STRING_SIZE            10
#define       LOCATION_STRING_SIZE        10
#define       TITLE_STRING_SIZE           10
#define       SUBJECT_WORD_SKIP_SIZE      9
#define       MAX_STRING_SUBJECT_SIZE     40
#define       MODE_STRING_MAX_SIZE        MODE_STRING_SIZE+1
#define       TIME_STRING_MAX_SIZE        TIME_STRING_SIZE+1  // 10 + 1 (for '\0')
#define       DATE_STRING_MAX_SIZE        DATE_STRING_SIZE+1
#define       LOCATION_STRING_MAX_SIZE    LOCATION_STRING_SIZE+1
#define       TITLE_STRING_MAX_SIZE       TITLE_STRING_SIZE+1

/* Email Bufer size, Pool Size */
#define       BUFFERSIZE                  45
#define       POOL_CAPACITY               30
#define       MIN_BOUND                   35

/* Num threads and min sleep bound */
#define       NUM_THREADS                 2
#define       SLEEP_LIMIT                 5

/* Event Modes */
#define       EVENT_CREATE                'C'
#define       EVENT_DELETE                'D'
#define       EVENT_MODIFY                'X'

// Format of the email
typedef struct eventFormat
{
  char mode;       // C: Create; D: Delete; x: Modify
  char title[TITLE_STRING_MAX_SIZE];
  char location[LOCATION_STRING_MAX_SIZE];
  char date[DATE_STRING_MAX_SIZE];
  char time[TIME_STRING_MAX_SIZE];
} event_t;


// Avents for a particular date
typedef struct Node node_t;

struct Node {
  event_t event;
  node_t *next;
};

// A struct to hold string pool of the event dateStr
typedef struct eventDatePool
{
  char **eventDate;           // <<-- Event dates will be stored here
  node_t **eventList;         // <<-- Event list for every eventDate will be here
  int count;
  int capacity;
} stringPool_t;

// Shared buffer between Producer and Consumer
typedef struct buffer {
  int in;
  int out;
  int capacity;
  char **items;
} circularBuffer_t;

/* Functions for event date pool */
stringPool_t *createStringPool(int capacity);
int addEventDateToPool(stringPool_t **pool, char *newDateString);
void displayEventDatePool(stringPool_t *pool);
void destroyEventDatePool(stringPool_t **pool);
int getEventDateIndexFromPool(stringPool_t *pool, char *stringToFind, int *indexOut);

/* Functions for managing each event list */
int listLength(node_t *head);
void displayList(node_t *head);
void deleteList(node_t **head);
void sortEventList(node_t **head);
int sortedInsert(node_t **head, event_t *event);
int insertNode(node_t **head, int position, event_t *event);
int getNode(node_t *head, int position, event_t *outEvent);
int setNode(node_t *head, int position, event_t *srcEvent);
int deleteNode(node_t **head, int position);

/* Functions for string processing */
void parseEvent(char *buffer, event_t *event);
int isSameString(char *str1, char* str2, int size);
int isSameEvent(event_t *event1, event_t *event2);
int updateEvent(event_t *destEvent, event_t *srcEvent);

/* Functions for event processing */
void displayEvent(event_t *requestedEvent);
int isEarlierInTime(event_t *srcEvent, event_t *destEvent);
int getEarliestEventOfTheDay(node_t *sortedEventList, event_t *OutEvent);
int isEventPresentInTheList(node_t *list, event_t *requestedEvent, int *indexOut);
int getEventIDWithMatchingTitleFromList(node_t *list, event_t *requestedEvent, int *indexOut);

/* Functions for Email Filter */
int isComma(char alpha);
int isValidTime(char *time);
int isValidDate(char *date);
int isValidMode(char mode);
int isValidEmailFormat(char *emailSubject, size_t size);

/* Pthreads Buffer Functions */
circularBuffer_t *createBuffer(int capacity);
void destroyBuffer(circularBuffer_t **bufferToDestroy);

/* Functions for Calender Filter */
void processEventCreate(stringPool_t **pool, event_t *event);
void processEventDelete(stringPool_t **pool, event_t *event);
void processEventModify(stringPool_t **pool, event_t *event);

#endif
