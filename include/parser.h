#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"  

typedef struct ASTNode ASTNode;

typedef enum
{
    AST_NUM,
    AST_BINOP,
    AST_UNOP,
    AST_IDENT,
    AST_ASSIGN,
    AST_FUNC,
    AST_ROW,
    AST_MAT,
    AST_INDEX
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    struct ASTNode* parent;
    union
    {
        double value;
        char* name;
        struct {
            TokenType op;
            struct ASTNode* right;
        } unary;
        struct
        {
            TokenType op;
            struct ASTNode* left;
            struct ASTNode* right;
        } binary;
        struct {         // assignment
            ASTNode* left;
            ASTNode* expr;
        } assign;
        struct          // function
        {
            char* name;
            struct ASTNode** args;
            int arg_size;
        } func;
        struct
        {
            int row_cap;
            int rows;
            struct ASTNode** matrix;
        } mat;
        struct
        {
            struct ASTNode** elements;
            int cols;
            int col_cap;
        } row; 
    };
} ASTNode;

typedef struct
{
    Token* tokens;
    int num_tokens;
    int pos;
    Token current_token;
} Parser;

ASTNode* parse(Parser* parser, int* err);
void print_ast(ASTNode* node, int indent);
void free_ast(ASTNode* node);
#endif