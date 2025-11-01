#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"  

typedef struct ASTNode ASTNode;

typedef struct ASTNode ASTNode;

typedef enum
{
    AST_NUM,
    AST_BINOP,
    AST_UNOP,
    AST_IDENT,
    AST_ASSIGN,
    AST_FUNC,
    AST_LIST,
    AST_INDEX,
    AST_SLICE,
    AST_STRING,
    AST_IF,
    AST_PROGRAM,
    AST_FOR,
    AST_EOF
} ASTNodeType;

typedef struct ASTNode
{
    ASTNodeType type;
    struct ASTNode *parent;
    union
    {
        double value;
        char *name;
        struct
        {
            TokenType op;
            struct ASTNode *right;
        } unary;
        struct
        {
            TokenType op;
            struct ASTNode *left;
            struct ASTNode *right;
        } binary;
        struct
        { // assignment
            ASTNode *left;
            ASTNode *expr;
        } assign;
        struct // function
        {
            char *name;
            struct ASTNode **args;
            int arg_size;
        } func;
        struct
        {
            int max_size;
            int size;
            struct ASTNode **elements;
        } list;
        struct
        {
            struct ASTNode *cond;
            struct ASTNode *if_stmt;
            struct ASTNode *else_stmt;
        } if_node;
        struct
        {
            char *var;
            struct ASTNode *iterator;
            struct ASTNode *body;
        } for_node;
        struct
        {
            struct ASTNode **nodes;
            int count;
        } program;
    };
} ASTNode;

typedef struct
{
    Token *tokens;
    int num_tokens;
    int pos;
    Token current_token;
} Parser;

ASTNode *parse(Parser *parser, int *err);
void print_ast(ASTNode *node, int indent);
void free_ast(ASTNode *node);
#endif
