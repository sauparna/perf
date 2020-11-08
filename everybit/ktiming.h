#ifndef _KTIMING_H_
#define _KTIMING_H_

#include <stdint.h>

/***************************************************************************/
/* Types                                                                   */
/***************************************************************************/

typedef uint64_t clockmark_t; /* A clock time. */

/***************************************************************************/
/* Prototypes                                                              */
/***************************************************************************/

/* Returns the difference between two clockmark_t in nanoseconds. In particular,
   returns the *end - *start.

   Warning: Although the function is called ktiming_diff_usec, it returns a
   value in nanoseconds, not microseconds!
 */
uint64_t ktiming_diff_usec(const clockmark_t *const start,
                           const clockmark_t *const end);

/* Returns the difference between two clockmark_t in seconds. In particular,
 * returns the *end - *start.
 */
float ktimitng_diff_sec(const clockmark_t *const start,
                        const clockmark_t *const end);

/* Gets the current clock time.
 */
clockmark_t ktiming_getmark();

#endif // _KTIMING_H_
