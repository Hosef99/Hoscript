#define _POSIX_C_SOURCE 200809L
#include <stdio.h>   // printf, scanf
#include <stdlib.h>  // malloc, free, atof, exit
#include <string.h>  // strcmp, strdup, strlen
#include <ctype.h>   // isdigit, isalpha, isspace
#include <math.h>

#include "eval.h"

#define VAR_MAX 100

#define DEF_UNARY(name) \
Value builtin_##name(Value *args, int n, int* err) { \
    Value res; \
    res.type = VAL_NUM; \
    if (n != 1 || args[0].type != VAL_NUM) { \
        fprintf(stderr, #name " expects 1 numeric arg\n"); \
        res.number = NAN; \
        *err = 1; \
        return res; \
    } \
    res.number = name(args[0].number); \
    return res; \
}

#define DEF_BINARY(name) \
Value builtin_##name(Value *args, int n, int* err) { \
    Value res; \
    res.type = VAL_NUM; \
    if (n != 2 || args[0].type != VAL_NUM || args[1].type != VAL_NUM) { \
        fprintf(stderr, #name " expects 2 numeric args\n"); \
        res.number = NAN; \
        *err = 1; \
        return res; \
    } \
    res.number = name(args[0].number, args[1].number); \
    return res; \
}


Value PI()
{
    Value res;
    res.type = VAL_NUM;
    res.number = 3.1415926535897932384626;
    return res;
}

Value deg_to_rad(Value* args, int n, int* err)
{
    Value res;
    res.type = VAL_NUM;

    if (n != 1 || args[0].type != VAL_NUM)
    {
        fprintf(stderr, "deg2rad expects 1 numeric arg\n");
        *err = 1;
        return res;
    }
    
    res.number = args[0].number/180*PI().number;
    return res;
}

Value rad_to_deg(Value* args, int n, int* err)
{
    Value res;
    res.type = VAL_NUM;

    if (n != 1 || args[0].type != VAL_NUM)
    {
        fprintf(stderr, "rad2deg expects 1 numeric arg\n");
        *err = 1;
        return res;
    }

    res.number = args[0].number/PI().number*180;
    return res;
}

Value zero(Value* args, int n, int* err)
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

    return zero_mat(rows, cols, err);
}

Value ident(Value* args, int n, int* err)
{
    Value res;
    res.type = VAL_MAT;

    if (n != 1 || args[0].type != VAL_NUM)
    {
        fprintf(stderr, "ident expects 1 numeric arg\n");
        *err = 1;
        return res;
    }

    return ident_mat(args[0].number, err);
}

Value det(Value* args, int n, int* err)
{
    Value res;
    res.type = VAL_MAT;

    if (n != 1 || args[0].type != VAL_MAT)
    {
        fprintf(stderr, "det expects 1 matrix arg\n");
        *err = 1;
        return res;
    }

    return det_mat(args[0], err);
}

Value inv(Value* args, int n, int* err)
{
    Value res;
    res.type = VAL_MAT;

    if (n != 1 || args[0].type != VAL_MAT)
    {
        fprintf(stderr, "inv expects 1 matrix arg\n");
        *err = 1;
        return res;
    }

    return inv_mat(args[0], err);
}

DEF_UNARY(sin)
DEF_UNARY(cos)
DEF_UNARY(tan)
DEF_UNARY(asin)
DEF_UNARY(acos)
DEF_UNARY(atan)
// DEF_UNARY(deg_to_rad)
// DEF_UNARY(rad_to_deg)
DEF_UNARY(sinh)
DEF_UNARY(cosh)
DEF_UNARY(tanh)
DEF_UNARY(exp)
DEF_UNARY(log)
DEF_UNARY(log10)
DEF_UNARY(fabs)
DEF_UNARY(floor)
DEF_UNARY(ceil)
DEF_UNARY(round)
DEF_BINARY(fmod)
DEF_UNARY(sqrt)
DEF_BINARY(pow)

