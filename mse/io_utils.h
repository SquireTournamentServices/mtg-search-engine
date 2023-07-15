#pragma once
/// Writes binary in my arbitary format. Strings are pascal strings when
/// written, then c strings when read. Simple as.

/// All return 1 on success, 0 on fail.
#include <time.h>
#include <stdio.h>

extern int mse_write_double(FILE *f, double d);
extern int mse_read_double(FILE *f, double *d);

extern int mse_write_int(FILE *f, int i);
extern int mse_read_int(FILE *f, int *i);

extern int mse_write_size_t(FILE *f, size_t i);
extern int mse_read_size_t(FILE *f, size_t *i);

extern int mse_write_str(FILE *f, char *str);
extern int mse_read_str(FILE *f, char **str);

extern int mse_write_tm(FILE *f, struct tm t);
extern int mse_read_tm(FILE *f, struct tm *t);

/// strtod wrapper
extern int mse_to_double(char *input, double *ret);

extern char *mse_to_lower(char *input);
