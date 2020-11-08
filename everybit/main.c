/* This test harness which allows both functional testing (i.e., testing for correctness) and performance testing (i.e., testing for speed).
*/

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include "./tests.h"

/***************************************************************************/
/* Prototypes                                                              */
/***************************************************************************/

void print_usage(const char* const argv_0);

/***************************************************************************/
/* Functions                                                               */
/***************************************************************************/

int main(int argc, char** argv)
{
    int retval = EXIT_SUCCESS;
    char optchar; /* Parse options. */
    opterr = 0;
    int selected_test = -1;

    while ((optchar = getopt(argc, argv, "n:t:sml")) != -1)
    {
        switch (optchar)
        {
        case 'n':
            selected_test = atoi(optarg);
            break;
        case 't':
            /* -t <file> runs functional tests in the provided file. */
            parse_and_run_tests(optarg, selected_test);
            retval = EXIT_SUCCESS;
            goto cleanup;
        case 's':
            /* -s runs the short rotation performance test. */
            timed_rotation(0.01);
            retval = EXIT_SUCCESS;
            goto cleanup;
        case 'm':
            /* -m runs the medium rotation performance test. */
            timed_rotation(0.1);
            retval = EXIT_SUCCESS;
            goto cleanup;
        case 'l':
            /* -l runs the large rotation performance test. */
            timed_rotation(1.0);
            retval = EXIT_SUCCESS;
            goto cleanup;
        }
    }

    /* If we made it here, we couldn't find any arguments to tell us what to do so we tell the user how to run the program. */
    print_usage(argv[0]);
    retval = EXIT_SUCCESS;

cleanup:
    return retval;
}

void print_usage(const char* const argv_0)
{
    fprintf(stderr, "usage: %s\n"
            "\t -s Run a sample small (0.01s) rotation operation\n"
            "\t -m Run a sample medium (0.1s) rotation operation\n"
            "\t -s Run a sample large (1s) rotation operation\n"
            "\t    (note: the provided -[s/m/l] options only test performance and NOT correctness.)\n"
            "\t -t tests/default\tRun all tests in the testfile tests/default\n"
            "\t -n 1 -t tests/default\tRun test 1 in the testfile tests/default\n",
            argv_0);
}
