#include <stdio.h>   // printf, scanf
#include <stdlib.h>  // malloc, free, atof, exit
#include <string.h>  // strcmp, strdup, strlen
#include <ctype.h>   // isdigit, isalpha, isspace
#include <math.h>

#include "lexer.h"
#include "parser.h"
#include "eval.h"

#define VAR_MAX 100

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

double eval(ASTNode* root, Env* env);
double get_var(const char* name, Env* env);
void set_var(const char* name, double value, Env* env);

double eval(ASTNode* node, Env* env)
{
    double a;
    double b;

    switch (node->type)
    {
    case AST_ASSIGN:
        a = eval(node->assign.expr, env);
        set_var(node->assign.name, a, env);
        printf("%s = ", node->assign.name);
        return a;
    case AST_NUM:
        return node->value;
    case AST_BINOP:
        a = eval(node->binary.left, env);
        b = eval(node->binary.right, env);
        switch (node->binary.op)
        {
        case TOK_PLUS:
            return a+b;
        case TOK_MINUS:
            return a-b;
        case TOK_MUL:
            return a*b;
        case TOK_DIV:
            return a/b;
        case TOK_POW:
            return pow(a,b);
        default:
            return 0;
        }
    case AST_IDENT:
        return get_var(node->name, env);
    default:
        return 0;
        break;
    }
}

double get_var(const char* name, Env* env)
{
    for (int i = 0; i < env->var_count; i++)
    {
        if (strcmp(env->var[i].name, name) == 0) // if equal strcmp returns 0
        {
            return env->var[i].value;
        }
    }
    fprintf(stderr, "Variable error: variable %s not found\n", name);
    exit(1);
}

void set_var(const char* name, double value, Env* env)
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
        exit(1);
    }
    
    // variable not found, create a new one and increment var_count
    env->var[env->var_count].name = strdup(name); // need to free
    env->var[env->var_count].value = value;
    (env->var_count)++;
}

int main()
{
    char input[256];
    int tok_count;

    Variable vars[VAR_MAX];
    Env env = {vars, 0};

    while (1)
    {
        printf("> ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';
        if (strcmp(input, "quit()") == 0)
        {
            printf("\n");
            break;
        }
        Token* tokens = generate_tokens(input, &tok_count); 
        Parser parser = {tokens, tok_count, 0, tokens[0]};
        ASTNode* root = parse(&parser);
        if (parser.current_token.type != TOK_EOF) {
            printf("Syntax error: extra tokens at end of input\n");
        }

        printf("%.2lf\n", eval(root, &env));
    }
}