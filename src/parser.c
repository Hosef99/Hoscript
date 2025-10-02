#include <stdio.h>   // printf, scanf
#include <stdlib.h>  // malloc, free, atof, exit
#include <string.h>  // strcmp, strdup, strlen
#include <ctype.h>   // isdigit, isalpha, isspace
// #include "parser.h"

#include "parser.h"

void next_token(Parser* parser);
int match_token(Parser* parser, TokenType type, int look_ahead);
ASTNode* parse_assign(Parser* parser, int* err);
ASTNode* parse_expr(Parser* parser, int* err);
ASTNode* parse_term(Parser* parser, int* err);
ASTNode* parse_factor(Parser* parser, int* err);
ASTNode* parse_power(Parser* parser, int* err);
ASTNode* parse_base(Parser* parser, int* err);
ASTNode* parse_func(Parser* parser, int* err);
ASTNode* parse_mat(Parser* parser, int* err);
ASTNode* parse_row(Parser* parser, int* err);
void print_ast(ASTNode* node, int indent);

void next_token(Parser* parser)
{
    parser->current_token = parser->tokens[++(parser->pos)];
}

int match_token(Parser* parser, TokenType type, int look_ahead)
{
    return (parser->tokens[parser->pos+look_ahead].type == type);
}

ASTNode* make_op_node(TokenType op, ASTNode* left, ASTNode* right)
{
    ASTNode* node = malloc(sizeof(ASTNode)); // free all ast node

    node->type = AST_BINOP;
    node->binary.op = op;
    node->binary.left = left;
    node->binary.right = right;
    return node;
}

ASTNode* parse(Parser* parser, int* err)
{
    // need to do some filtering, look ahead for second token 
    if (match_token(parser, TOK_IDENT, 0)) // identifier
    {   
        if (match_token(parser, TOK_ASSIGN, 1)) // look ahead, be careful of spaces
        {
            return parse_assign(parser, err);
        }
    }
    
    return parse_expr(parser, err);
}

ASTNode* parse_assign(Parser* parser, int* err)
{
    ASTNode* node = malloc(sizeof(ASTNode)); // free all ast node

    node->assign.name = strdup(parser->current_token.name); // free assign.name 
    // first token must be ident
    next_token(parser); // IDENT 
    next_token(parser); // '='
    
    ASTNode* rhs = parse_expr(parser, err);

    node->type = AST_ASSIGN;
    node->assign.expr = rhs;
    return node;
}

ASTNode* parse_expr(Parser* parser, int* err)
{
    ASTNode* left = parse_term(parser, err);

    while (match_token(parser, TOK_PLUS, 0) || match_token(parser, TOK_MINUS, 0))
    {
        TokenType op = parser->current_token.type;
        next_token(parser);

        ASTNode* right = parse_term(parser, err);

        ASTNode* new_node = make_op_node(op, left, right);
        left = new_node;
    }
    return left;
}

ASTNode* parse_term(Parser* parser, int* err)
{

    ASTNode* left = parse_factor(parser, err);

    while (match_token(parser, TOK_MUL, 0) || match_token(parser, TOK_DIV, 0))
    {
        TokenType op = parser->current_token.type;
        next_token(parser);

        ASTNode* right = parse_factor(parser, err);

        ASTNode* new_node = make_op_node(op, left, right);
        left = new_node;
    }
    return left;
}

ASTNode* parse_factor(Parser* parser, int* err)
{
    int sign = 1;

    while (match_token(parser, TOK_PLUS, 0) || match_token(parser, TOK_MINUS, 0))
    {
        if (match_token(parser, TOK_MINUS, 0))
        {
            sign = -sign;
        }
        next_token(parser);
    }

    ASTNode* node = parse_power(parser, err);
    
    if (sign == -1)
    {
        ASTNode* unary = malloc(sizeof(ASTNode));
        unary->type = AST_UNOP;
        unary->binary.op = TOK_MINUS;
        unary->binary.right = node;
        node = unary;    
    }

    return node;
}

ASTNode* parse_power(Parser* parser, int* err)
{
    ASTNode* left = parse_base(parser, err);

    if (match_token(parser, TOK_POW, 0))
    {
        TokenType op = TOK_POW;
        next_token(parser);
        
        ASTNode* right = parse_factor(parser, err);
        ASTNode* op_node = make_op_node(op, left, right);
        left = op_node;
    }

    return left;
}

ASTNode* parse_base(Parser* parser, int* err)
{
    ASTNode* new_node = malloc(sizeof(ASTNode)); // need to free every astnode
    if (match_token(parser, TOK_NUM, 0)) // number
    {
        new_node->type = AST_NUM;
        new_node->value = parser->current_token.value;
        next_token(parser);
    }
    else if (match_token(parser, TOK_IDENT, 0)) // if first token identifier
    {
        if (match_token(parser, TOK_LPAREN, 1)) // second token LPAREN
        {
            return parse_func(parser, err);
        }
        else // identifier
        {
            new_node->type = AST_IDENT;
            new_node->name = strdup(parser->current_token.name); // need to free ast.name
            next_token(parser);
        }
    }
    else if (match_token(parser, TOK_LPAREN, 0)) // LPAREN expr RPARAN
    {
        next_token(parser);
        new_node = parse_expr(parser, err);

        if (!match_token(parser, TOK_RPAREN, 0)) // SYNTAX ERROR!!!
        {
            fprintf(stderr, "Syntax error: expected ')'\n");
            *err = 1;
            return NULL;
        }

        next_token(parser);
    }
    else if (match_token(parser, TOK_LBRACK, 0))
    {
        return parse_mat(parser, err);
    }
    return new_node;
}

