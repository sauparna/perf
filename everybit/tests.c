#define _GNU_SOURCE
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#include "./bitarray.h"
#include "./ktiming.h"
#include "./tests.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/***************************************************************************/
/* Prototypes                                                              */
/***************************************************************************/

void testutil_frmstr(const char* const bitstring);
void testutil_rotate(const size_t bit_offset,
                     const size_t bit_length,
                     const ssize_t bit_right_shift_amount);
void testutil_require_valid_input(const size_t bit_offset,
                                  const size_t bit_length,
                                  const ssize_t bit_right_shift_amount,
                                  const char* const func_name,
                                  const int line);
static void testutil_newrand(const size_t bit_sz, const unsigned int seed);
static void bitarray_fprint(FILE* const stream, const bitarray_t* const bitarray);
static void testutil_expect_internal(const char* const bitstring,
                                     const char* const func_name,
                                     const int line);
static bool boolfromchar(const char c);
char* next_arg_char();

/***************************************************************************/
/* Globals                                                                */
/***************************************************************************/

/* Some global variable make it easier to run individual tests. */

static bitarray_t* test_bitarray = NULL; /* The bit array currently under test. */
static bool test_verbose = false; /* Whether or not test should be verbose. */

/***************************************************************************/
/* Macros                                                                  */
/***************************************************************************/

/* Marks a test as successful, outputting its name and line. */
#define TEST_PASS() TEST_PASS_WITH_NAME(__func__, __LINE__)

/* Marks a test as successful, outputting the specified name and line. */
#define TEST_PASS_WITH_NAME(name, line)                             \
    fprintf(stderr, " --> %s at line %d: PASS\n", (name), (line))

