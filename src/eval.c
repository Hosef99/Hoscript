#include <stddef.h>
#define _POSIX_C_SOURCE 200809L
#define IS_FILE 1
#define NOT_FILE 0
#include <ctype.h> // isdigit, isalpha, isspace
#include <math.h>
#include <stdio.h>  // printf, scanf
#include <stdlib.h> // malloc, free, atof, exit
#include <string.h> // strcmp, strdup, strlen
#include <stdbool.h>

#include "compat_readline.h"
#include "eval.h"
#include "file.h"
#include "lexer.h"
#include "mat.h"
#include "mat_raw.h"
#include "parser.h"
#include "value.h"

#define VAR_MAX 100

#define DEF_UNARY(name)                                                                            \
    Value builtin_##name(Value *args, int n, int *err)                                             \
    {                                                                                              \
        Value res;                                                                                 \
        res.type = VAL_NUM;                                                                        \
        if (n != 1 || args[0].type != VAL_NUM)                                                     \
        {                                                                                          \
            fprintf(stderr, #name " expects 1 numeric arg\n");                                     \
            res.number = NAN;                                                                      \
            *err = 1;                                                                              \
            return res;                                                                            \
        }                                                                                          \
        res.number = name(args[0].number);                                                         \
        return res;                                                                                \
    }

#define DEF_BINARY(name)                                                                           \
    Value builtin_##name(Value *args, int n, int *err)                                             \
    {                                                                                              \
        Value res;                                                                                 \
        res.type = VAL_NUM;                                                                        \
        if (n != 2 || args[0].type != VAL_NUM || args[1].type != VAL_NUM)                          \
        {                                                                                          \
            fprintf(stderr, #name " expects 2 numeric args\n");                                    \
            res.number = NAN;                                                                      \
            *err = 1;                                                                              \
            return res;                                                                            \
        }                                                                                          \
        res.number = name(args[0].number, args[1].number);                                         \
        return res;                                                                                \
    }

// GLOBAL VARIABLE
static Env *current_env = NULL;

// DECLARE FUNCTIONS HERE
// BUILT IN FUNCTIONS FOR INTERPRETERS
Value print(Value *args, int n, int *err);
Value quit(Value *args, int n, int *err);
Value help(Value *args, int n, int *err);
Value printvar(Value *args, int n, int *err);
Value clear_screen(Value* args, int n, int* err);

// EVAL RELATED FUNCTIONS
Value eval(ASTNode *root, Env *env, int *err);
Value get_var(const char *name, Env *env, int *err);
void set_var(const char *name, Value value, Env *env, int *err);
Value calc_func(const char *name, Value *args, int arg_size, int *err);
void assign_to_index(ASTNode *node, Value rhs, Env *env, int *err);

void set_current_env(Env *env)
{
    current_env = env;
}

Value PI()
{
    Value res;
    res.type = VAL_NUM;
    res.number = 3.1415926535897932384626;
    return res;
}

Value deg_to_rad(Value *args, int n, int *err)
{
    Value res;
    res.type = VAL_NUM;

    if (n != 1 || args[0].type != VAL_NUM)
    {
        fprintf(stderr, "deg2rad expects 1 numeric arg\n");
        *err = 1;
        return res;
    }

    res.number = args[0].number / 180 * PI().number;
    return res;
}

Value rad_to_deg(Value *args, int n, int *err)
{
    Value res;
    res.type = VAL_NUM;

    if (n != 1 || args[0].type != VAL_NUM)
    {
        fprintf(stderr, "rad2deg expects 1 numeric arg\n");
        *err = 1;
        return res;
    }

    res.number = args[0].number / PI().number * 180;
    return res;
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
    {"print", print, -1, "print(val1, val2, ...): prints values"},
    {"quit", quit, 0, "quit(): quits the program"},
    {"help", help, 0, "help(): shows this screen"},
    {"printvar", printvar, 0, "printvar(): prints all the variables"},
    {"cls", clear_screen, 0, "cls(): clears the screen"},
    {"sin", builtin_sin, 1, "sin(num): sine of angle"},
    {"cos", builtin_cos, 1, "cos(num): cosine of angle"},
    {"tan", builtin_tan, 1, "tan(num): tangent of angle"},
    {"asin", builtin_asin, 1, "asin(num): inverse sine"},
    {"acos", builtin_acos, 1, "acos(num): inverse cosine"},
    {"atan", builtin_atan, 1, "atan(num): inverse tangent"},
    {"rad2deg", rad_to_deg, 1, "rad2deg(rad): convert radians to degrees"},
    {"deg2rad", deg_to_rad, 1, "deg2rad(deg): convert degrees to radians"},
    {"sinh", builtin_sinh, 1, "sinh(num): hyperbolic sine"},
    {"cosh", builtin_cosh, 1, "cosh(num): hyperbolic cosine"},
    {"tanh", builtin_tanh, 1, "tanh(num): hyperbolic tangent"},
    {"exp", builtin_exp, 1, "exp(num): exponential function e^x"},
    {"log", builtin_log, 1, "log(num): natural logarithm (base e)"},
    {"log10", builtin_log10, 1, "log10(num): logarithm base 10"},
    {"abs", builtin_fabs, 1, "abs(num): absolute value"},
    {"floor", builtin_floor, 1, "floor(num): round down to nearest integer"},
    {"ceil", builtin_ceil, 1, "ceil(num): round up to nearest integer"},
    {"round", builtin_round, 1, "round(num): round to nearest integer"},
    {"fmod", builtin_fmod, 2, "fmod(x, y): remainder of division x mod y"},
    {"sqrt", builtin_sqrt, 1, "sqrt(num): square root"},
    {"pow", builtin_pow, 2, "pow(x, y): raise x to power y"},
    {"pi", PI, 0, "pi(): mathematical constant 3.14159"},
    {"zero", zero_mat, 2, "zero(rows, cols): create a zero matrix"},
    {"ones", ones_mat, 2, "one(rows,cols): create an one matrix"},
    {"ident", ident_mat, 1, "ident(dim): create an identity matrix"},
    {"det", det_mat, 1, "det(mat): determinant of a matrix"},
    {"inv", inv_mat, 1, "inv(mat): inverse of a matrix"},
    {NULL, NULL, 0, NULL} // sentinel
};

Value clear_screen(Value* args, int n, int* err)
{
    (void) args;
    (void) n;
    (void) err;
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    Value res = {.type = VAL_EMP};
    return res;
}

Value print(Value *args, int n, int *err)
{
    Value res;
    res.type = VAL_EMP;
    char *end = " ";

    for (int i = 0; i < n; i++)
    {
        print_value(args[i], end, err);
    }
    printf("\n");
    return res;
}

Value quit(Value *args, int n, int *err)
{
    (void) args;
    (void) n;
    (void) err;
    printf("Exiting Hoscript.\n");
    exit(0);

    Value res = {.type = VAL_EMP};
    return res;
}

Value help(Value *args, int n, int *err)
{
    (void) args;
    (void) n;
    (void) err;

    printf("Available functions:\n");
    for (int i = 0; builtins[i].name != NULL; i++)
    {
        printf("  %-8s (%d args) - %s\n", builtins[i].name, builtins[i].arg_size,
               builtins[i].description);
    }

    printf("\nOperators: +  -  *  /  ^  =\n\n");

    printf("Matrix syntax:\n");
    printf("  [a,b,c;d,e,f;g,h,i]\n");
    printf("  Example: [1,2,3;4,5,6;7,8,9]\n");
    printf("  Use commas for columns and semicolons for new rows.\n");
    Value res = {.type = VAL_EMP};
    return res;
}

Value printvar(Value *args, int n, int *err)
{
    (void) args;
    (void) n;
    (void) err;
    Value res = {.type = VAL_EMP};

    if (current_env == NULL)
    {
        printf("No variables.\n");
        return res;
    }
    if (current_env->var_count > 0)
    {
        for (int i = 0; i < current_env->var_count; i++)
        {
            printf("%s = ", current_env->var[i].name);
            print_value(current_env->var[i].value, "\n", err);
        }
    }
    else
    {
        printf("No variables defined.\n");
    }
    return res;
}

void eval_program(ASTNode* node, Env *env, int *err)
{
    for (int i = 0; i < node->program.count; i++)
    {
        eval(node->program.nodes[i], env, err);
    }
}

Value eval_assign(ASTNode *node, Env *env, int *err)
{
    Value rhs = eval(node->assign.expr, env, err);

    if (node->assign.left->type == AST_IDENT)
    {
        set_var(node->assign.left->name, rhs, env, err);
        return rhs;
    }

    else if (node->assign.left->type == AST_INDEX)
    {
        assign_to_index(node->assign.left, rhs, env, err);
        return rhs;
    }

    fprintf(stderr, "Invalid assignment target.\n");
    *err = 1;
    return EMPTY;
}

Value eval_addition(ASTNode *node, Env *env, int *err)
{
    Value a, b, res;
    a = eval(node->binary.left, env, err);
    if (*err)
        return EMPTY;
    b = eval(node->binary.right, env, err);
    if (*err)
        return EMPTY;

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
        if (*err)
            return EMPTY;
        return res;
    default:
        fprintf(stderr, "Value Error: value does not support addition\n");
        *err = 1;
        return EMPTY;
    }
}

Value eval_subtraction(ASTNode *node, Env *env, int *err)
{
    Value a, b, res;
    a = eval(node->binary.left, env, err);
    if (*err)
        return EMPTY;
    b = eval(node->binary.right, env, err);
    if (*err)
        return EMPTY;

    if (a.type != b.type)
    {
        fprintf(stderr, "Value Error: value does not support subtraction\n");
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
        if (*err)
            return EMPTY;
        return res;
    default:
        fprintf(stderr, "Value Error: value is empty\n");
        *err = 1;
        return EMPTY;
    }
}

Value eval_multiplication(ASTNode *node, Env *env, int *err)
{
    Value a, b, res;
    a = eval(node->binary.left, env, err);
    if (*err)
        return EMPTY;
    b = eval(node->binary.right, env, err);
    if (*err)
        return EMPTY;

    if ((a.type == VAL_MAT && b.type == VAL_NUM) || (b.type == VAL_MAT && a.type == VAL_NUM))
    {
        // scalar * matrix multiplication
        if (a.type == VAL_MAT)
        {
            return scalar_mat(a, b, err);
        }
        else
        {
            return scalar_mat(b, a, err);
        }
    }
    if (a.type != b.type) {
        fprintf(stderr, "Value Error: incompatible value types to perform multiplication\n");
        *err = 1;
        return EMPTY;
    }
    switch (a.type)
    {
    case VAL_NUM:
        res.type = VAL_NUM;
        res.number = a.number * b.number;
        return res;
    case VAL_MAT:
        res = mul_mat(a, b, err);
        if (*err)
            return EMPTY;
        return res;
    default:
        fprintf(stderr, "Value Error: value does not support multiplication\n");
        *err = 1;
        return EMPTY;
    }
}

Value eval_division(ASTNode *node, Env *env, int *err)
{
    Value a, b, res;
    a = eval(node->binary.left, env, err);
    if (*err)
        return EMPTY;
    b = eval(node->binary.right, env, err);
    if (*err)
        return EMPTY;

    if (a.type == VAL_MAT)
    {
        fprintf(stderr, "Value Error: cannot perform division on matrices\n");
        *err = 1;
        return EMPTY;
    }
    if (a.type != b.type) {
        fprintf(stderr, "Value Error: incompatible value types to perform division\n");
        *err = 1;
        return EMPTY;
    }
    switch (a.type) {
        case VAL_NUM:
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
        default:
            fprintf(stderr, "Value Error: value does not support division\n");
            *err = 1;
            return EMPTY;
    }
}

Value eval_exponentiation(ASTNode *node, Env *env, int *err)
{
    Value a, b, res;
    a = eval(node->binary.left, env, err);
    if (*err)
        return EMPTY;
    b = eval(node->binary.right, env, err);
    if (*err)
        return EMPTY;

    switch (a.type)
    {
    case VAL_NUM:
        res.type = VAL_NUM;
        res.number = pow(a.number, b.number);
        return res;
    case VAL_MAT:
        res = pow_mat(a, b, err);
        if (*err)
            return EMPTY;
        return res;
    default:
        fprintf(stderr, "Value Error: value does not support exponentiation\n");
        *err = 1;
        return EMPTY;
    }
}

Value eval_binop(ASTNode *node, Env *env, int *err)
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

Value eval_func(ASTNode *node, Env *env, int *err)
{
    Value *args = malloc(sizeof(Value) * node->func.arg_size);
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
    if (*err)
        return EMPTY;
    for (int i = 0; i < node->func.arg_size; i++)
    {
        free_value(args[i]);
    }
    free(args);
    return a;
}

Value eval_unop(ASTNode *node, Env *env, int *err)
{
    Value val = eval(node->unary.right, env, err);
    if (*err) return EMPTY;

    if (node->unary.op == TOK_MINUS)
    {
        if (val.type == VAL_NUM) {
            val.number = -val.number;
	}
	else if (val.type == VAL_MAT) { 
	    Value scalar = {.type = VAL_NUM, .number = -1};
	    val = scalar_mat(val, scalar, err);
	}
        else {
            fprintf(stderr, "Type Error: unary minus only supports numbers\n");
	}
    }

    return val;
}

Value eval_mat(ASTNode *node, Env *env, int *err)
{
    // start evaluating matrices
    Value a, res;
    ASTNode *row;
    int rows = node->mat.rows;
    int cols = node->mat.matrix[0]->row.cols;
    res.type = VAL_MAT;
    res.mat.rows = rows;
    res.mat.cols = cols;
    res.mat.data = alloc_mat(rows, cols);
    if (!res.mat.data)
    {
        fprintf(stderr, "Memory Error: allocation failed\n");
        *err = 1;
        return EMPTY;
    }

    for (int i = 0; i < rows; i++)
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

Value eval_slice(ASTNode* node, Env* env, int* err)
{
    Value res = {.type = VAL_EMP};
    Value left = eval(node->binary.left, env, err);
    ASTNode* slice_node = node->binary.right;
    Value start_val = eval(slice_node->binary.left, env, err);
    Value end_val = eval(slice_node->binary.right, env, err);
    if (start_val.type != VAL_NUM || end_val.type != VAL_NUM) {
        fprintf(stderr, "Type Error: invalid index type.\n");
        *err = 1;
        return EMPTY;
    }

    // iterator check
    if (!is_iterator(left)) {
        fprintf(stderr, "Type Error: value is not an iterator.\n");
        *err = 1;
        return EMPTY;
    }

    int n;
    if (left.type == VAL_MAT) {
        if (left.mat.rows == 1)
            n = left.mat.cols;
        else 
            n = left.mat.rows;
    }
    else if (left.type == VAL_STR) {
        n = strlen(left.string);
    }

    // bounds check
    if (start_val.number < 0 || start_val.number >= n || end_val.number < 0 || end_val.number >= n) {
        fprintf(stderr, "Index Error: index out of bounds\n");
        *err = 1;
        return EMPTY;
    }

    int size = start_val.number > end_val.number ? 0 : (end_val.number - start_val.number);

    if (left.type == VAL_STR) {
        if (size == 0) {
            res.type = VAL_STR;
            res.string = malloc(1);
            res.string = "";
            return res;
        }
        char* slice_string = malloc(size + 1);
        strncpy(slice_string, left.string + (size_t)start_val.number, (size_t)end_val.number - (size_t)start_val.number);
        slice_string[(int)(end_val.number - start_val.number)] = '\0';
        res.type = VAL_STR;
        res.string = slice_string;
    }
    return res;
}

Value eval_index(ASTNode* node, Env* env, int* err)
{
    if (node->binary.right->type == AST_SLICE) {
        return eval_slice(node, env, err);
    }
    Value left = eval(node->binary.left, env, err);
    Value index = eval(node->binary.right, env, err);
    if (*err) return EMPTY;

    if (index.type != VAL_NUM) 
    {
        fprintf(stderr, "TypeError: invalid index type.\n");
        *err = 1;
        return EMPTY;
    }

    int i = (int)index.number;
    Value v = EMPTY;
    char buffer;

    int nested_as_inner = (node->parent && node->parent->type == AST_INDEX && node->parent->binary.left == node);
    int nested_as_outer = (node->binary.left && node->binary.left->type == AST_INDEX);

    switch (left.type) {
        case VAL_MAT:
            if (left.mat.rows == 1) 
            {
                if (nested_as_inner) 
                {
                    if (i != 0) 
                    {
                        fprintf(stderr, "Index Error: row index out of range for 1x%d vector.\n", left.mat.cols);
                        *err = 1;
                        return EMPTY;
                    }
                    return left;
                }

                if (i < 0 || i >= left.mat.cols) 
                {
                    fprintf(stderr, "Index Error: column index out of range for 1x%d vector.\n", left.mat.cols);
                    *err = 1;
                    return EMPTY;
                }
                v.type = VAL_NUM;
                v.number = left.mat.data[0][i];
                return v;
            }

            if (nested_as_outer) 
            {
                if (left.mat.rows != 1) 
                {
                    fprintf(stderr, "TypeError: nested indexing expects a 1xN row.\n");
                    *err = 1;
                    return EMPTY;
                }
                if (i < 0 || i >= left.mat.cols) 
                {
                    fprintf(stderr, "Index Error: column index out of range.\n");
                    *err = 1;
                    return EMPTY;
                }
                v.type = VAL_NUM;
                v.number = left.mat.data[0][i];
                return v;
            }

            if (i < 0 || i >= left.mat.rows) 
            {
                fprintf(stderr, "Index Error: row index out of range for %dx%d matrix.\n", left.mat.rows, left.mat.cols);
                *err = 1;
                return EMPTY;
            }

            v.type = VAL_MAT;
            v.mat.rows = 1;
            v.mat.cols = left.mat.cols;
            v.mat.data = malloc(sizeof(double*));
            v.mat.data[0] = malloc(sizeof(double) * v.mat.cols);
            for (int c = 0; c < v.mat.cols; ++c)
                v.mat.data[0][c] = left.mat.data[i][c];

            return v;
            break;
        case VAL_STR:
            if (strlen(left.string) <= index.number || index.number < 0) {
                fprintf(stderr, "Index Error: index out of bounds\n");
                *err = 1;
                return EMPTY;
            }
            buffer = left.string[(int)index.number];
            v.type = VAL_STR;
            v.string = malloc(2*sizeof(char));
            v.string[0] = buffer;
            v.string[1] = '\0';
            return v;
        default:
            fprintf(stderr, "TypeError: invalid indexing target.\n");
            *err = 1;
            return EMPTY;
    }
}


void assign_to_index(ASTNode* node, Value rhs, Env* env, int* err)
{
    Value index = eval(node->binary.right, env, err); // At the right side, binary.right should be a double type index at this point but its still havent evaluate that it is column or row
    if (index.type != VAL_NUM)
    {
        fprintf(stderr, "Index must be numeric.\n");
        *err = 1;
        return;
    }
    int j = (int)index.number; // make sure its integer instead of double

    if (node->binary.left->type == AST_INDEX) // At the left side is another A[i] , so we have something like A[i][j] , then we can consider i = rows , j = cols in this condition
    {
        ASTNode* left_index = node->binary.left;
        Value row_index = eval(left_index->binary.right, env, err); // can look as eval(node->binary.left->binary.right, ...)
        if (row_index.type != VAL_NUM)
        {
            fprintf(stderr, "Index must be numeric.\n");
            *err = 1;
            return;
        }
        int i = (int)row_index.number; // same, make sure its integer

        // base variable
        ASTNode* ident_node = left_index->binary.left; // Go to the bottom left of tree to get the AST_IDENT
        Value mat = get_var(ident_node->name, env, err); // Get the actual matrix variable "A"

        if (mat.type != VAL_MAT)
        {
            fprintf(stderr, "Cannot index non-matrix.\n");
            *err = 1;
            return;
        }

        if (i < 0 || i >= mat.mat.rows || j < 0 || j >= mat.mat.cols) // struct { int rows; int cols; double** data; }mat
        {
            fprintf(stderr, "Index Error: index out of range.\n");
            *err = 1;
            return;
        }

        mat.mat.data[i][j] = rhs.number;  // assign desired number to the particular position
        set_var(ident_node->name, mat, env, err); // update environment
    }
    else if (node->binary.left->type == AST_IDENT) // At the left side, we directly get only IDENT(A) , so it is only A[j] (j = cols)
    {
        Value mat = get_var(node->binary.left->name, env, err);
        if (mat.type != VAL_MAT)
        {
            fprintf(stderr, "Cannot index non-matrix.\n");
            *err = 1;
            return;
        }

        if (j < 0 || j >= mat.mat.cols)
        {
            fprintf(stderr, "Index Error: index out of range.\n");
            *err = 1;
            return;
        }

        mat.mat.data[0][j] = rhs.number; // only access the first row and assign the value of specific column , doesnt matter if the row is more than 1
        set_var(node->binary.left->name, mat, env, err);
    }
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
    case AST_STRING:
	a.type = VAL_STR;
	a.string = strdup(node->name);
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
    case AST_INDEX:
        return eval_index(node, env, err);
    case AST_PROGRAM:
        if (node->program.count == 0) 
            return EMPTY;
        return eval(node->program.nodes[0], env, err);

    default:
        return EMPTY;
        break;
    }
}

Value get_var(const char *name, Env *env, int *err)
{
    for (int i = 0; i < env->var_count; i++)
    {
        if (strcmp(env->var[i].name, name) == 0) // if equal strcmp returns 0
        {
            *err = 0;
            return copy_value(env->var[i].value, err);
        }
    }
    fprintf(stderr, "Variable error: variable %s not found\n", name);
    *err = 1;
    return EMPTY;
}

void set_var(const char *name, Value value, Env *env, int *err)
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

Value calc_func(const char *name, Value *args, int arg_size, int *err)
{
    // compare name with all function name
    for (int i = 0; builtins[i].name != NULL; i++)
    {
        if (strcmp(builtins[i].name, name) == 0) // 0 is equal
        {
            if (builtins[i].arg_size == -1)
            {
                Value res = builtins[i].func(args, arg_size, err);
                if (*err)
                    return EMPTY;
                return res;
            }
            if (arg_size != builtins[i].arg_size)
            {
                fprintf(stderr, "Function %s expects %d args, got %d\n", name, builtins[i].arg_size,
                        arg_size);
                *err = 1;
                return EMPTY;
            }
            else
            {
                Value res = builtins[i].func(args, arg_size, err);
                if (*err)
                    return EMPTY;
                return res;
            }
        }
    }
    // name not in the list of built in functions, print error
    fprintf(stderr, "Function %s not defined\n", name);
    *err = 1;
    return EMPTY;
}

void interpret(const char *input, Env *env, int is_file, int *err)
{
    int tok_count;
    if (input[0] == '\0')
    {
        return;
    }

    Token *tokens = generate_tokens(input, &tok_count, err);
    if (*err)
        return;

    Parser parser = {tokens, tok_count, 0, tokens[0]};
    ASTNode *root = parse(&parser, err);
    if (*err)
    {
        free_tokens(tokens, tok_count);
        return;
    }

    if (parser.current_token.type != TOK_EOF)
    {
	print_token(parser.current_token);
        fprintf(stderr, "Syntax Error: extra tokens at end of input\n");
        free_tokens(tokens, tok_count);
        free_ast(root);
        *err = 1;
        return;
    }

    if (is_file) {
        eval_program(root, env, err);
        if (*err) {
            free_tokens(tokens, tok_count);
            free_ast(root);
            return;
        }
    }
    else {
        Value result = eval(root, env, err);
        if (*err)
        {
            free_tokens(tokens, tok_count);
            free_ast(root);
            return;
        }

        if (!*err && tok_count != 1)
        {
            if (root->program.nodes[0]->type != AST_ASSIGN)
            {
                if (!is_file)
                {
                    print_value(result, "\n", err);
                }
                free_value(result);
            }
        }
    }

    free_tokens(tokens, tok_count);
    free_ast(root);
}

void start_repl()
{
    Variable vars[VAR_MAX];
    Env env = {vars, 0};
    char *input;

    const char *history_file = ".interpreter_history";
    read_history(history_file);

    while (1)
    {
        int err = 0;

        input = readline("> ");
        if (!input)
            break;

        add_history(input);

        interpret(input, &env, NOT_FILE, &err);

        free(input);
    }
    write_history(history_file);

    printf("\nExiting Hoscript.\n");
}

void run_file(const char *filename, int *err)
{
    char *source = read_file(filename);
    if (*err || !source)
    {
        fprintf(stderr, "Error: could not read file '%s'\n", filename);
        return;
    }

    Variable vars[VAR_MAX];
    Env env = {vars, 0};

    interpret(source, &env, IS_FILE, err);

    free(source);
}
