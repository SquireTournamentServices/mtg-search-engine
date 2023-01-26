#include "./set.h"
#include "./io_utils.h"
#include "../testing_h/testing.h"

static int write_set_code(FILE *f, mtg_set_t *set)
{
    int w = fwrite(set->code, sizeof(set->code), 1, f);
    return w == sizeof(set->code);
}

static int read_set_code(FILE *f, mtg_set_t *set)
{
    int r = fread(set->code, sizeof(set->code), 1, f);
    return r == sizeof(set->code);
}

int write_set(FILE *f, mtg_set_t *set)
{
    ASSERT(write_uuid(f, set->id));
    ASSERT(write_str(f, set->name));
    ASSERT(write_set_code(f, set));
    ASSERT(write_tm(f, set->release));
    return 1;
}

int read_set(FILE *f, mtg_set_t *set)
{
    ASSERT(read_uuid(f, &set->id));
    ASSERT(read_str(f, &set->name));
    ASSERT(read_set_code(f, set));
    ASSERT(read_tm(f, &set->release));
    return 1;
}

