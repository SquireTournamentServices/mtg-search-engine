#include "./uuid.h"

uuid_t from_string(char *str, int *status)
{
    uuid_t ret;
    int dash_cnt = 0;
    *status = 1;

    for (int i = 0, j = 0; i < 34; i++) {
        if (str[i] > '0' && str[i] < '9') {
            ret.bytes[j++] = str[i] - '0';
        } else if (str[i] > 'a' && str[i] < 'f') {
            ret.bytes[j++] = str[i] - 'a';
        } else if (str[i] > 'A' && str[i] < 'F') {
            ret.bytes[j++] = str[i] - 'A';
        } else if (str[i] == '-') {
            dash_cnt++;
            if (dash_cnt > 3) {
                *status = 0;
                break;
            }
        } else {
            *status = 0;
            break;
        }
    }

    return ret;
}

// Returns 0 on error
int write_uuid(FILE *f, uuid_t uuid)
{
    size_t w = fwrite(uuid.bytes, sizeof(uuid.bytes), 1, f);
    return w == sizeof(uuid.bytes);
}

// Returns 0 on error
int read_uuid(FILE *f, uuid_t *uuid)
{
    size_t r = fread(uuid->bytes, sizeof(uuid->bytes), 1, f);
    return r == sizeof(uuid->bytes);
}

