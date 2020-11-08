/* We need _POSIX_C_SOURCES to pick up 'struct timespec' and clock_gettime. */
#define _POSIX_C_SOURCE 200112L

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef __APPLE__
#include <time.h>
#else
#include "CoreServices/CoreServices.h"
#include "mach/mach.h"
#include "mach/mach_time.h"
#endif

#include "./ktiming.h"

/***************************************************************************/
/* Macros                                                                  */
/***************************************************************************/

#ifdef __CYGWIN__
/* Which clock to get the time from. */
#define KTIMING_CLOCK_ID CLOCK_REALTIME
#else
/* Which clock to get the time from. */
#define KTIMING_CLOCK_ID CLOCK_PROCESS_CPUTIME_ID
#endif

/***************************************************************************/
/* Functions                                                               */
/***************************************************************************/

clockmark_t ktiming_getmark() {
#ifdef __APPLE__
  const uint64_t now = mach_absolute_time();
  const Nanoseconds now_nanoseconds =
      AbsoluteToNanoseconds(*(AbsoluteTime *)&now);
  return *(uint64_t *)&now_nanoseconds;
#else
  struct timespec now;
  uint64_t now_nanoseconds;

  int stat = clock_gettime(KTIMING_CLOCK_ID, &now);
  if (stat != 0) {
/* Whoops, we couldn't get hold of the clock. If we're on a platform that
 * supports it, we try again with CLOCK_MONOTONIC */
#ifndef __CYGWIN__
    stat = clock_gettime(CLOCK_MONOTONIC, &now);
#endif
    if (stat != 0) {
      perror("ktiming_getmark()");
      exit(-1);
    }
  }

  now_nanoseconds = now.tv_nsec;
  now_nanoseconds += ((uint64_t)now.tv_sec) * 1000 * 1000 * 1000;
  return now_nanoseconds;
#endif
}

uint64_t ktiming_diff_usec(const clockmark_t *const start,
                           const clockmark_t *const end) {
  return *end - *start;
}

float ktiming_diff_sec(const clockmark_t *const start,
                       const clockmark_t *const end) {
  return (float)ktiming_diff_usec(start, end) / 1000000000.0f;
}
