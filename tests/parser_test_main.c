#include "../testing_h/testing.h"
#include "../src/query_parser.h"
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

    // Strip the last char (\n)
    ASSERT(strlen(buffer) > 2);
    size_t last = strlen(buffer) - 1;
    if (buffer[last] == '\n') {
        buffer[last] = 0;
    }

    lprintf(LOG_INFO, "Testing parse of '%s'\n", buffer);
    ASSERT(parse_input_string(buffer));
    return 1;
}

static int __main()
{
    FILE *f = fopen("./valid_queries.txt", "r");
    ASSERT(f != NULL);

    while(__test_next_query(f) != EOF);

    fclose(f);
    return 1;
}

int main()
{
    return __main() ? 0 : 1;
}
