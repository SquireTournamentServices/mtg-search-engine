#include "./test_uuid.h"
#include "../testing_h/testing.h"
#include "../src/uuid.h"
#include <unistd.h>
#include <string.h>

static int test_uuid_read_write()
{
    uuid_t uuid;
    for (size_t i = 0; i < sizeof(uuid); i++) {
        uuid.bytes[i] = (char) i;
    }

    int fid[2];
    ASSERT(pipe(fid) == 0);

    FILE *r = fdopen(fid[0], "rb");
    ASSERT(r != NULL);

    FILE *w = fdopen(fid[1], "wb");
    ASSERT(w != NULL);

    ASSERT(write_uuid(w, uuid));
    fclose(w);

    uuid_t uuid_2;
    ASSERT(read_uuid(r, &uuid_2));
    ASSERT(memcmp(&uuid, &uuid_2, sizeof(uuid)) == 0);
    fclose(r);

    return 1;
}

#define UUID_1_STR "7cfbdb70-96dd-49ac-8f7a-4658996b2c2b"
#define UUID_2_STR "7CFBDB70-96DD-49AC-8F7A-4658996B2C2B"
static int test_uuid_from_string()
{
    int status = 0;
    uuid_t uuid_1 = from_string(UUID_1_STR, &status);
    ASSERT(status == 1);

    uuid_t uuid_2 = from_string(UUID_2_STR, &status);
    ASSERT(status == 1);

    // Test that they are read the same
    ASSERT(memcmp(&uuid_1, &uuid_2, sizeof(uuid_1)) == 0);

    // Test that they are read as the input is
    uuid_t uuid_val = {
        {
            0x7c, 0xfb, 0xdb, 0x70, 0x96, 0xdd, 0x49, 0xac, 0x8f, 0x7a, 0x46, 0x58, 0x99, 0x6b, 0x2c, 0x2b
        }
    };
    lprintf(LOG_INFO, "%p %p\n", *(long *)&uuid_1.bytes[0], *(long *)&uuid_1.bytes[8]);
    lprintf(LOG_INFO, "%p %p\n", *(long *)&uuid_val.bytes[0], *(long *)&uuid_val.bytes[8]);
    ASSERT(memcmp(&uuid_1, &uuid_val, sizeof(uuid_1)) == 0);

    return 1;
}

static int test_set_code_read_write()
{
    mtg_set_code_t set = {'M', '2', '0'};

    int fid[2];
    ASSERT(pipe(fid) == 0);

    FILE *r = fdopen(fid[0], "rb");
    ASSERT(r != NULL);

    FILE *w = fdopen(fid[1], "wb");
    ASSERT(w != NULL);

    ASSERT(write_set_code(w, set));
    fclose(w);

    mtg_set_code_t set_2;
    ASSERT(read_set_code(r, &set_2));
    fclose(r);

    ASSERT(memcmp(set, set_2, sizeof(mtg_set_code_t)) == 0);

    return 1;
}

SUB_TEST(test_uuid, {&test_uuid_read_write, "UUID read and, write"},
{&test_uuid_from_string, "UUID from string"},
{&test_set_code_read_write, "Set code read and, write"})