/* Marks a test as unsuccessful, outputting its name, line and the specified failure message.

   Use this macro just like you would call printf.
*/
#define TEST_FAIL(failure_msg, args...) \
    TEST_FAIL_WITH_NAME(__func__, __LINE__, failure_msg, ##args)

/* Marks a test as unsuccessful, outputting the specified name, line and failure message. 

   Use this macro just like you would call printf.
*/
#define TEST_FAIL_WITH_NAME(name, line, failure_msg, args...)       \
    do {                                                            \
        fprintf(stderr, " --> %s at line %d: FAIL\n    Reason:",    \
                (name), (line));                                    \
        fprintf(stderr, (failure_msg), ##args);                     \
        fprintf(stderr, "\n");                                      \
    } while (0)
    
/* Calls testutil_expect_internal with the current function and line number.

   Requires that test_bitarray is not NULL.
*/
#define testutil_expect(bitstring)              \
    testutil_expect_internal((bitstring), __func__, __LINE__)

/* Retrieves an integer from the strtok buffer.
*/
#define NEXT_ARG_LONG() atol(strtok(NULL, " "))

/***************************************************************************/
/* Functions                                                               */
/***************************************************************************/

static void testutil_newrand(const size_t bit_sz, const unsigned int seed)
{
    /* Free it if it had not been freed after a previous test. */
    if (test_bitarray != NULL)
    {
        bitarray_free(test_bitarray);
    }
    test_bitarray = bitarray_new(bit_sz);
    assert(test_bitarray != NULL);

    /* Reseed the RNG with whatever we are passed; this ensures that we can repeat the test deterministically by specifying the same seed. */
    srand(seed);

    bitarray_randfill(test_bitarray);

    /* If we are asked to be verbose print the random seed and bit array. */
    if (test_verbose)
    {
        bitarray_fprint(stdout, test_bitarray);
        fprintf(stdout, " newrand sz=%zu, seed=%u\n", bit_sz, seed);
    }
}

void testutil_frmstr(const char* const bitstring)
{
    const size_t bitstring_length = strlen(bitstring);

    /* Free it if it had not been freed after a previous test. */
    if (test_bitarray != NULL)
    {
        bitarray_free(test_bitarray);
    }

    test_bitarray = bitarray_new(bitstring_length);
    assert(test_bitarray != NULL);

    bool current_bit;
    for (size_t i = 0; i < bitstring_length; ++i)
    {
        current_bit = boolfromchar(bitstring[i]);
        bitarray_set(test_bitarray, i, current_bit);
    }
    bitarray_fprint(stdout, test_bitarray);
    
    if (test_verbose)
    {
        /* Check if the constructed bit array matches the input string. */
        fprintf(stdout, " newstr lit=%s\n", bitstring);
        testutil_expect(bitstring);
    }
}

static void bitarray_fprint(FILE* const stream, const bitarray_t* const bitarray)
{
    for (size_t i = 0; i < bitarray_get_bit_sz(bitarray); ++i)
    {
        fprintf(stream, "%d", bitarray_get(bitarray, i) ? 1 : 0);
    }
}

static void testutil_expect_internal(const char* bitstring, const char* const func_name, const int line)
{
    const char* bad = NULL; /* The reason why the test fails. If the test passes, this will stay NULL. */
    
    assert(test_bitarray != NULL);

    /* Check the length of the bit array under test. */
    const size_t bitstring_length = strlen(bitstring);
    if (bitstring_length != bitarray_get_bit_sz(test_bitarray))
    {
        bad = "bitarray size";
    }

    /* Check the content. */
    for (size_t i = 0; i < bitstring_length; ++i)
    {
        if (bitarray_get(test_bitarray, i) != boolfromchar(bitstring[i]))
        {
            bad = "bitarray content";
            break;
        }
    }

    /* Obtain a string for the actual bit array. */
    const size_t actual_bitstring_length = bitarray_get_bit_sz(test_bitarray);
    char* actual_bitstring = calloc(sizeof(char), bitstring_length + 1);
    for (size_t i = 0; i < actual_bitstring_length; ++i)
    {
        if (bitarray_get(test_bitarray, i))
        {
            actual_bitstring[i] = '1';
        }
        else
        {
            actual_bitstring[i] = '0';
        }
    }

    if (bad != NULL)
    {
        bitarray_fprint(stdout, test_bitarray);
        fprintf(stdout, " expect bits=%s \n", bitstring);
        TEST_FAIL_WITH_NAME(func_name, line, " Incorrect %s.\n    Expected: %s\n     Actual: %s", bad, bitstring, actual_bitstring);
    }
    else
    {
        TEST_PASS_WITH_NAME(func_name, line);
    }
    free(actual_bitstring);
}

void testutil_rotate(const size_t bit_offset,
                     const size_t bit_length,
                     const ssize_t bit_right_shift_amount)
{
    assert(test_bitarray != NULL);
    bitarray_rotate(test_bitarray, bit_offset, bit_length, bit_right_shift_amount);
    if (test_verbose)
    {
        bitarray_fprint(stdout, test_bitarray);
        fprintf(stdout, " rotate off=%zu, len=%zu, amnt=%zd\n",
                bit_offset, bit_length, bit_right_shift_amount);
    }
}

void testutil_require_valid_input(const size_t bit_offset,
                                  const size_t bit_length,
                                  const ssize_t bit_right_shift_amount,
                                  const char* const func_name,
                                  const int line)
{
    size_t bitarray_length = bitarray_get_bit_sz(test_bitarray);
    if (bit_offset >= bitarray_length || bit_length > bitarray_length ||
        bit_offset + bit_length > bitarray_length)
    {
        // Invalid input.
        TEST_FAIL_WITH_NAME(func_name, line, " TEST SUITE ERROR - " \
                            "bit_offset + bitlength > bitarray_length");
    }
}

// Precomputed array of fibonacci numbers
const int FIB_SIZE = 53;
const double fibs[FIB_SIZE] = {1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946, 17711, 28657, 46368, 75025, 121393, 196418, 317811, 514229, 832040, 1346269, 2178309, 3524578, 5702887, 9227465, 14930352, 24157817, 39088169, 63245986, 102334155, 165580141, 267914296, 433494437, 701408733, 1134903170, 1836311903, 2971215073, 4807526976, 7778742049, 12586269025, 20365011074, 32951280099, 53316291173, 86267571272};

int timed_rotation(const double time_limit_seconds)
{
    /* In doing a large number of rotations, output could become extremely verbose, so shut it down right away. */
    test_verbose = false;
    int tier_num = 0;

    /* Output format strings */
    char header[50];
    sprintf(header, "%-4s %-15s %-15s %-10s\n", "TIER", "SIZE(B)", "#SHIFTS", "TIME(s)");
    printf("%s", header);

    /* Continue until the rotation exceeds time_limit_seconds. */
    while (tier_num + 3 < FIB_SIZE)
    {
        const size_t bit_offset             = fibs[tier_num];
        const size_t bit_right_shift_amount = fibs[tier_num + 1];
        const size_t bit_length             = fibs[tier_num + 2];
        const size_t bit_sz                 = fibs[tier_num + 3];
        assert(bit_sz > bit_length);
        assert(bit_length > bit_right_shift_amount);
        assert(bit_right_shift_amount > bit_offset);
        assert(bit_sz > bit_offset + bit_length);

        /* Initialize a new bit array. */
        testutil_newrand(bit_sz, 6172);

        /* Time the duration of a rotation. */
        const clockmark_t start_time = ktiming_getmark();
        testutil_rotate(bit_offset, bit_length, bit_right_shift_amount);
        const clockmark_t end_time = ktiming_getmark();
        double diff_seconds = ktiming_diff_usec(&start_time, &end_time) / 1000000000.0; 

        if (diff_seconds < time_limit_seconds)
        {
            printf("%-4d %-15lu %-15lu %-.6f\n", tier_num, bit_length / 8, bit_right_shift_amount, diff_seconds);
            tier_num++;
        }
        else
        {
            printf("%-4d %-15lu %-15lu %-.6f exceeded %.2fs cutoff\n", tier_num, bit_length / 8, bit_right_shift_amount, diff_seconds, time_limit_seconds);
            /* Return the last tier that was successful. */
            return tier_num - 1;
        }
    }
    /* Return the last tier that was successful. */
    return tier_num - 1;
}

static bool boolfromchar(const char c)
{
    assert(c == '0' || c == '1');
    return c == '1';
}

char* next_arg_char()
{
    char* buf = strtok(NULL, " ");
    char* eol = NULL;
    if ((eol = strchr(buf, '\n')) != NULL)
    {
        *eol = '\0';
    }
    return buf;
}

void parse_and_run_tests(const char* filename, int selected_test)
{
    test_verbose = false;
    fprintf(stderr, "Testing file %s.\n", filename);
    FILE* f = fopen(filename, "r");
    char* buf = NULL;
    size_t bufsize = 0;
    int test = -1;
    int line = 0;
    bool ready_to_run = false;
    if (f == NULL)
    {
        fprintf(stderr, "Error opening file.\n");
        return;
    }
    while (getline(&buf, &bufsize, f) != -1)
    {
        line++;
        char* token = strtok(buf, " ");
        switch (token[0])
        {
        case '\n':
        case '#':
            continue;
        case 't':
            test = (int) NEXT_ARG_LONG();
            ready_to_run = (test == selected_test || selected_test == -1);
            if (!ready_to_run)
            {
                continue;
            }
            fprintf(stderr, "\nRunning test #%d...\n", test);
            break;
        case 'n':
            if (!ready_to_run)
            {
                continue;
            }
            testutil_frmstr(next_arg_char());
            break;
        case 'e':
            if (!ready_to_run)
            {
                continue;
            }
            {
                char* expected = next_arg_char();
                testutil_expect_internal(expected, filename, line);
            }
            break;
        case 'r':
            if (!ready_to_run)
            {
                continue;
            }
            {
                size_t offset  = (size_t)  NEXT_ARG_LONG();
                size_t length  = (size_t)  NEXT_ARG_LONG();
                ssize_t amount = (ssize_t) NEXT_ARG_LONG();
                testutil_require_valid_input(offset, length, amount, filename, line);
                testutil_rotate(offset, length, amount);
            }
            break;
        default:
            fprintf(stderr, "Done testing file %s.\n", buf);
        }
    }
    free(buf);
    fprintf(stderr, "Done testing file %s.\n", filename);
}
