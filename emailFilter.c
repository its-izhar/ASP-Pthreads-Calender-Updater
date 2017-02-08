/**
* @Author: Izhar Shaikh
* @Date:   2017-02-08T03:15:44-05:00
* @Email:  izharits@gmail.com
* @Filename: emailFilter.c
* @Last modified by:   Izhar Shaikh
* @Last modified time: 2017-02-08T04:39:13-05:00
*/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "calenderFilter.h"
#include "debugMacros.h"

// Check for valid mode
int isValidMode(char mode)
{
  bool valid = false;
  dbg_trace("Mode Char: %c\n", mode);
  switch(mode)
  {
    case EVENT_CREATE:
    case EVENT_DELETE:
    case EVENT_MODIFY:
      valid = true;
      break;

    default:
      valid = false;
  }
  return valid;
}

// Returns true if the given date string in the main string is MM/DD/YYYY string
int isValidDate(char *date)     // mm/dd/yyyy
{
  int count=0;
  dbg_trace("Date String: %s\n", date);

  for(count = 0; count < DATE_STRING_SIZE; count++) {
    //dbg_trace("Char: %c, Pos: %d\n", date[count], count);
    if(count == 2 || count == 5) {
      if(date[count] != '/') {
        dbg_trace("Expected Char: %c, found: %c, Pos: %d\n", '/', date[count], count);
        return 0;
      }
    }
    else {
      if(!isdigit(date[count])) {
        dbg_trace("Expected Char: 0-9, found: %c, Pos: %d\n", date[count], count);
        return 0;
      }
    }
  }
  return 1;
}

// Returns true if the given time string in the main string is in HH:MM format
int isValidTime(char *time)
{
  int count=0;
  dbg_trace("Time String: %s\n", time);

  for(count = 0; count < TIME_STRING_SIZE; count++) {
    //dbg_trace("Char: %c, Pos: %d\n", time[count], count);
    if(count == 2) {
      if(time[count] != ':') {
        dbg_trace("Expected Char: %c, found: %c, Pos: %d\n", ':', time[count], count);
        return 0;
      }
    }
    else {
      if(!isdigit(time[count])) {
        dbg_trace("Expected Char: 0-9, found: %c, Pos: %d\n", time[count], count);
        return 0;
      }
    }
  }
  return 1;
}


int isComma(char alpha)
{
  return alpha == ',';
}

/* Parses the email subject into different fields */
int isValidEmailFormat(char *emailSubject, size_t size)
{
  bool isValid = false;
  // Skip the SUBJECT_WORD_SKIP_SIZE and point to the line start
  char *lineBuffer = emailSubject + SUBJECT_WORD_SKIP_SIZE;
  dbg_trace("String: %s\n", lineBuffer);

  int position = -1, pass = -1, commaCount = 0;
  for(char *ptr = lineBuffer; *ptr != '\0'; ptr++)
  {
    ++position;
    ++pass;
    //1, 12, 23, 29 <<--comma posiitons
    switch(pass)
    {
      case MODE_STRING_SIZE:
      case MODE_STRING_SIZE+TITLE_STRING_SIZE+1:
      case MODE_STRING_SIZE+TITLE_STRING_SIZE*2+2:
      case MODE_STRING_SIZE+TITLE_STRING_SIZE*2+TIME_STRING_SIZE+3:
        if(isComma(*ptr)){
          ++commaCount;
          //dbg_trace("Comma found at %d position!\n", position);
        }
        break;
    }
  }
  dbg_trace("position: %d\n", position);

  // Decide the validity of the string based on the format, mode check,
  // date check and time check as well
  if(commaCount == 4 && position <= MAX_STRING_SUBJECT_SIZE) {
    if(isValidMode(lineBuffer[0])) {
      if(isValidDate(lineBuffer + MODE_STRING_SIZE + TITLE_STRING_SIZE + 2)) {
        if(isValidTime(lineBuffer+ MODE_STRING_SIZE + TITLE_STRING_SIZE*2 + 3)) {
          isValid = true;
        }
      }
    }
  }
  return isValid;
}
