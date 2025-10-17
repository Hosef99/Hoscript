#include <stdio.h>   // printf, scanf
#include <stdlib.h>  // malloc, free, atof, exit
#include <string.h>  // strcmp, strdup, strlen
#include <ctype.h>   // isdigit, isalpha, isspace
#include <math.h>

#include "mat_raw.h"
#include "value.h"
#include "mat.h"

const Value EMPTY;

Value zero_mat(Value* args, int n, int* err)
{
    Value res;
    res.type = VAL_MAT;

    // 2 input
    if (n != 2 || args[0].type != VAL_NUM || args[1].type != VAL_NUM)
    {
        fprintf(stderr, "zero expects 2 numeric arg\n");
        *err = 1;
        return res;
    }
    int rows = args[0].number;
    int cols = args[1].number;

    if (rows <= 0 || cols <= 0)
    {
        fprintf(stderr, "Matrix Error: dimension of zero matrix must be positive\n");
        *err = 1;
        return EMPTY;
    }

    double** mat = zero_mat_raw(rows, cols, err);
    
    res.mat.data = mat;
    res.mat.rows = rows;
    res.mat.cols = cols;

    return res;
}

Value ones_mat(Value* args, int n, int* err)
{
    Value res;
    res.type = VAL_MAT;

    // 2 input
    if (n != 2 || args[0].type != VAL_NUM || args[1].type != VAL_NUM)
    {
        fprintf(stderr, "ones expects 2 numeric arg\n");
        *err = 1;
        return res;
    }
    int rows = args[0].number;
    int cols = args[1].number;

    if (rows <= 0 || cols <= 0)
    {
        fprintf(stderr, "Matrix Error: dimension of ones matrix must be positive\n");
        *err = 1;
        return EMPTY;
    }

    double** mat = ones_mat_raw(rows, cols, err);
    
    res.mat.data = mat;
    res.mat.rows = rows;
    res.mat.cols = cols;

    return res;
}


Value ident_mat(Value* args, int n, int* err)
{
    Value res;
    res.type = VAL_MAT;

    // 1 input
    if (n != 1 || args[0].type != VAL_NUM)
    {
        fprintf(stderr, "ident expects 1 numeric arg\n");
        *err = 1;
        return res;
    }
    int rows = args[0].number;

    if (rows <= 0)
    {
        fprintf(stderr, "Matrix Error: dimension of ident matrix must be positive\n");
        *err = 1;
        return EMPTY;
    }

    double** mat = ident_mat_raw(rows, err);
    
    res.mat.data = mat;
    res.mat.rows = rows;
    res.mat.cols = rows;

    return res;
}

Value add_mat(Value val1, Value val2, int* err)
{
    if (val1.type != VAL_MAT || val2.type != VAL_MAT)
    {
        *err = 1;
        fprintf(stderr, "Value Error: value is not matrix\n");
    }
    
    // dimension check
    if (val1.mat.rows != val2.mat.rows || val1.mat.cols != val2.mat.cols)
    {
        fprintf(stderr, "Matrix Error: dimension of mat1 and mat2 is not the same\n");
        *err = 1;
        return EMPTY;
    }

    int rows = val1.mat.rows;
    int cols = val1.mat.cols;

    double** mat = add_mat_raw(val1.mat.data, val2.mat.data, rows, cols, err);

    Value res;
    res.type = VAL_MAT;
    res.mat.rows = rows;
    res.mat.cols = cols;
    res.mat.data = mat;
    
    return res;
}

Value sub_mat(Value val1, Value val2, int* err)
{
    if (val1.type != VAL_MAT || val2.type != VAL_MAT)
    {
        *err = 1;
        fprintf(stderr, "Value Error: value is not matrix\n");
    }
    
    // dimension check
    if (val1.mat.rows != val2.mat.rows || val1.mat.cols != val2.mat.cols)
    {
        fprintf(stderr, "Matrix Error: dimension of mat1 and mat2 is not the same\n");
        *err = 1;
        return EMPTY;
    }

    int rows = val1.mat.rows;
    int cols = val1.mat.cols;

    double** mat = sub_mat_raw(val1.mat.data, val2.mat.data, rows, cols, err);

    Value res;
    res.type = VAL_MAT;
    res.mat.rows = rows;
    res.mat.cols = cols;
    res.mat.data = mat;
    
    return res;
}