ASTNode* parse_func(Parser* parser, int* err)
{
    // func -> IDENT LPARAN (expr (COMMA expr)*)? RPARAN

    ASTNode* node = malloc(sizeof(ASTNode)); // need to free all the ast nodes 
    node->type = AST_FUNC;
    node->func.name = strdup(parser->current_token.name); // need to free astnode.func.name
    node->func.arg_size = 0;

    next_token(parser); // consume IDENT

    if (parser->current_token.type != TOK_LPAREN) {
        fprintf(stderr, "Syntax error: expected '('\n");
        *err = 1;
        return NULL;
    }
    next_token(parser); // consume '('

    // arguments
    if (parser->current_token.type != TOK_RPAREN) {
        // allocate space for arguments (say max 16 for now)
        node->func.args = malloc(16 * sizeof(ASTNode*)); // need to free the args in the AST

        node->func.args[node->func.arg_size++] = parse_expr(parser, err);
        while (match_token(parser, TOK_COMMA, 0)) {
            next_token(parser); // consume ','
            node->func.args[node->func.arg_size++] = parse_expr(parser, err);
        }
    }

    if (parser->current_token.type != TOK_RPAREN) {
        fprintf(stderr, "Syntax error: expected ')'\n");
        *err = 1;
        return NULL;
    }
    next_token(parser); // consume ')'

    return node;
}

ASTNode* parse_mat(Parser* parser, int* err)
{
    int row, col;
    // mat -> LBRACK row (SEMICOLON row)* RBRACK
    next_token(parser);
    int row_cap = 4;
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_MAT;
    node->mat.row_cap = row_cap;
    node->mat.matrix = malloc(4 * sizeof(ASTNode*));

    node->mat.rows = 1;
    node->mat.matrix[0] = parse_row(parser, err);

    while (match_token(parser, TOK_SEMI, 0))
    {
        // consume SEMI
        next_token(parser);
        
        if (node->mat.rows == node->mat.row_cap)
        {
            node->mat.row_cap *= 2;
            node->mat.matrix = realloc(node->mat.matrix, node->mat.row_cap * sizeof(ASTNode*));
        }
        
        node->mat.matrix[node->mat.rows++] = parse_row(parser, err);
    }

    // consume RBRACK
    next_token(parser);

    return node;
}

ASTNode* parse_row(Parser* parser, int* err)
{
    // make a row ASTNode
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_ROW;
    node->row.col_cap = 4;
    node->row.elements = malloc(4 * sizeof(ASTNode*));
    
    node->row.cols = 1;
    node->row.elements = parse_expr(parser, err);

    while (match_token(parser, TOK_COMMA, 0))
    {
        // consume COMMA
        next_token(parser);

        if (node->row.cols == node->row.col_cap)
        {
            node->row.col_cap *= 2;
            node->row.elements = realloc(node->row.elements, node->row.col_cap * sizeof(ASTNode*));
        }
        
        node->row.elements[node->row.cols++] = parse_expr(parser, err);
    }
    
    return node;
}

void print_ast(ASTNode* node, int indent) {
    if (!node) return;

    for (int i = 0; i < indent; i++) printf("  ");

    switch (node->type) {
        case AST_NUM:
            printf("NUM: %lf\n", node->value);
            break;
        case AST_IDENT:
            printf("VAR: %s\n", node->name);
            break;
        case AST_BINOP:
            printf("BINOP: %s\n", type_to_string(node->binary.op)); // or convert to string
            print_ast(node->binary.left, indent + 1);
            print_ast(node->binary.right, indent + 1);
            break;
        case AST_ASSIGN:
            printf("ASSIGN: %s\n", node->assign.name);
            print_ast(node->assign.expr, indent + 1);
            break;
        case AST_FUNC:
            printf("FUNC: %s\n", node->func.name);
            for (int i = 0; i < node->func.arg_size; i++)
            {
                print_ast(node->func.args[i], indent + 1);
            }
            break;
        default:
            printf("UNKNOWN\n");
    }
}

void free_ast(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
    case AST_FUNC:
        free(node->func.name);  // strdup’ed
        for (int i = 0; i < node->func.arg_size; i++) {
            free_ast(node->func.args[i]); // recursive free for children
        }
        free(node->func.args);
        break;

    case AST_ASSIGN:
        free(node->assign.name);
        free_ast(node->assign.expr);
        break;

    case AST_IDENT:
        free(node->name); // strdup’ed
        break;

    case AST_BINOP:
        free_ast(node->binary.left);
        free_ast(node->binary.right);
        break;

    case AST_NUM:
        // nothing heap-allocated
        break;
    case AST_UNOP:
        free_ast(node->binary.right);
        break;
    }

    free(node); // finally, free the node itself
}