BuiltinEntry builtins[] = {
    {"sin",     builtin_sin,            1, "sin(num): sine of angle"},
    {"cos",     builtin_cos,            1, "cos(num): cosine of angle"},
    {"tan",     builtin_tan,            1, "tan(num): tangent of angle"},
    {"asin",    builtin_asin,           1, "asin(num): inverse sine"},
    {"acos",    builtin_acos,           1, "acos(num): inverse cosine"},
    {"atan",    builtin_atan,           1, "atan(num): inverse tangent"},
    {"rad2deg", rad_to_deg,             1, "rad2deg(rad): convert radians to degrees"},
    {"deg2rad", deg_to_rad,             1, "deg2rad(deg): convert degrees to radians"},
    {"sinh",    builtin_sinh,           1, "sinh(num): hyperbolic sine"},
    {"cosh",    builtin_cosh,           1, "cosh(num): hyperbolic cosine"},
    {"tanh",    builtin_tanh,           1, "tanh(num): hyperbolic tangent"},
    {"exp",     builtin_exp,            1, "exp(num): exponential function e^x"},
    {"log",     builtin_log,            1, "log(num): natural logarithm (base e)"},
    {"log10",   builtin_log10,          1, "log10(num): logarithm base 10"},
    {"abs",     builtin_fabs,           1, "abs(num): absolute value"},
    {"floor",   builtin_floor,          1, "floor(num): round down to nearest integer"},
    {"ceil",    builtin_ceil,           1, "ceil(num): round up to nearest integer"},
    {"round",   builtin_round,          1, "round(num): round to nearest integer"},
    {"fmod",    builtin_fmod,           2, "fmod(x, y): remainder of division x mod y"},
    {"sqrt",    builtin_sqrt,           1, "sqrt(num): square root"},
    {"pow",     builtin_pow,            2, "pow(x, y): raise x to power y"},
    {"pi",      PI,                     0, "pi(): mathematical constant 3.14159"},
    {"zero",    zero,                   2, "zero(rows, cols): create a zero matrix"},
    {"ident",   ident,                  1, "ident(dim): create an identity matrix"},
    {"det",     det,                    1, "det(mat): determinant of a matrix"},
    {"inv",     inv,                    1, "inv(mat): inverse of a matrix"},
    {NULL,      NULL, 0, NULL} // sentinel
};


Value copy_value(Value val);
Value eval(ASTNode* root, Env* env, int* err);
Value get_var(const char* name, Env* env, int* err);
void set_var(const char* name, Value value, Env* env, int* err);
Value calc_func(const char* name, Value* args, int arg_size, int* err);

Value copy_value(Value val)
{
    Value res;
    res.type = val.type;
    if (val.type == VAL_NUM)
    {
        res.number = val.number;
    }
    else if (val.type == VAL_MAT)
    {
        res.mat.rows = val.mat.rows;
        res.mat.cols = val.mat.cols;
        res.mat.data = cpy_mat(val.mat.data, val.mat.rows, val.mat.cols);
    }

    return res;
}


void print_value(Value val, int* err)
{
    switch (val.type)
    {
    case VAL_EMP:
        printf("Empty value\n");
        break;
    case VAL_NUM:
        printf("%.15g\n", val.number);
        break;
    case VAL_MAT:
        print_mat(val, err);
        break;
    default:
        fprintf(stderr, "Value Error: Unknown type\n");
        *err = 1;
        break;
    }
}

Value eval_assign(ASTNode* node, Env* env, int* err)
{
    Value a = eval(node->assign.expr, env, err);
    if (*err) return EMPTY;
    set_var(node->assign.name, a, env, err);
    if (*err) return EMPTY;
    printf("%s = ", node->assign.name);
    if (a.type == VAL_MAT)
        printf("\n");
    return a;
}

Value eval_addition(ASTNode* node, Env* env, int* err)
{
    Value a, b, res;
    a = eval(node->binary.left, env, err);
    if (*err) return EMPTY;
    b = eval(node->binary.right, env, err);
    if (*err) return EMPTY;

    if (a.type != b.type)
    {
        fprintf(stderr, "Value Error: cannot perform addition on different types\n");
        *err = 1;
        return EMPTY;
    }
    switch (a.type)
    {
    case VAL_NUM:
        res.type = VAL_NUM;
        res.number = a.number + b.number;
        return res;
    case VAL_MAT:
        res = add_mat(a, b, err);
        if (*err) return EMPTY;
        return res;
    default:
        fprintf(stderr, "Value Error: value is empty\n");
        *err = 1;
        return EMPTY;
    }
}

