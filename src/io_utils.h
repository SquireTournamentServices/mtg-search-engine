#pragma once
/// Writes binary in my arbitary format. Strings are pascal strings when
/// written, then c strings when read. Simple as.

/// All return 1 on success, 0 on fail.
#include <time.h>
#include <stdio.h>

int write_double(FILE *f, double d);
int read_double(FILE *f, double *d);

int write_int(FILE *f, int i);
int read_int(FILE *f, int *i);

int write_size_t(FILE *f, size_t i);
int read_size_t(FILE *f, size_t *i);

int write_str(FILE *f, char *str);
int read_str(FILE *f, char **str);

int write_tm(FILE *f, struct tm t);
int read_tm(FILE *f, struct tm *t);

/// strtod wrapper
int mse_to_double(char *input, double *ret);
