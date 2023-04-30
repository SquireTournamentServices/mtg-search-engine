#include "../testing_h/testing.h"
#include "./io_utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

int write_double(FILE *f, double d)
{
    int w = fwrite(&d, sizeof(d), 1, f);
    return w == 1;
}

int read_double(FILE *f, double *d)
{
    int r = fread(d, sizeof * d, 1, f);
    return r == 1;
}

int write_int(FILE *f, int i)
{
    int w = fwrite(&i, sizeof(i), 1, f);
    return w == 1;
}

int read_int(FILE *f, int *i)
{
    int r = fread(i, sizeof * i, 1, f);
    return r == 1;
}

int write_size_t(FILE *f, size_t i)
{
    int w = fwrite(&i, sizeof(i), 1, f);
    return w == 1;
}

int read_size_t(FILE *f, size_t *i)
{
    int r = fread(i, sizeof * i, 1, f);
    return r == 1;
}

int write_str(FILE *f, char *str)
{
    size_t len = strlen(str);
    ASSERT(write_size_t(f, len));

    int w = fwrite(str, sizeof * str, len, f);
    return (size_t) w == len;
}

int read_str(FILE *f, char **str)
{
    size_t len;
    ASSERT(read_size_t(f, &len));
    ASSERT(len > 0);

    *str = malloc(sizeof(*str) * (len + 1));
    ASSERT(*str != NULL);
    (*str)[len] = 0;

    int r = fread(*str, sizeof ** str, len, f);
    return (size_t) r == len;
}

int write_tm(FILE *f, struct tm t)
{
    int w = fwrite(&t, sizeof(t), 1, f);
    return w == 1;
}

int read_tm(FILE *f, struct tm *t)
{
    int r = fread(t, sizeof(*t), 1, f);
    return r == 1;
}

int mse_to_double(char *input, double *ret)
{
    char *endptr = NULL;
    *ret = strtod(input, &endptr);
    return endptr == &input[strlen(input)];
}

char *mse_to_lower(char *input)
{
    size_t len = strlen(input);
    char *ret = malloc(len + 1);
    for (size_t i = 0; i < len; i++) {
        ret[i] = tolower(ret[i]);
    }
    ret[len] = 0 ;
    return ret;
}