Value eval_subtraction(ASTNode* node, Env* env, int* err)
{
    Value a, b, res;
    a = eval(node->binary.left, env, err);
    if (*err) return EMPTY;
    b = eval(node->binary.right, env, err);
    if (*err) return EMPTY;

    if (a.type != b.type)
    {
        fprintf(stderr, "Value Error: cannot perform subtraction on different types\n");
        *err = 1;
        return EMPTY;
    }
    switch (a.type)
    {
    case VAL_NUM:
        res.type = VAL_NUM;
        res.number = a.number - b.number;
        return res;
    case VAL_MAT:
        res = sub_mat(a, b, err);
        if (*err) return EMPTY;
        return res;
    default:
        fprintf(stderr, "Value Error: value is empty\n");
        *err = 1;
        return EMPTY;
    }
}

Value eval_multiplication(ASTNode* node, Env* env, int* err)
{
    Value a, b, res;
    a = eval(node->binary.left, env, err);
    if (*err) return EMPTY;
    b = eval(node->binary.right, env, err);
    if (*err) return EMPTY;

    if ((a.type == VAL_MAT && b.type == VAL_NUM) || (b.type == VAL_MAT && a.type == VAL_NUM))
    {
        // scalar * matrix multiplication
        if (a.type == VAL_MAT)
        {
            return scalar_mat(a, b);
        }
        else
        {
            return scalar_mat(b, a);
        }
        
    }
    switch (a.type)
    {
    case VAL_NUM:
        res.type = VAL_NUM;
        res.number = a.number * b.number;
        return res;
    case VAL_MAT:
        res = mul_mat(a, b, err);
        if (*err) return EMPTY;
        return res;
    default:
        fprintf(stderr, "Value Error: value is empty\n");
        *err = 1;
        return EMPTY;
    }
}

Value eval_division(ASTNode* node, Env* env, int* err)
{
    Value a, b, res;
    a = eval(node->binary.left, env, err);
    if (*err) return EMPTY;
    b = eval(node->binary.right, env, err);
    if (*err) return EMPTY;

    if (a.type == VAL_MAT)
    {
        fprintf(stderr, "Value Error: cannot perform division on matrices\n");
        *err = 1;
        return EMPTY;
    }
    if (b.number != 0)
    {
        res.type = VAL_NUM;
        res.number = a.number / b.number;
        return res;
    }
    else 
    {
        fprintf(stderr, "Value Error: division by zero\n");
        *err = 1;
        return EMPTY;
    }
}

Value eval_exponentiation(ASTNode* node, Env* env, int* err)
{
    Value a, b, res;
    a = eval(node->binary.left, env, err);
    if (*err) return EMPTY;
    b = eval(node->binary.right, env, err);
    if (*err) return EMPTY;

    switch (a.type)
    {
    case VAL_NUM:
        res.type = VAL_NUM;
        res.number = pow(a.number, b.number);
        return res;
    case VAL_MAT:
        res = pow_mat(a, b, err);
        if (*err) return EMPTY;
        return res;
    default:
        fprintf(stderr, "Value Error: value is empty\n");
        *err = 1;
        return EMPTY;
    }
}

Value eval_binop(ASTNode* node, Env* env, int* err)
{
    switch (node->binary.op)
    {
    case TOK_PLUS:
        return eval_addition(node, env, err);
    case TOK_MINUS:
        return eval_subtraction(node, env, err);
    case TOK_MUL:
        return eval_multiplication(node, env, err);
    case TOK_DIV:
        return eval_division(node, env, err);
    case TOK_POW:
        return eval_exponentiation(node, env, err);
    default:
        return EMPTY;
    }
}

Value eval_func(ASTNode* node, Env* env, int* err)
{
    Value* args = malloc(sizeof(Value) * node->func.arg_size);
    for (int i = 0; i < node->func.arg_size; i++)
    {
        args[i] = eval(node->func.args[i], env, err);
        if (*err) 
        {
            free(args);
            return EMPTY;
        }
    }
    Value a = calc_func(node->func.name, args, node->func.arg_size, err);
    if (*err) return EMPTY;    
    for (int i = 0; i < node->func.arg_size; i++)
    {
        free_value(args[i]);
    }
    free(args);
    return a;
}

Value eval_unop(ASTNode* node, Env* env, int* err)
{
    Value a = eval(node->binary.right, env, err);
    Value b;
    switch (a.type)
    {
    case VAL_NUM:
        a.number = -a.number;
        return a;
    case VAL_MAT:
        b.type = VAL_NUM;
        b.number = -1;
        a = scalar_mat(a, b);
        return a;
    default:
        fprintf(stderr, "Value Error: cannot subtract empty value\n");
        *err = 1;
        return EMPTY;
    }
}

