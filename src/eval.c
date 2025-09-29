#include <stdio.h>   // printf, scanf
#include <stdlib.h>  // malloc, free, atof, exit
#include <string.h>  // strcmp, strdup, strlen
#include <ctype.h>   // isdigit, isalpha, isspace
#include <math.h>

#include "eval.h"

#define VAR_MAX 100

#define DEF_UNARY(name) \
double builtin_##name(double *args, int n) { \
    if (n != 1) { fprintf(stderr, #name " expects 1 arg\n"); return NAN; } \
    return name(args[0]); \
}

#define DEF_BINARY(name) \
double builtin_##name(double *args, int n) { \
    if (n != 2) { fprintf(stderr, #name " expects 2 args\n"); return NAN; } \
    return name(args[0], args[1]); \
}

typedef struct
{
    char* name;
    double value;
} Variable;

typedef struct
{
    Variable* var;
    int var_count;
} Env;

typedef enum
{
    EVAL_OK = 0,
    EVAL_SYNTAX_ERROR,
    EVAL_NUMBER_ERROR,
    EVAL_VARIABLE_ERROR
} EvalStatus;

typedef double (*BuiltinFunc)(double *args, int arg_size);

typedef struct
{
    const char* name;
    BuiltinFunc func;
    int arg_size;
    const char* description;
} BuiltinEntry;

double eval(ASTNode* root, Env* env, int* err);
double get_var(const char* name, Env* env, int* err);
void set_var(const char* name, double value, Env* env, int* err);
double eval_func(const char* name, double* args, int arg_size, int* err);

double PI()
{
    return 3.14159265358979323846;
}

double deg_to_rad(double x)
{
    return x/180*PI();
}

double rad_to_deg(double x)
{
    return x/PI()*180;
}

DEF_UNARY(sin)
DEF_UNARY(cos)
DEF_UNARY(tan)
DEF_UNARY(asin)
DEF_UNARY(acos)
DEF_UNARY(atan)
DEF_UNARY(deg_to_rad)
DEF_UNARY(rad_to_deg)
DEF_UNARY(sinh)
DEF_UNARY(cosh)
DEF_UNARY(tanh)
DEF_UNARY(exp)
DEF_UNARY(log)
DEF_UNARY(log10)
DEF_UNARY(abs)
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
    {"rad2deg", builtin_rad_to_deg,     1, "Convert Radians to Degree"},
    {"deg2rad", builtin_deg_to_rad,     1, "Convert Degree to Radians"},
    {"sinh",    builtin_sinh,           1, "Hyperbolic sine"},
    {"cosh",    builtin_cosh,           1, "Hyperbolic cosine"},
    {"tanh",    builtin_tanh,           1, "Hyperbolic tangent"},
    {"exp",     builtin_exp,            1, "Exponential function (e^x)"},
    {"log",     builtin_log,            1, "Natural logarithm (base e)"},
    {"log10",   builtin_log10,          1, "Logarithm base 10"},
    {"abs",     builtin_abs,            1, "Absolute value"},
    {"floor",   builtin_floor,          1, "Round down to nearest integer"},
    {"ceil",    builtin_ceil,           1, "Round up to nearest integer"},
    {"round",   builtin_round,          1, "Round to nearest integer"},
    {"fmod",    builtin_fmod,           2, "Remainder of division (x mod y)"},
    {"sqrt",    builtin_sqrt,           1, "Square root"},
    {"pow",     builtin_pow,            2, "Raise to power (x^y)"},
    {"pi",      PI,                     0, "Mathematical constant 3.14159..."},
    {NULL,      NULL, 0, NULL} // sentinel
};

double eval(ASTNode* node, Env* env, int* err)
{
    double a;
    double b;
    double* args;

    switch (node->type)
    {
    case AST_ASSIGN:
        a = eval(node->assign.expr, env, err);
        if (*err) return NAN;
        set_var(node->assign.name, a, env, err);
        if (*err) return NAN;
        printf("%s = ", node->assign.name);
        return a;
    case AST_NUM:
        return node->value;
    case AST_BINOP:
        a = eval(node->binary.left, env, err);
        if (*err) return NAN;
        b = eval(node->binary.right, env, err);
        if (*err) return NAN;
        switch (node->binary.op)
        {
        case TOK_PLUS:
            return a+b;
        case TOK_MINUS:
            return a-b;
        case TOK_MUL:
            return a*b;
        case TOK_DIV:
            return b != 0 ? a/b : (fprintf(stderr,"Division by zero\n"), *err=1, NAN);
        case TOK_POW:
            return pow(a,b);
        default:
            return 0;
        }
    case AST_IDENT:
        return get_var(node->name, env, err);
    case AST_FUNC:
        args = malloc(sizeof(double) * node->func.arg_size);
        for (int i = 0; i < node->func.arg_size; i++)
        {
            args[i] = eval(node->func.args[i], env, err);
            if (*err) 
            {
                free(args);
                return NAN;
            }
        }
        a = eval_func(node->func.name, args, node->func.arg_size, err);
        free(args);
        return a;
    default:
        return 0;
        break;
    }
}

double get_var(const char* name, Env* env, int* err)
{
    for (int i = 0; i < env->var_count; i++)
    {
        if (strcmp(env->var[i].name, name) == 0) // if equal strcmp returns 0
        {
            *err = 0;
            return env->var[i].value;
        }
    }
    fprintf(stderr, "Variable error: variable %s not found\n", name);
    *err = 1;
    return NAN;
}

void set_var(const char* name, double value, Env* env, int* err)
{
    for (int i = 0; i < env->var_count; i++) // check reassignment
    {
        if (strcmp(env->var[i].name, name) == 0) // if equal strcmp returns 0, variable found
        {
            env->var[i].value = value;
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

double eval_func(const char* name, double* args, int arg_size, int* err)
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
                return NAN;
            }
            else
            {
                return builtins[i].func(args, arg_size);
            }
        }
    }
    // name not in the list of built in functions, print error
    fprintf(stderr, "Function %s not defined\n", name);
    *err = 1;
    return NAN;
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
        
        double result = eval(root, &env, &err);
        if (!err)
        {
            printf("%lf\n", result);
        }

        // at the end of a line, free the tokens and ast nodes
        free_tokens(tokens, tok_count);
        free_ast(root);
    }
}