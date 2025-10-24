#ifndef VALUE_H
#define VALUE_H 

typedef enum
{
    VAL_NUM,
    VAL_MAT,
    VAL_EMP,
    VAL_STR
} ValueType;

typedef struct
{
    ValueType type;
    union
    {
        double number;
        char* string;
        struct
        {
            int rows;
            int cols;
            double** data;
        }mat;
    };
} Value;

int is_iterator(Value value);
void print_value(Value value, const char* end, int* err);
void free_value(Value value);
Value copy_value(Value value, int* err);

extern const Value EMPTY;
#endif
