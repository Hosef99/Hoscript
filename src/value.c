#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "value.h"
#include "mat_raw.h"

int is_iterator(Value value)
{
    switch (value.type) {
        case VAL_MAT:
        case VAL_LIST:
        case VAL_STR:
            return 1;
        default:
            return 0;
    }
}

void append_to_buffer(char **buffer, int *pos, int *size, const char *text) {
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
    char* temp;
    switch (value.type) {
        case VAL_NUM:
            printf("%.15g%s", value.number, end);
            break;
        case VAL_MAT:
            print_mat(value.mat.data, value.mat.rows, value.mat.cols, err);
            break;
        case VAL_BOOL:
            printf(value.boolean ? "true%s" : "false%s", end);
            break;
        case VAL_STR:
            printf("%s%s", value.string, end);
            break;
        case VAL_LIST:
            temp = string_value(value, err);
            printf("%s%s", temp, end);
            free(temp);
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
    else if (value.type == VAL_LIST) {
        for (int i = 0; i < value.list.size; i++) {
            free_value(value.list.elements[i]);
        }
        free(value.list.elements);
    }
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
        case VAL_LIST:
            new.list.size = value.list.size;
            new.list.elements = malloc(new.list.size * sizeof(Value));
            for (int i = 0; i < value.list.size; i++) {
                new.list.elements[i] = copy_value(value.list.elements[i], err);
            }
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
        fprintf(stderr, "Memory Error: allocation failed\n");
        *err = 1;
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
        case VAL_STR:
            append_to_buffer(&buffer, &pos, &size, "\"");
            append_to_buffer(&buffer, &pos, &size, value.string);
            append_to_buffer(&buffer, &pos, &size, "\"");
            return buffer;
        case VAL_LIST:
            append_to_buffer(&buffer, &pos, &size, "[");
            for (int i = 0; i < value.list.size; i++) {
                char* elem_str = string_value(value.list.elements[i], err);
                if (*err) {
                    free(buffer);
                    return NULL;
                }
                append_to_buffer(&buffer, &pos, &size, elem_str);
                free(elem_str);
                if (i != value.list.size - 1)
                    append_to_buffer(&buffer, &pos, &size, ",");
            }
            append_to_buffer(&buffer, &pos, &size, "]");
            return buffer;
        default:
            return NULL;
    }
}

int bool_value(Value value)
{
    switch (value.type) {
        case VAL_BOOL:
            return value.boolean;
            break;
        case VAL_NUM:
            return (value.number == 0) ? 0 : 1;
        case VAL_LIST:
            return value.list.size;
        case VAL_MAT:
            for (int i = 0; i < value.mat.rows; i++) {
                for (int j = 0; j < value.mat.cols; j++) {
                    if (value.mat.data[i][j] != 0) return 0;
                }
            }
            return 1;
        case VAL_STR:
            return strcmp(value.string, "") ? 1 : 0;
        default:
            return 0;
    }
}

Value index_value(Value value, int index, int *err)
{
    *err = 0;

    switch (value.type)
    {
    case VAL_LIST:
        if (index < 0 || index >= value.list.size)
        {
            *err = 1;
            return EMPTY;
        }
        return copy_value(value.list.elements[index], err);

    case VAL_STR:
        {
            int len = strlen(value.string);
            if (index < 0 || index >= len)
            {
                *err = 1;
                return EMPTY;
            }

            char *ch = malloc(2);
            ch[0] = value.string[index];
            ch[1] = '\0';

            Value v;
            v.type = VAL_STR;
            v.string = ch;
            return v;
        }
    default:
        *err = 1;
        return EMPTY;
    }
}

Value slice_value(Value value, int start, int end, int *err)
{
    *err = 0;

    if (start < 0) start = 0;

    switch (value.type)
    {
    case VAL_LIST:
        if (end > value.list.size) end = value.list.size;
        if (start > end)
        {
            *err = 1;
            return EMPTY;
        }

        int new_size = end - start;
        Value v;
        v.type = VAL_LIST;
        v.list.size = new_size;
        v.list.elements = malloc(sizeof(Value) * new_size);

        for (int i = 0; i < new_size; i++)
            v.list.elements[i] = copy_value(value.list.elements[start + i], err);

        return v;

    case VAL_STR:
        {
            int len = strlen(value.string);
            if (end > len) end = len;
            if (start > end)
            {
                *err = 1;
                return EMPTY;
            }

            int new_len = end - start;
            char *substr = malloc(new_len + 1);
            strncpy(substr, value.string + start, new_len);
            substr[new_len] = '\0';

            Value v;
            v.type = VAL_STR;
            v.string = substr;
            return v;
        }
    default:
        *err = 1;
        return EMPTY;
    }
}
