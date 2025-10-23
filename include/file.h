#ifndef FILE_H
#define FILE_H

#include <stdlib.h>

char* read_file(const char* filename, int* err);
int write_file(const char *filename, const char *buffer, size_t size);

#endif

