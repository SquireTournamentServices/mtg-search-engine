#include "./test_io_utils.h"
#include "../testing_h/testing.h"
#include "../src/io_utils.h"
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>

// Hack for the local scope
typedef struct tm tm;

#define TEST_BASIC_READ_WRITE(type, value) \
static int test_##type##_read_write() \
{ \
    type val_1 = value; \
    int fid[2]; \
    ASSERT(pipe(fid) == 0); \
    FILE *r = fdopen(fid[0], "rb"); \
    ASSERT(r != NULL); \
    FILE *w = fdopen(fid[1], "wb"); \
    ASSERT(w != NULL); \
    ASSERT(write_##type(w, val_1)); \
    fclose(w); \
    type val_2; \
    ASSERT(read_##type(r, &val_2)); \
    ASSERT(memcmp(&val_1, &val_2, sizeof(val_1)) == 0); \
    fclose(r); \
    return 1; \
}

TEST_BASIC_READ_WRITE(double, M_PI)
TEST_BASIC_READ_WRITE(int, INT_MAX)
TEST_BASIC_READ_WRITE(size_t, LLONG_MAX)
TEST_BASIC_READ_WRITE(tm, *gmtime(NULL))

SUB_TEST(test_io_utils, {&test_double_read_write, "Test double read and, write"},
{&test_int_read_write, "Test int read and, write"},
{&test_size_t_read_write, "Test size_t read and, write"},
{&test_tm_read_write, "Test struct tm read and, write"})
