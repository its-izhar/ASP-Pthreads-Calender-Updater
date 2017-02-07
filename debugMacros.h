/**
* @Author: Izhar Shaikh
* @Date:   2017-02-07T14:46:31-05:00
* @Last modified by:   Izhar
* @Last modified time: 2017-02-07T15:25:57-05:00
*/

#ifndef __DEBUG_MACROS__
#define __DEBUG_MACROS__

#include <stdio.h>

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

#define SUCCESS 0
#define FAIL    1

#define dbg_trace(fmt, ...) \
        do { if(DEBUG_TEST) \
                     fprintf(stderr, \
                             "DEBUG:%d: %s(): " fmt, \
                             __LINE__, __func__, __VA_ARGS__); \
        } while(0)

#define dbg_info(fmt, ...) \
        do { if(DEBUG_TEST) \
                     fprintf(stderr, \
                             "INFO:%d: %s(): " fmt, \
                             __LINE__, __func__ ); \
        } while(0)

#define print_output(fmt, ...) \
        do { fprintf(stdout, fmt, __VA_ARGS__); \
        } while(0)

#endif
