#include <stdio.h>   // printf, scanf
#include <stdlib.h>  // malloc, free, atof, exit
#include <string.h>  // strcmp, strdup, strlen
#include <ctype.h>   // isdigit, isalpha, isspace
#include <math.h>

#include "eval.h"

#define VAR_MAX 100

#define DEF_UNARY(name) \
Value builtin_##name(Value *args, int n) { \
    Value res; \
    res.type = VAL_NUM; \
    if (n != 1 || args[0].type != VAL_NUM) { \
        fprintf(stderr, #name " expects 1 numeric arg\n"); \
        res.number = NAN; \
        return res; \
    } \
    res.number = name(args[0].number); \
    return res; \
}

#define DEF_BINARY(name) \
Value builtin_##name(Value *args, int n) { \
    Value res; \
    res.type = VAL_NUM; \
    if (n != 2 || args[0].type != VAL_NUM || args[1].type != VAL_NUM) { \
        fprintf(stderr, #name " expects 2 numeric args\n"); \
        res.number = NAN; \
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

Value deg_to_rad(Value* args, int n)
{
    Value res;
    res.type = VAL_NUM;

    if (n != 1 || args[0].type != VAL_NUM)
    {
        fprintf(stderr, "deg2rad expects 1 numeric arg\n");
        return res;
    }
    
    res.number = args[0].number/180*PI().number;
    return res;
}

Value rad_to_deg(Value* args, int n)
{
    Value res;
    res.type = VAL_NUM;

    if (n != 1 || args[0].type != VAL_NUM)
    {
        fprintf(stderr, "rad2deg expects 1 numeric arg\n");
        return res;
    }

    res.number = args[0].number/PI().number*180;
    return res;
}

Value zero(Value* args, int n)
{
    Value res;
    res.type = VAL_MAT;

    // 2 input
    if (n != 2 || args[0].type != VAL_NUM || args[1].type != VAL_NUM)
    {
        fprintf(stderr, "zero expects 2 numeric arg\n");
        return res;
    }
    int rows = args[0].number;
    int cols = args[1].number;

    int err = 0;

    return zero_mat(rows, cols, &err);
}

Value ident(Value* args, int n)
{
    Value res;
    res.type = VAL_MAT;

    if (n != 1 || args[0].type != VAL_NUM)
    {
        fprintf(stderr, "ident expects 1 numeric arg\n");
        return res;
    }

    int err = 0;

    return ident_mat(args[0].number, &err);
}

Value det(Value* args, int n)
{
    Value res;
    res.type = VAL_MAT;

    if (n != 1 || args[0].type != VAL_MAT)
    {
        fprintf(stderr, "det expects 1 matrix arg\n");
        return res;
    }

    int err = 0;

    return det_mat(args[0], &err);
}

Value inv(Value* args, int n)
{
    Value res;
    res.type = VAL_MAT;

    if (n != 1 || args[0].type != VAL_MAT)
    {
        fprintf(stderr, "inv expects 1 matrix arg\n");
        return res;
    }

    int err = 0;

    return inv_mat(args[0], &err);
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
    {"sin",     builtin_sin,            1, "Sine of angle (radians)"},
    {"cos",     builtin_cos,            1, "Cosine of angle (radians)"},
    {"tan",     builtin_tan,            1, "Tangent of angle (radians)"},
    {"asin",    builtin_asin,           1, "Inverse sine (result in radians)"},
    {"acos",    builtin_acos,           1, "Inverse cosine (result in radians)"},
    {"atan",    builtin_atan,           1, "Inverse tangent (result in radians)"},
    {"rad2deg", rad_to_deg,     1, "Convert Radians to Degree"},
    {"deg2rad", deg_to_rad,     1, "Convert Degree to Radians"},
    {"sinh",    builtin_sinh,           1, "Hyperbolic sine"},
    {"cosh",    builtin_cosh,           1, "Hyperbolic cosine"},
    {"tanh",    builtin_tanh,           1, "Hyperbolic tangent"},
    {"exp",     builtin_exp,            1, "Exponential function (e^x)"},
    {"log",     builtin_log,            1, "Natural logarithm (base e)"},
    {"log10",   builtin_log10,          1, "Logarithm base 10"},
    {"abs",     builtin_fabs,            1, "Absolute value"},
    {"floor",   builtin_floor,          1, "Round down to nearest integer"},
    {"ceil",    builtin_ceil,           1, "Round up to nearest integer"},
    {"round",   builtin_round,          1, "Round to nearest integer"},
    {"fmod",    builtin_fmod,           2, "Remainder of division (x mod y)"},
    {"sqrt",    builtin_sqrt,           1, "Square root"},
    {"pow",     builtin_pow,            2, "Raise to power (x^y)"},
    {"pi",      PI,                     0, "Mathematical constant 3.14159..."},
    {"zero",    zero,                   2, "Raise to power (x^y)"},
    {"ident",   ident,                  1, "Raise to power (x^y)"},
    {"det",     det,                    1, "Raise to power (x^y)"},
    {"inv",     inv,                    1, "Raise to power (x^y)"},
    {NULL,      NULL, 0, NULL} // sentinel
};

Value copy_value(Value val);
void print_value(Value val, int* err);
Value eval(ASTNode* root, Env* env, int* err);
Value get_var(const char* name, Env* env, int* err);
void set_var(const char* name, Value value, Env* env, int* err);
Value eval_func(const char* name, Value* args, int arg_size, int* err);

const Value EMPTY;

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
        printf("%lf\n", val.number);
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

Value eval(ASTNode* node, Env* env, int* err)
{
    int rows, cols;
    ASTNode* row;
    Value a, b, res;
    Value* args;

    switch (node->type)
    {
    case AST_ASSIGN:
        a = eval(node->assign.expr, env, err);
        if (*err) return EMPTY;
        set_var(node->assign.name, a, env, err);
        if (*err) return EMPTY;
        printf("%s = ", node->assign.name);
        if (a.type == VAL_MAT)
            printf("\n");
        return a;
    case AST_NUM:
        a.type = VAL_NUM;
        a.number = node->value;
        return a;
    case AST_BINOP:
        a = eval(node->binary.left, env, err);
        if (*err) return EMPTY;
        b = eval(node->binary.right, env, err);
        if (*err) return EMPTY;
        
        switch (node->binary.op)
        {
        case TOK_PLUS:
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
        case TOK_MINUS:
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
        case TOK_MUL:
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
        case TOK_DIV:
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
        case TOK_POW:
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
        default:
            return EMPTY;
        }
    case AST_IDENT:
        return get_var(node->name, env, err);
    case AST_FUNC:
        args = malloc(sizeof(Value) * node->func.arg_size);
        for (int i = 0; i < node->func.arg_size; i++)
        {
            args[i] = eval(node->func.args[i], env, err);
            if (*err) 
            {
                free(args);
                return EMPTY;
            }
        }
        a = eval_func(node->func.name, args, node->func.arg_size, err);
        if (*err) return EMPTY;    
        for (int i = 0; i < node->func.arg_size; i++)
        {
            free_value(args[i]);
        }
        free(args);
        return a;
    case AST_UNOP:
        a = eval(node->binary.right, env, err);

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
    case AST_MAT:
        // start evaluating matrices
        rows = node->mat.rows;
        cols = node->mat.matrix[0]->row.cols;
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
                    // nEED TO BE NUMBER PLEASE
                }
                
                res.mat.data[i][j] = a.number;
            }
        }
        return res;
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

Value eval_func(const char* name, Value* args, int arg_size, int* err)
{
    // compare name with all function name
    for (int i = 0; builtins[i].name != NULL; i++)
    {
        if (strcmp(builtins[i].name, name) == 0) // 0 is equal
        {
            if (arg_size != builtins[i].arg_size)
            {
                fprintf(stderr, "Function %s expects %d args, got %d\n", name, builtins[i].arg_size, arg_size);
                if (err) *err = 1;
                return EMPTY;
            }
            else
            {   
                Value res = builtins[i].func(args, arg_size);
                return res;
            }
        }
    }
    // name not in the list of built in functions, print error
    fprintf(stderr, "Function %s not defined\n", name);
    *err = 1;
    return EMPTY;
}

void print_help() {
    printf("Available commands:\n");
    printf("  quit   Exit the program\n");
    printf("  help   Show this message\n\n");

    printf("Available functions:\n");
    for (int i = 0; builtins[i].name != NULL; i++) {
        printf("  %-8s (%d args) - %s\n", 
               builtins[i].name, 
               builtins[i].arg_size, 
               builtins[i].description);
    }

    printf("\nOperators: +  -  *  /  ^  =\n");
}


int main()
{
    char input[256];
    int tok_count;

    Variable vars[VAR_MAX];
    Env env = {vars, 0};

    while (1)
    {
        int err = 0;
        printf("> ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';
        if (strcmp(input, "quit") == 0)
        {
            printf("\n");
            break;
        }
        else if (strcmp(input, "help") == 0)
        {
            print_help();
            continue;
        }

        Token* tokens = generate_tokens(input, &tok_count, &err); 
        if (err) continue;


        Parser parser = {tokens, tok_count, 0, tokens[0]};
        
        ASTNode* root = parse(&parser, &err);
        if (err) continue;

        if (parser.current_token.type != TOK_EOF) {
            fprintf(stderr, "Syntax error: extra tokens at end of input\n");
            continue;
        }
        
        Value result = eval(root, &env, &err);
        if (!err && tok_count != 1)
        {   
            print_value(result, &err);

            if (root->type != AST_ASSIGN)
            {
                free_value(result);
            }
        }

        // at the end of a line, free the tokens and ast nodes
        free_tokens(tokens, tok_count);
        free_ast(root);
    }
}