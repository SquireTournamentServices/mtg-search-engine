#include "../testing_h/testing.h"
#include "../src/query_parser.h"
#include "../src/interpretor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int __test_next_query(FILE *f)
{
    char buffer[1024];
    char *r = fgets(buffer, sizeof(buffer), f);

    // EOF
    if (r == NULL) {
        return EOF;
    }
    // Empty string is also EOF
    if (buffer[0] == 0) {
        return EOF;
    }

    // Strip the last char (\n)
    ASSERT(strlen(buffer) > 2);
    size_t last = strlen(buffer) - 1;
    if (buffer[last] == '\n') {
        buffer[last] = 0;
    }

    lprintf(TEST_INFO, "Testing parse of '%s'\n", buffer);
    mse_interp_node_t *ret = NULL;
    ASSERT(parse_input_string(buffer, &ret));
    mse_free_interp_node(ret);

    lprintf(TEST_PASS, "Passed\n");
    return 1;
}

static int __main()
{
    FILE *f = fopen("./valid_queries.txt", "r");
    ASSERT(f != NULL);

    int s = 1;
    int flag = 1;
    do {
        flag = __test_next_query(f);
        if (!flag) {
            s = 0;
        }
    } while (flag != EOF);

    fclose(f);
    return s;
}

int main()
{
    if (__main()) {
        lprintf(TEST_PASS, "All queries were successfully parsed\n");
        return 0;
    } else {
        lprintf(TEST_FAIL, "Some queries did not parse\n");
        return 1;
    }
}
