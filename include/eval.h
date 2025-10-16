#ifndef EVAL_H
#define EVAL_H

#include "lexer.h"
#include "parser.h"
#include "mat.h"

typedef struct
{
    char* name;
    Value value;
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

typedef Value (*BuiltinFunc)(Value *args, int arg_size, int* err);

typedef struct
{
    const char* name;
    BuiltinFunc func;
    int arg_size;
    const char* description;
} BuiltinEntry;

Value eval(ASTNode* node, Env* env, int* err);
void print_help();
void print_value(Value val, int* err);

#endif
