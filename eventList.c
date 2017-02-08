#include <stdlib.h>
#include <string.h>
#include "calenderFilter.h"
#include "debugMacros.h"



/* returns the list of the length */
int listLength(node_t *head)
{
  node_t *current = head;
  int count = 0;

  while(current != NULL)
  {
    ++count;
    current = current->next;
  }
  return count;
}



/* displays the contents of the list */
void displayList(node_t *head)
{
  node_t *current = head;

  //print_output("\t%s\n", "***List contents:");
  while(current != NULL)
  {
    dbg_trace("\tMode: %c, Title: %s, Date: %s, Time: %s, Location: %s\n",
      current->event.mode, current->event.title, current->event.date,
      current->event.time, current->event.location);

    current = current->next;
  }
  //print_output("\t%s\n","***End!");
}



/* insert elements in the list */
int insertNode(node_t **head, int position, event_t *event)
{
  node_t *newNode, *previousNode, *positionNode;
  int count=1;

  // get the current head of the list
  positionNode = *head;
  previousNode = *head;

  // Error checking
  if(position < 1){
    dbg_trace("Invalid position: %d. Must be greater than or equal to 1.\n", position);
    return FAIL;
  }

  // Get the new node and store the data in it
  newNode = (node_t *) calloc(1, sizeof(node_t));
  if(newNode == NULL){
    dbg_info("Can't allocate memory for new node! Exiting!\n");
    return FAIL;
  }
  memcpy(&newNode->event, event, sizeof(event_t));
  newNode->next = NULL;

  // Inserting at the beginning of the list
  if(position == 1)
  {
    newNode->next = positionNode;         // newNode points to positionNode now
    *head = newNode;                      // New node is head now
  }
  else
  {
    // Traverse until we find the desired position of the node or reach the end,
    // insert the node at the position of whichever earliest
    while((positionNode != NULL) && (count < position))
    {
      count++;
      previousNode = positionNode;
      positionNode = positionNode->next;
    }
    previousNode->next = newNode;
    newNode->next = positionNode;
  }
  dbg_trace("inserted '%s:%s' at position %d in '%s' list.\n",
    newNode->event.title, newNode->event.time, position, newNode->event.date);

  return SUCCESS;
}


/* Insert events into the list in an sorted timed order for some date */
int sortedInsert(node_t **head, event_t *event)
{
  int position = -1;
  // Get the new node and store the data in it
  node_t *newNode = (node_t *) calloc(1, sizeof(node_t));
  if(newNode == NULL){
    dbg_info("Can't allocate memory for new node! Exiting!\n");
    return FAIL;
  }
  memcpy(&newNode->event, event, sizeof(event_t));
  newNode->next = NULL;

  // Special case for the head end
  if (*head == NULL || isEarlierInTime(&newNode->event, &(*head)->event)) {
    newNode->next = *head;
    *head = newNode;
    position = 1;
    return position;
  }
  else {
    // Locate the node before the point of insertion
    node_t *current = *head;
    position = 1;
    while (current->next != NULL && isEarlierInTime(&current->next->event, &newNode->event)) {
      current = current->next;
      position++;
    }
    newNode->next = current->next;
    current->next = newNode;
    position++;
  }
  return position;
}


// Helper function for sortEventList(): adds events into the list in an sorted order
static void sortEventListHelper(node_t **head, node_t *newNode)
{
  // Special case for the head end
  if (*head == NULL || isEarlierInTime(&newNode->event, &(*head)->event)) {
    newNode->next = *head;
    *head = newNode;
  }
  else {
    // Locate the node before the point of insertion
    node_t *current = *head;
    while (current->next != NULL && isEarlierInTime(&current->next->event, &newNode->event)) {
      current = current->next;
    }
    newNode->next = current->next;
    current->next = newNode;
  }
}

// Given a list, change it to be in sorted order (using sortEventListHelper()).
void sortEventList(node_t **head)
{
  node_t *result = NULL;         // build the answer here
  node_t *current = *head;    // iterate over the original list
  node_t *next;

  while (current != NULL) {
    next = current->next; // tricky - note the next pointer before we change it
    sortEventListHelper(&result, current);
    current = next;
  }
  // Update the list head in the caller
  *head = result;
}


/* deletes elements from the list */
int deleteNode(node_t **head, int position)
{
  node_t *previousNode, *positionNode;
  int count=1;

  // get the current head of the list
  positionNode = *head;

  // Error checking
  if(positionNode == NULL){
    dbg_info("List is already empty!\n");
    return FAIL;
  }
  if(position < 1){
    dbg_trace("Invalid position: %d. Must be greater than or equal to 1.\n", position);
    return FAIL;
  }

  // Deleting the node from the beginning of the list
  if(position == 1)
  {
    *head = positionNode->next;         // Update the head to next node
    dbg_trace("deleted '%s:%s' at position %d in '%s' list.\n",
      positionNode->event.title, positionNode->event.time, position, positionNode->event.date);
    free(positionNode);
    positionNode->next = NULL;          // Remove the first node
  }
  else
  {
    // Traverse until we find the desired position of the node or reach the end,
    // delete the node at the position of whichever earliest
    while((positionNode != NULL) && (count < position))
    {
      count++;
      previousNode = positionNode;
      positionNode = positionNode->next;
    }
    if(positionNode == NULL)
    {
      dbg_info("Reached the end of the list. No node to delete! Returning.\n");
      return FAIL;
    }
    previousNode->next = positionNode->next;
    dbg_trace("deleted '%s:%s' at position %d in '%s' list.\n",
      positionNode->event.title, positionNode->event.time, position, positionNode->event.date);
    free(positionNode);
    positionNode->next = NULL;
  }
  return SUCCESS;
}



/* get the event data from the position (1 to listLength(list), inclusive) */
int getNode(node_t *head, int position, event_t *outEvent)
{
  node_t *current = head;

  if((position < 1) || (position > listLength(head))){
    dbg_trace("Invalid position: %d, must be between 1 to %d (listLength)\n",
                        position, listLength(head));
    return FAIL;
  }
  if(current == NULL){
    dbg_info("Empty List!\n");
    return FAIL;
  }

  while(--position){
    current = current->next;
  }
  // Copy the event data into requested struct
  if(current != NULL){
    memset(outEvent, 0 , sizeof(event_t));
    memcpy(outEvent, &current->event, sizeof(event_t));
  }
  return SUCCESS;
}


/* UPDATES only "time" and "location" fields of the node at position (range: 1 to listLength(list))  */
int setNode(node_t *head, int position, event_t *srcEvent)
{
  node_t *current = head;

  if((position < 1) || (position > listLength(head))){
    dbg_trace("Invalid position: %d, must be between 1 to %d (listLength)\n",
                        position, listLength(head));
    return FAIL;
  }
  if(current == NULL){
    dbg_info("Empty List!\n");
    return FAIL;
  }

  while(--position){
    current = current->next;
  }
  // Copy the event data into requested struct
  return updateEvent(&current->event, srcEvent);
}


/* Deletes the entire linked list */
void deleteList(node_t **head)
{
  node_t *auxNode, *current;

  // make the current one point to the head
  current = *head;
  while(current != NULL)
  {
    auxNode = current->next;
    free(current);
    current = auxNode;
  }
  // Make the change visible to the caller
  *head = NULL;
}
