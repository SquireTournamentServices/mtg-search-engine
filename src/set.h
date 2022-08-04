#include <time.h>
#include "./uuid.h"

typedef struct mtg_set_t {
    uuid_t id;
    char *name;
    char code[3]; // i.e: 2xm
    struct tm release;
} mtg_set_t;
