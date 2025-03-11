#include "./uuid.h"
#include "../testing_h/testing.h"
#include <string.h>
#include <ctype.h>

mse_uuid_t mse_from_string(const char *str, int *status)
{
    mse_uuid_t ret;
    // The values must be zeroed to make sure that the assignment using |= does not cause
    // residue bits to creep in.
    memset(&ret, 0, sizeof(ret));

    size_t len = strlen(str);
    int dash_cnt = 0;

    size_t str_ptr = 0;
    size_t nibbles_read = 0;
    for (; nibbles_read < 2 * sizeof(ret.bytes) && str_ptr < len; str_ptr++) {
        unsigned char val = 0;
        int has_val = 0;

        if (str[str_ptr] == '-') {
            dash_cnt++;
            if (dash_cnt > 4) {
                *status = 0;
                break;
            }
        } else  if (str[str_ptr] >= '0' && str[str_ptr] <= '9') {
            val = str[str_ptr] - '0';
            has_val = 1;
        } else if (str[str_ptr] >= 'a' && str[str_ptr] <= 'f') {
            val = 10 + str[str_ptr] - 'a';
            has_val = 1;
        } else if (str[str_ptr] >= 'A' && str[str_ptr] <= 'F') {
            val = 10 + str[str_ptr] - 'A';
            has_val = 1;
        } else {
            *status = 0;
            break;
        }

        if (has_val) {
            ret.bytes[nibbles_read / 2] |= val << (nibbles_read % 2 ? 0 : 4);
            nibbles_read++;
        }
    }

    if (nibbles_read != 2 * sizeof(ret.bytes) || str_ptr != len) {
        *status = 0;
    } else {
        *status = 1;
    }

    return ret;
}

int mse_write_uuid(FILE *f, mse_uuid_t uuid)
{
    ASSERT(fwrite(uuid.bytes, sizeof(uuid.bytes), 1, f));
    return 1;
}

int mse_read_uuid(FILE *f, mse_uuid_t *uuid)
{
    ASSERT(fread(uuid->bytes, sizeof(uuid->bytes), 1, f));
    return 1;
}

int mse_get_set_code(const char *code, mse_set_code_t *ret)
{
    ASSERT(ret != NULL);
    memset(ret, 0, sizeof(*ret));

    ASSERT(code != NULL);

    size_t len = strlen(code);
    ASSERT(len >= MIN_SET_CODE_LEN);
    ASSERT(len <= MAX_SET_CODE_LEN);

    for (size_t i = 0; i < len; i++) {
        (*ret)[i] = tolower(code[i]);
    }
    return 1;
}

int mse_write_set_code(FILE *f, mse_set_code_t code)
{
    ASSERT(fwrite(code, sizeof(mse_set_code_t), 1, f));
    return 1;
}

int mse_read_set_code(FILE *f, mse_set_code_t *code)
{
    ASSERT(fread(code, sizeof(mse_set_code_t), 1, f));
    return 1;
}

mse_uuid_t mse_max_uuid()
{
    mse_uuid_t ret;
    memset(ret.bytes, 0xFF, sizeof(ret.bytes));
    return ret;
}

mse_uuid_t mse_min_uuid()
{
    mse_uuid_t ret;
    memset(ret.bytes, 0, sizeof(ret.bytes));
    return ret;
}

static int __to_hex(unsigned char c)
{
    if (c >= 0xa) {
        return 'a' + (c - 0xa);
    } else {
        return '0' + c;
    }
}

#define NIBBLE_MASK 0xF

//                    000000000011111111112222222222333333
//                    --------8----3----8----3------------
#define EXAMPLE_UUID "7c58134f-8116-4d49-9023-3152d114b590"

char *mse_uuid_as_string(mse_uuid_t uuid)
{
    char *res = (char *) malloc(sizeof(EXAMPLE_UUID));
    if (res == NULL) {
        return res;
    }

    for (size_t i = 0, j = 0; i < sizeof(uuid.bytes); i++) {
        if (j == 8 || j == 13 || j == 18 || j == 23) {
            res[j++] = '-';
        }

        res[j++] = __to_hex((uuid.bytes[i] >> 4) & NIBBLE_MASK);
        res[j++] = __to_hex(uuid.bytes[i] & NIBBLE_MASK);
    }

    res[sizeof(EXAMPLE_UUID) - 1] = 0;
    return res;
}
