#ifndef MAT_RAW_H
#define MAT_RAW_H

double** alloc_mat(int rows, int cols);
void free_mat(double** mat, int rows);
double** cpy_mat(double** mat, int rows, int cols, int* err);
void print_mat(double** mat, int rows, int cols, int* err);
double** zero_mat_raw(int rows, int cols, int* err);
double** ident_mat_raw(int rows, int* err);
double** add_mat_raw(double** mat_a, double** mat_b, int rows, int cols, int* err);
double** sub_mat_raw(double** mat_a, double** mat_b, int rows, int cols, int* err);
double** scalar_mat_raw(double** mat, double scalar, int rows, int cols, int* err);
double** mul_mat_raw(double** mat_a, double** mat_b, int rows_a, int cols_a, int cols_b, int* err);
double** pow_mat_raw(double** mat, double power, int rows, int* err);
double det_mat_raw(double** mat, int rows, int* err);
double** inv_mat_raw(double** mat, int rows, int* err);
#endif
