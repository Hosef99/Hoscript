#include <stdio.h>   // printf, scanf
#include <stdlib.h>  // malloc, free, atof, exit
#include <string.h>  // strcmp, strdup, strlen
#include <ctype.h>   // isdigit, isalpha, isspace

#include "mat.h"

const Value EMPTY;

double** alloc_mat(int rows, int cols)
{
    double** mat = malloc(rows * sizeof(double*));

    for (int i = 0; i < rows; i++)
    {
        mat[i] = malloc(cols * sizeof(double));
    }

    return mat;
}

void free_mat(double** mat, int rows)
{
    for (int i = 0; i < rows; i++)
    {
        free(mat[i]);
    }
    free(mat);
}

void print_mat(Value matrix, int* err)
{
    // check if it is matrix
    if (matrix.type != VAL_MAT)
    {
        *err = 1;
        fprintf(stderr, "Value Error: value is not matrix\n");
    }

    printf("\n[\n");
    for (int i = 0; i < matrix.mat.rows; i++)
    {
        for (int j = 0; j < matrix.mat.cols; j++)
        {
            printf("%10.4lf ", matrix.mat.data[i][j]);
        }
        printf("\n");
    }
    printf("]\n");
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

    double** mat = alloc_mat(rows, cols);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            mat[i][j] = val1.mat.data[i][j] + val2.mat.data[i][j];   
        }
    }

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

    double** mat = alloc_mat(rows, cols);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            mat[i][j] = val1.mat.data[i][j] - val2.mat.data[i][j];   
        }
    }

    Value res;
    res.type = VAL_MAT;
    res.mat.rows = rows;
    res.mat.cols = cols;
    res.mat.data = mat;
    
    return res;
}

Value mul_mat(Value val1, Value val2, int* err)
{
    if (val1.type != VAL_MAT || val2.type != VAL_MAT)
    {
        *err = 1;
        fprintf(stderr, "Value Error: value is not matrix\n");
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


    double** mat = alloc_mat(rows1, cols2);
    for (int i = 0; i < rows1; i++)
    {
        for (int j = 0; j < cols2; j++)
        {
            mat[i][j] = 0;
            for (int k = 0; k < cols1; k++)
            {
                mat[i][j] += val1.mat.data[i][k]*val2.mat.data[k][j];
            }
        }
    }
    
    Value res;
    res.type = VAL_MAT;
    res.mat.rows = rows1;
    res.mat.cols = cols2;
    res.mat.data = mat;

    return res;
}

Value det_mat(Value val, int* err) // recursive, receives a matrix value, returns a number value
{
    if (val.type != VAL_MAT)
    {
        *err = 1;
        fprintf(stderr, "Value Error: value is not matrix\n");
        return EMPTY;
    }

    // dimension check, need to be square
    if (val.mat.rows != val.mat.cols)
    {
        *err = 1;
        fprintf(stderr, "Matrix Error: not a square matrix\n");
        return EMPTY;
    }

    Value res;
    res.type = VAL_NUM;
    
    // base case: 1x1
    if (val.mat.rows == 1)
    {
        res.number = val.mat.data[0][0];
        return res;
    }
}

int main()
{
    int err = 0;
    double** mat1 = alloc_mat(2, 2);
    
    mat1[0][0] = 1;
    mat1[0][1] = 2;
    mat1[1][0] = 3;
    mat1[1][1] = 4;

    double** mat2 = alloc_mat(2, 2);

    mat2[0][0] = 5;
    mat2[0][1] = 6;
    mat2[1][0] = 7;
    mat2[1][1] = 8;

    Value val1, val2;
    val1.type = VAL_MAT;
    val1.mat.rows = 2;
    val1.mat.cols = 2;
    val1.mat.data = mat1;

    val2.type = VAL_MAT;
    val2.mat.rows = 2;
    val2.mat.cols = 2;
    val2.mat.data = mat2;

    Value add = add_mat(val1, val2, &err);
    Value sub = sub_mat(val1, val2, &err);
    Value mul = mul_mat(val1, val2, &err);

    print_mat(add, &err);
    print_mat(sub, &err);
    print_mat(mul, &err);
    return 0;
}