Value eval_mat(ASTNode* node, Env* env, int* err)
{
    // start evaluating matrices
    Value a, res;
    ASTNode* row;
    int rows = node->mat.rows;
    int cols = node->mat.matrix[0]->row.cols;
    res.type = VAL_MAT;
    res.mat.rows = rows;
    res.mat.cols = cols;
    res.mat.data = alloc_mat(rows, cols);

    for (int i = 0; i < rows; i++) // there will be many bugs here please notice ????????
    {
        row = node->mat.matrix[i];
        if (row->row.cols != cols)
        {
            fprintf(stderr, "Matrix Error: columns not consistent\n");
            *err = 1;
            return EMPTY;
        }
        
        for (int j = 0; j < cols; j++)
        {
            a = eval(row->row.elements[j], env, err);
            if (a.type != VAL_NUM)
            {
                fprintf(stderr, "Value Error: contents of matrix must be a double\n");
                *err = 1;
                return EMPTY;
            }
            
            res.mat.data[i][j] = a.number;
        }
    }
    return res;
}

Value eval(ASTNode* node, Env* env, int* err)
{
    Value a;
    switch (node->type)
    {
    case AST_ASSIGN:
        return eval_assign(node, env, err);
    case AST_NUM:
        a.type = VAL_NUM;
        a.number = node->value;
        return a;
    case AST_BINOP:
        return eval_binop(node, env, err);
    case AST_IDENT:
        return get_var(node->name, env, err);
    case AST_FUNC:
        return eval_func(node, env, err);
    case AST_UNOP:
        return eval_unop(node, env, err);
    case AST_MAT:
        return eval_mat(node, env, err);
    default:
        return EMPTY;
        break;
    }
}

Value get_var(const char* name, Env* env, int* err)
{
    for (int i = 0; i < env->var_count; i++)
    {
        if (strcmp(env->var[i].name, name) == 0) // if equal strcmp returns 0
        {
            *err = 0;
            return copy_value(env->var[i].value);
        }
    }
    fprintf(stderr, "Variable error: variable %s not found\n", name);
    *err = 1;
    return EMPTY;
}

void set_var(const char* name, Value value, Env* env, int* err)
{
    for (int i = 0; i < env->var_count; i++) // check reassignment
    {
        if (strcmp(env->var[i].name, name) == 0) // if equal strcmp returns 0, variable found
        {
            free_value(env->var[i].value); // so that matrix in the value can be freed
            env->var[i].value = value;
            return;
        }
    }

    // if var_count is already full, then give error
    if (env->var_count == VAR_MAX)
    {
        fprintf(stderr, "Variable error: max number of variable reached\n");
        *err = 1;
    }
    
    // variable not found, create a new one and increment var_count
    env->var[env->var_count].name = strdup(name); // need to free
    env->var[env->var_count].value = value;
    (env->var_count)++;
    *err = 0;
}

Value calc_func(const char* name, Value* args, int arg_size, int* err)
{
    // compare name with all function name
    for (int i = 0; builtins[i].name != NULL; i++)
    {
        if (strcmp(builtins[i].name, name) == 0) // 0 is equal
        {
            if (arg_size != builtins[i].arg_size)
            {
                fprintf(stderr, "Function %s expects %d args, got %d\n", name, builtins[i].arg_size, arg_size);
                *err = 1;
                return EMPTY;
            }
            else
            {   
                Value res = builtins[i].func(args, arg_size, err);
                if (*err) return EMPTY;
                return res;
            }
        }
    }
    // name not in the list of built in functions, print error
    fprintf(stderr, "Function %s not defined\n", name);
    *err = 1;
    return EMPTY;
}

void print_help()
{
    printf("Available commands:\n");
    printf("  quit   - Exit the program\n");
    printf("  help   - Show this help message\n\n");

    printf("Available functions:\n");
    for (int i = 0; builtins[i].name != NULL; i++) {
        printf("  %-8s (%d args) - %s\n", 
               builtins[i].name, 
               builtins[i].arg_size, 
               builtins[i].description);
    }

    printf("\nOperators: +  -  *  /  ^  =\n\n");

    printf("Matrix syntax:\n");
    printf("  [a,b,c;d,e,f;g,h,i]\n");
    printf("  Example: [1,2,3;4,5,6;7,8,9]\n");
    printf("  Use commas for columns and semicolons for new rows.\n");
}