Value scalar_mat(Value val, Value scalar, int* err)
{
    if (val.type != VAL_MAT || scalar.type != VAL_NUM)
    {
        *err = 1;
        fprintf(stderr, "Value Error: value is not matrix\n");
        return EMPTY;
    }
    int rows = val.mat.rows;
    int cols = val.mat.cols;

    Value res;
    res.type = VAL_MAT;
    res.mat.rows = rows;
    res.mat.cols = cols;
    res.mat.data = scalar_mat_raw(val.mat.data, scalar.number, rows, cols, err);
    
    return res;
}

Value mul_mat(Value val1, Value val2, int* err)
{
    if (val1.type != VAL_MAT || val2.type != VAL_MAT)
    {
        *err = 1;
        fprintf(stderr, "Value Error: value is not matrix\n");
        return EMPTY;
    }
    
    // dimension check n x m, n = rows, m = cols

    int rows1 = val1.mat.rows;
    int cols1 = val1.mat.cols;
    int rows2 = val2.mat.rows;
    int cols2 = val2.mat.cols;

    // col of val1 needs to be equal to row of val2
    if (cols1 != rows2)
    {
        *err = 1;
        fprintf(stderr, "Matrix Error: cols of mat1 and rows of mat2 is not the same\n");
        return EMPTY;
    }


    double** mat = mul_mat_raw(val1.mat.data, val2.mat.data, rows1, cols1, cols2, err);
    
    Value res;
    res.type = VAL_MAT;
    res.mat.rows = rows1;
    res.mat.cols = cols2;
    res.mat.data = mat;

    return res;
}

Value pow_mat(Value val, Value pow, int* err)
{
    // dimension check, if the matrix is square
    if (val.mat.rows != val.mat.cols)
    {
        fprintf(stderr, "Matrix Error: cannot raise a non-square matrix to a power\n");
        *err = 1;
        return EMPTY;
    }

    if (pow.number < 0)
    {
        fprintf(stderr, "Matrix Error: cannot raise a matrix to a negative power\n");
    }

    Value res = {.type = VAL_MAT, .mat.rows = val.mat.rows, .mat.cols = val.mat.cols};
    res.mat.data = pow_mat_raw(val.mat.data, pow.number, val.mat.rows, err);

    return res;
}

Value det_mat(Value* args, int n, int* err)
{
    Value res;
    res.type = VAL_NUM;

    if (n != 1 || args[0].type != VAL_MAT)
    {
        fprintf(stderr, "det expects 1 matrix arg\n");
        *err = 1;
        return res;
    }

    Value val = args[0];

    // dimension check, need to be square
    if (val.mat.rows != val.mat.cols)
    {
        *err = 1;
        fprintf(stderr, "Matrix Error: not a square matrix\n");
        return EMPTY;
    }

    res.number = det_mat_raw(val.mat.data, val.mat.rows, err);
    return res;
}

Value inv_mat(Value* args, int n, int* err) 
{
    Value res;
    res.type = VAL_MAT;

    if (n != 1 || args[0].type != VAL_MAT)
    {
        fprintf(stderr, "inv expects 1 matrix arg\n");
        *err = 1;
        return res;
    }

    Value val = args[0];

    // dimension check, need to be square
    if (val.mat.rows != val.mat.cols)
    {
        *err = 1;
        fprintf(stderr, "Matrix Error: not a square matrix\n");
        return EMPTY;
    }
    
    res.mat.data = inv_mat_raw(val.mat.data, val.mat.rows, err);
    res.mat.rows = val.mat.rows;
    res.mat.cols = val.mat.cols;
    return res;
}
