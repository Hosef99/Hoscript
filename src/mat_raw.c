#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "mat_raw.h"

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

double** cpy_mat(double** mat, int rows, int cols, int* err) // whatever is returned, needs to be freed
{
    double** new = alloc_mat(rows, cols);
    if (!new) {
        fprintf(stderr, "Memory Error: allocation failed\n");
        *err = 1;
        return 0;
    }
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            new[i][j] = mat[i][j];
        }
    }

    return new;
}

void print_mat(double** mat, int rows, int cols, int* err)
{
    if (!mat || rows <= 0 || cols <= 0) {
        fprintf(stderr, "Matrix Error: invalid input\n");
        *err = 1;
        return;
    }

    int max_width = 0;
    char buffer[64];

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            sprintf(buffer, "%g", mat[i][j]);
            int len = 0;
            while (buffer[len] != '\0') len++;
            if (len > max_width) max_width = len;
        }
    }

    printf("[\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%*g ", max_width, mat[i][j]);
        }
        printf("\n");
    }
    printf("]\n\n");
}

double** zero_mat_raw(int rows, int cols, int* err)
{
    if (rows <= 0 || cols <= 0) {
        fprintf(stderr, "Matrix Error: invalid input\n");
        *err = 1;
        return NULL;
    }

    double** result = alloc_mat(rows, cols);
    if (!result) {
        fprintf(stderr, "Memory Error: allocation failed\n");
        *err = 1;
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[i][j] = 0;
        }
    }
    
    return result;
}

double** ones_mat_raw(int rows, int cols, int* err)
{
    if (rows <= 0 || cols <= 0) {
        fprintf(stderr, "Matrix Error: invalid input\n");
        *err = 1;
        return NULL;
    }

    double** result = alloc_mat(rows, cols);
    if (!result) {
        fprintf(stderr, "Memory Error: allocation failed\n");
        *err = 1;
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[i][j] = 1;
        }
    }

    return result;
}

double** ident_mat_raw(int rows, int* err)
{
    if (rows <= 0) {
        fprintf(stderr, "Matrix Error: invalid input\n");
        *err = 1;
        return NULL;
    }

    double** result = zero_mat_raw(rows, rows, err);
    if (!result) {
        fprintf(stderr, "Memory Error: allocation failed\n");
        *err = 1;
        return NULL;
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < rows; j++) {
            if (i == j) {
                result[i][j] = 1;
            }
        } 
    }
    return result;
}

double** add_mat_raw(double** mat_a, double** mat_b, int rows, int cols, int* err)
{
    if (!mat_a || !mat_b || rows < 0 || cols < 0){
        fprintf(stderr, "Matrix Error: invalid input\n");
        *err = 1;
        return NULL;
    }

    double** result = alloc_mat(rows, cols);
    if(!result){
        fprintf(stderr, "Memory Error: allocation failed\n");
        *err = 1;
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[i][j] = mat_a[i][j] + mat_b[i][j];
        }
    } 
    return result;
}


double** sub_mat_raw(double** mat_a, double** mat_b, int rows, int cols, int* err)
{
    if (!mat_a || !mat_b || rows < 0 || cols < 0){
        fprintf(stderr, "Matrix Error: invalid input\n");
        *err = 1;
        return NULL;
    }

    double** result = alloc_mat(rows, cols);
    if(!result){
        fprintf(stderr, "Memory Error: allocation failed\n");
        *err = 1;
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[i][j] = mat_a[i][j] - mat_b[i][j];
        }
    } 
    return result;
}


double** scalar_mat_raw(double** mat, double scalar, int rows, int cols, int* err)
{
    if (!mat || rows <= 0 || cols <= 0)
    {
        fprintf(stderr, "Matrix Error: invalid input\n");
        *err = 1;
        return NULL;
    }
    double** result = cpy_mat(mat, rows, cols, err);

    if (!result)
    {
        fprintf(stderr, "Memory Error: allocation failed\n");
        *err = 1;
        return NULL;
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            result[i][j] *= scalar;
        }
    }
    
    return result;
}

