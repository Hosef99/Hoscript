#ifndef MAT_H
#define MAT_H

typedef enum
{
    VAL_NUM,
    VAL_MAT,
    VAL_EMP
} ValueType;

typedef struct
{
    ValueType type;
    union
    {
        double number;
        struct
        {
            int rows;
            int cols;
            double** data;
        }mat;
    };
    
} Value;

extern const Value EMPTY;

double** alloc_mat(int rows, int cols);
void free_mat(double** mat, int rows);
void free_value(Value val);
double** cpy_mat(double** mat, int rows, int cols);
void print_mat(Value matrix, int* err);
Value zero_mat(int rows, int cols, int* err);
Value one_mat(int rows, int cols, int* err);
Value ident_mat(int n, int* err);
Value add_mat(Value val1, Value val2, int* err);
Value sub_mat(Value val1, Value val2, int* err);
Value scalar_mat(Value val, Value scalar);
Value mul_mat(Value val1, Value val2, int* err);
Value pow_mat(Value val, Value pow, int* err);
Value det_mat(Value val, int* err);
Value inv_mat(Value val, int* err);

#endif
