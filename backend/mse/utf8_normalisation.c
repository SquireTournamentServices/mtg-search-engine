#include "./utf8_normalisation.h"
#include <utf8proc.h>
#include <string.h>

char *mse_normalise_utf8(const char *utf8)
{
    utf8proc_uint8_t *fold_str = NULL;
    utf8proc_map((unsigned char *) utf8, 0, &fold_str, UTF8PROC_NULLTERM | UTF8PROC_STRIPMARK | UTF8PROC_DECOMPOSE);
    return (char *) fold_str;
}
