#include "./set.h"
#include "./io_utils.h"
#include "../testing_h/testing.h"
#include <string.h>
#include <time.h>
#include "../strptime/strptime.h"

// Parse dates in the form 2007-07-13, see man strptime.h
#define SET_DATE_FORMAT "%Y-%m-%d"

int parse_set_json(json_t *set_node, mtg_set_t *ret, const char *code)
{
    memset(ret, 0, sizeof(*ret));
    ASSERT(set_node != NULL);
    ASSERT(ret != NULL);

    // Copy set code
    size_t len = strlen(code);
    ASSERT(len >= MIN_SET_CODE_LEN);
    ASSERT(len <= MAX_SET_CODE_LEN);
    memset(ret->code, 0, sizeof(ret->code));
    memcpy(ret->code, code, len);

    // Copy set name
    json_t *name_node = json_object_get(set_node, "name");
    ASSERT(json_is_string(name_node));
    ret->name = strdup(json_string_value(name_node));

    // Copy set release date
    json_t *release_node = json_object_get(set_node, "releaseDate");
    ASSERT(json_is_string(release_node));
    ASSERT(strptime(json_string_value(release_node), SET_DATE_FORMAT, &ret->release) != NULL);

    return 1;
}

int write_set(FILE *f, mtg_set_t set)
{
    ASSERT(write_str(f, set.name));
    ASSERT(write_set_code(f, set.code));
    ASSERT(write_tm(f, set.release));
    return 1;
}

int read_set(FILE *f, mtg_set_t *set)
{
    ASSERT(read_str(f, &set->name));
    ASSERT(read_set_code(f, &set->code));
    ASSERT(read_tm(f, &set->release));
    return 1;
}

void free_set(mtg_set_t *set)
{
    if (set->name != NULL) {
        free(set->name);
        set->name = NULL;
    }
}

int cmp_set(mtg_set_t *a, mtg_set_t *b)
{
    return memcmp(a->code, b->code, sizeof(a->code));
}

int avl_cmp_set(void *a, void *b)
{
    return cmp_set((mtg_set_t *) a, (mtg_set_t *) b);
}
