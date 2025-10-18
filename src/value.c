#include <stdio.h>
#include "value.h"
#include "mat_raw.h"

void print_value(Value value, const char* end, int* err)
{
    switch (value.type) {
        case VAL_NUM:
            printf("%.15g%s", value.number, end);
            break;
        case VAL_MAT:
            print_mat(value.mat.data, value.mat.rows, value.mat.cols, err);
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
        default:
            fprintf(stderr, "Value Error: value is empty\n");
            *err = 1;
            return EMPTY;
    }
}
