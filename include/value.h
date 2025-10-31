#ifndef VALUE_H
#define VALUE_H 

typedef enum
{
    VAL_NUM,
    VAL_MAT,
    VAL_BOOL,
    VAL_LIST,
    VAL_EMP,
    VAL_STR
} ValueType;

typedef struct Value
{
    ValueType type;
    union
    {
        double number;
        char* string;
        int boolean;
        struct
        {
            int rows;
            int cols;
            double** data;
        }mat;
        struct
        {
            int size;
            struct Value* elements;
        } list;

    };
} Value;

int is_iterator(Value value);
void append_to_buffer(char** buffer, int* pos, int* size, const char* text);
void print_value(Value value, const char* end, int* err);
void free_value(Value value);
Value copy_value(Value value, int* err);
char* string_value(Value value, int* err);
int bool_value(Value value);

extern const Value EMPTY;
#endif
