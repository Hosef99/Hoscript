#ifndef MAT_H
#define MAT_H

#include "value.h"

Value zero_mat(Value* args, int n, int* err);
Value ones_mat(Value* args, int n, int* err);
Value ident_mat(Value* args, int n, int* err);
Value add_mat(Value val1, Value val2, int* err);
Value sub_mat(Value val1, Value val2, int* err);
Value scalar_mat(Value val, Value scalar, int* err);
Value mul_mat(Value val1, Value val2, int* err);
Value pow_mat(Value val, Value pow, int* err);
Value det_mat(Value* args, int n, int* err);
Value inv_mat(Value* args, int n, int* err);

#endif