double** mul_mat_raw(double** mat_a, double** mat_b, int rows_a, int cols_a, int cols_b, int* err)
{
    if (!mat_a || mat_b || rows_a <= 0 || cols_a <= 0 || cols_b <= 0)
    {
        fprintf(stderr, "Matrix Error: invalid input\n");
        *err = 1;
        return NULL;
    }

    double** result = alloc_mat(rows_a, cols_b);
    if (!result)
    {
        fprintf(stderr, "Memory Error: allocation failed\n");
        *err = 1;
        return NULL;
    }

    for (int i = 0; i < rows_a; i++)
    {
        for (int j = 0; j < cols_b; j++)
        {
            result[i][j] = 0;
            for (int k = 0; k < cols_a; k++)
            {
                result[i][j] += mat_a[i][k]*mat_b[k][j];
            }
        }
    }

    return result;
}


double** pow_mat_raw(double** mat, double power, int rows, int* err)
{
    if (!mat || power < 0 || rows <= 0) {
        fprintf(stderr, "Matrix Error: invalid input\n");
        *err = 1;
        return NULL;
    }
    int n = rows;
    
    if (power == 0)
    {
        return ident_mat_raw(n, err);
    }
    
    double** base = cpy_mat(mat, n, n, err);
    if (!base) {
        fprintf(stderr, "Matrix Error: invalid input\n");
        *err = 1;
        return NULL;
    }

    double** result = cpy_mat(mat, n, n, err);
    if (!base) {
        fprintf(stderr, "Matrix Error: invalid input\n");
        *err = 1;
        return NULL;
    }

    for (int i = 0; i < power - 1; i++)
    {
        double** temp = result;
        result = mul_mat_raw(result, base, rows, rows, rows, err);
        free_mat(temp, rows);
    }
    
    return result;
}

double det_mat_raw(double** mat, int rows, int* err)
{
    if (!mat || rows <= 0) {
        fprintf(stderr, "Matrix Error: invalid input\n");
        *err = 1;
        return 0;
    }

    int n = rows;
    
    double** temp = cpy_mat(mat, n, n, err);
    if (!temp) {
        fprintf(stderr, "Memory Error: allocation failed\n");
        *err = 1;
        return 0;
    }

    double det = 1.0;
    int sign = 1;

    for (int j = 0; j < n; j++) {
        int pivot = j;
        for (int i = j + 1; i < n; i++) {
            if (fabs(temp[i][j]) > fabs(temp[pivot][j])) {
                pivot = i;
            } 
        }

        if (fabs(temp[pivot][j]) < 1e-12)
        {
            det = 0;
            break;
        }
        
        if (pivot != j)
        {
            double* tempRow = temp[pivot];
            temp[pivot] = temp[j];
            temp[j] = tempRow;
            sign *= -1;
        }

        for (int row = j + 1; row < n; row++)
        {
            double scale = temp[row][j]/temp[j][j];
            for (int k = 0; k < n; k++)
            {
                temp[row][k] -= scale*temp[j][k];
            }
        }
    }

    for (int i = 0; i < n; i++){
        det *= temp[i][i];
    }

    det *= sign;
    free_mat(temp, n);
    return det;
}

double** inv_mat_raw(double** mat, int rows, int* err)
{
    if (!mat || rows <= 0) {
        fprintf(stderr, "Matrix Error: invalid input\n");
        *err = 1;
        return NULL;
    }

    int n = rows;
    
    double** temp = cpy_mat(mat, n, n, err);
    if (!temp) {
        fprintf(stderr, "Memory Error: allocation failed\n");
        *err = 1;
        return NULL;
    }

    double** aug = alloc_mat(n, 2*n);
    if(!aug){
        fprintf(stderr, "Memory Error: allocation failed\n");
        *err = 1;
        return NULL;
    }

    // initialize augmented matrix
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            aug[i][j] = mat[i][j];
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
            return NULL;
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

    double** result = alloc_mat(n, n);
    if (!temp) {
        fprintf(stderr, "Memory Error: allocation failed\n");
        *err = 1;
        return 0;
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            result[i][j] = aug[i][j+n];
        }
    }
    free_mat(aug, n);
    return result;
}
