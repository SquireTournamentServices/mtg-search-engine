#include "./uuid.h"
#include "../testing_h/testing.h"
#include <string.h>
#include <ctype.h>

mse_uuid_t mse_from_string(const char *str, int *status)
{
    mse_uuid_t ret;
    memset(&ret, 0, sizeof(ret));
    if (str == NULL) {
        *status = 0;
        lprintf(LOG_ERROR, "Cannot parse NULL string\n");
        return ret;
    }

    size_t len = strlen(str);
    int dash_cnt = 0;
    *status = 1;

    // The values must be zeroed to make sure that the assignment using |= does not cause
    // residue bits to creep in.
    memset(&ret, 0, sizeof(ret));

    size_t str_ptr = 0;
    size_t bytes_read = 0;
    for (; bytes_read < 2 * sizeof(ret.bytes) && str_ptr < len; str_ptr++) {
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
            ret.bytes[bytes_read / 2] |= val << (bytes_read % 2 ? 0 : 4);
            bytes_read++;
        }
    }

    if (bytes_read != 2 * sizeof(ret.bytes) && str_ptr == len - 1) {
        *status = 0;
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
