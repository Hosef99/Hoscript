#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "value.h"
#include "mat_raw.h"

static void append_to_buffer(char **buffer, int *pos, int *size, const char *text) {
    int len = strlen(text);
    while (*pos + len + 1 >= *size) {  // +1 for null terminator
        *size *= 2;
        char *newbuf = realloc(*buffer, *size);
        if (!newbuf) {
            free(*buffer);
            fprintf(stderr, "Memory reallocation failed.\n");
            exit(EXIT_FAILURE);
        }
        *buffer = newbuf;
    }
    memcpy(*buffer + *pos, text, len);
    *pos += len;
    (*buffer)[*pos] = '\0';
}

void print_value(Value value, const char* end, int* err)
{
    switch (value.type) {
        case VAL_NUM:
            printf("%.15g%s", value.number, end);
            break;
        case VAL_MAT:
            print_mat(value.mat.data, value.mat.rows, value.mat.cols, err);
            break;
        case VAL_STR:
            printf("%s%s", value.string, end);
            break;
        case VAL_EMP:
            break;
        default:
            fprintf(stderr, "Value Error: value unrecognized\n");
            *err = 1;
            break;
    }
}

void free_value(Value value)
{
    if (value.type == VAL_MAT)
        free_mat(value.mat.data, value.mat.rows);
    else if (value.type == VAL_STR)
        free(value.string);
}

Value copy_value(Value value, int* err)
{
    Value new;
    new.type = value.type;

    switch (value.type) {
        case VAL_NUM:
            new.number = value.number;
            return new;
        case VAL_MAT:
            new.mat.data = cpy_mat(value.mat.data, value.mat.rows, value.mat.cols, err);
            new.mat.rows = value.mat.rows;
            new.mat.cols = value.mat.cols;
            return new;
        case VAL_STR:
            new.string = strdup(value.string);
            return new;
        default:
            fprintf(stderr, "Value Error: value is empty\n");
            *err = 1;
            return EMPTY;
    }
}

char* string_value(Value value, int* err)
{
    int size = 64, pos = 0;
    char* buffer = malloc(size);
    if (!buffer) {
        return NULL;
    }
    buffer[0] = '\0';
    char temp[64];

    switch (value.type) {
        case VAL_NUM:
            snprintf(temp, sizeof(temp), "%g", value.number);
            append_to_buffer(&buffer, &pos, &size, temp);
            return buffer;
        case VAL_MAT:
            append_to_buffer(&buffer, &pos, &size, "[");
            for (int i = 0; i < value.mat.rows; i++) {
                for (int j = 0; j < value.mat.cols; j++) {
                    snprintf(temp, sizeof(temp), "%g", value.mat.data[i][j]);
                    append_to_buffer(&buffer, &pos, &size, temp);
                    if (j != value.mat.cols - 1) {
                        append_to_buffer(&buffer, &pos, &size, ",");
                    }
                }
                if (i != value.mat.rows - 1) {
                    append_to_buffer(&buffer, &pos, &size, ";");
                }
            }
            append_to_buffer(&buffer, &pos, &size, "]");
            return buffer;
        default:
            return NULL;
    }
}
