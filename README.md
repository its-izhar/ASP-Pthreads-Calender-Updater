<!--
@Author: Izhar Shaikh
@Date:   2017-02-08T23:44:53-05:00
@Email:  izharits@gmail.com
@Filename: README.md
@Last modified by:   Izhar Shaikh
@Last modified time: 2017-02-08T23:48:09-05:00
-->


# ASP_Pthreads
Advanced Systems Programming: Assignment 2

The program has been divided in several source files.
The description is as follows:

calenderUpdater.c   : Main function resides here. This is the main app.

producerConsumer.c  : Has two thread functions "producerThread" (emailFilter) & "consumerThread" (calenderFilter),
                      producer is atomically writing to and consumer is atomically reading from circularBuffer,
                      managed by manageBuffer.c.

manageBuffer.c  :     Has function for creating a dynamic circular buffer and to destroy it.

emailFilter.c :       Functions associated with checking the valid email formats and producing filtered email output.

calenderFilter.c  :   Functions associated with dynamically updating the calender events in order that they
                      are produced from emailFilter (producerThread) from the circularBuffer.

stringPool.c  :       Functions associated with managing a pool of events, it maintains a global pool of all the events.
                      It maintains a structure stringPool_t which has date entries of all the dates, and a linked list for
                      each such entries in the pool. All the events are stored in a "time" sorted order in these lists.  

eventProcessing.c :   Basic functions to compare events, check which one is earlier in time, get the latest event of the day etc.

eventList.c :         Functions required for linked list: sortedInsert, deleting events, adding or updating events,
                      creating and/or destroying linked lists.

stringProcessing.c  : Functions for parsing the given input email into events, and other string manipulating functions.
