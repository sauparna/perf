#ifndef TESTS_H
#define TESTS_H

#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include <sys/types.h>

#include "./bitarray.h"

/***************************************************************************/
/* Prototypes                                                              */
/***************************************************************************/

/* Will run increasingly larger test cases, until a test case takes longer time_limit_seconds to complete.
*/
int timed_rotation(const double time_limit_seconds);

/* Runs testsuite specified in a given file.
 */
void parse_and_run_tests(const char* filename, int selected_test);

#endif // TESTS_H
