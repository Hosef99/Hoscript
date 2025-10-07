#include <stdio.h>   // printf, scanf
#include <stdlib.h>  // malloc, free, atof, exit
#include <string.h>  // strcmp, strdup, strlen
#include <ctype.h>   // isdigit, isalpha, isspace
#include <math.h>

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

void free_value(Value val)
{
    if (val.type == VAL_MAT)
    {
        free_mat(val.mat.data, val.mat.rows);
    }
}

double** cpy_mat(double** mat, int rows, int cols) // whatever is returned, needs to be freed
{
    double** new = alloc_mat(rows, cols);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            new[i][j] = mat[i][j];
        }
    }

    return new;
}

void print_mat(Value matrix, int* err)
{
    // check if it is matrix
    if (matrix.type != VAL_MAT)
    {
        *err = 1;
        fprintf(stderr, "Value Error: value is not matrix\n");
    }

    printf("[\n");
    for (int i = 0; i < matrix.mat.rows; i++)
    {
        for (int j = 0; j < matrix.mat.cols; j++)
        {
            printf("%10.4lf ", matrix.mat.data[i][j]);
        }
        printf("\n");
    }
    printf("]\n\n");
}

Value zero_mat(int rows, int cols, int* err)
{

    if (rows <= 0 || cols <= 0)
    {
        fprintf(stderr, "Matrix Error: dimension of zero matrix must be positive\n");
        *err = 1;
        return EMPTY;
    }

    Value res;
    res.type = VAL_MAT;
    res.mat.data = alloc_mat(rows, cols);
    res.mat.rows = rows;
    res.mat.cols = cols;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            res.mat.data[i][j] = 0;
        }
    }

    return res;
}

Value ident_mat(int n, int* err)
{
    if (n <= 0)
    {
        fprintf(stderr, "Matrix Error: dimension of identity must be positive\n");
        *err = 1;
        return EMPTY;
    }
    
    Value res = zero_mat(n, n, err);

    for (int i = 0; i < n; i++)
    {
        res.mat.data[i][i] = 1;
    }
    
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

Value scalar_mat(Value val, Value scalar)
{
    int rows = val.mat.rows;
    int cols = val.mat.cols;

    Value res;
    res.type = VAL_MAT;
    res.mat.rows = rows;
    res.mat.cols = cols;
    res.mat.data = cpy_mat(val.mat.data, rows, cols);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            res.mat.data[i][j] *= scalar.number;
        }
    }
    
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

    int n = val.mat.rows;
    
    if (pow.number == 0)
    {
        return ident_mat(n, err);
    }
    

    double** base = cpy_mat(val.mat.data, n, n);
    double** mat = cpy_mat(val.mat.data, n, n);

    Value base_val;
    Value mat_val;
    base_val.type = VAL_MAT;
    mat_val.type = VAL_MAT;

    base_val.mat.data = base;
    base_val.mat.rows = n;
    base_val.mat.cols = n;
    mat_val.mat.data = mat; 
    mat_val.mat.rows = n;
    mat_val.mat.cols = n;

    for (int i = 0; i < pow.number - 1; i++)
    {
        Value temp = mat_val;
        mat_val = mul_mat(mat_val, base_val, err);
        free_value(temp);
    }
    
    free_value(base_val);

    return mat_val;
}

Value det_mat(Value val, int* err) // caller need to free res
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

    int n = val.mat.rows;
    double** temp = cpy_mat(val.mat.data, n, n);
    double det = 1.0;
    int sign = 1;

    for (int col = 0; col < n; col++)
    {
        int pivot = col;
        for (int row = col + 1; row < n; row++)
        {
            if (fabs(temp[row][col]) > fabs(temp[pivot][col]))
            {
                pivot = row;
            }
        }
        
        if (fabs(temp[pivot][col]) < 1e-12)
        {
            det = 0;
            break;
        }
        
        if (pivot != col)
        {
            double* tempRow = temp[pivot];
            temp[pivot] = temp[col];
            temp[col] = tempRow;
            sign *= -1;
        }

        for (int row = col + 1; row < n; row++)
        {
            double scale = temp[row][col]/temp[col][col];
            for (int k = 0; k < n; k++)
            {
                temp[row][k] -= scale*temp[col][k];
            }
        }
    }

    for (int i = 0; i < n; i++)
    {
        det *= temp[i][i];
    }

    det *= sign;
    free_mat(temp, n);
    res.number = det;
    return res;
}

Value inv_mat(Value val, int* err) // caller must free res
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

    int n = val.mat.rows;
    double** aug = alloc_mat(n, 2*n);

    // initialize augmented matrix
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            aug[i][j] = val.mat.data[i][j];
        }
        for (int j = 0; j < n; j++) {
            aug[i][j+n] = (i == j) ? 1.0 : 0.0;
        }
    }
    
    // begin rrefing the augmented matrix
    for (int col = 0; col < n; col++)
    {
        int pivot = col;
        for (int row = col + 1; row < n; row++)
        {
            if (fabs(aug[row][col]) > fabs(aug[pivot][col]))
            {
                pivot = row;
            }
        }
        
        if (fabs(aug[pivot][col]) < 1e-12)
        {
            fprintf(stderr, "Matrix Error: matrix is singular (no inverse)\n");
            free_mat(aug, n);
            return EMPTY;
        }

        if (pivot != col)
        {
            double* tempRow = aug[pivot];
            aug[pivot] = aug[col];
            aug[col] = tempRow;
        }

        // make current row normalized, so pivot = 1

        double div = aug[col][col];
        for (int i = 0; i < n*2; i++)
        {
            aug[col][i] /= div;
        }
        
        for (int row = 0; row < n; row++)
        {
            if (row == col) continue;
            double scale = aug[row][col];
            for (int i = 0; i < 2*n; i++)
            {
                aug[row][i] -= scale * aug[col][i];
            }
        }
    }
    
    double** inv = alloc_mat(n, n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            inv[i][j] = aug[i][j+n];
        }
    }
    free_mat(aug, n);
    Value res;
    res.type = VAL_MAT;
    res.mat.rows = n;
    res.mat.cols = n;
    res.mat.data = inv;

    return res;
}