#include <stdio.h>   // printf, scanf
#include <stdlib.h>  // malloc, free, atof, exit
#include <string.h>  // strcmp, strdup, strlen
#include <ctype.h>   // isdigit, isalpha, isspace
// #include "parser.h"
#include "lexer.h"
#include "parser.h"


void next_token(Parser* parser);
ASTNode* parse_assign(Parser* parser);
ASTNode* parse_expr(Parser* parser);
ASTNode* parse_term(Parser* parser);
ASTNode* parse_factor(Parser* parser);
ASTNode* parse_base(Parser* parser);
ASTNode* parse_func(Parser* parser);
void print_ast(ASTNode* node, int indent);

void next_token(Parser* parser)
{
    parser->current_token = parser->tokens[++(parser->pos)];
}

ASTNode* make_op_node(TokenType op, ASTNode* left, ASTNode* right)
{
    ASTNode* node = malloc(sizeof(ASTNode));

    node->type = AST_BINOP;
    node->binary.op = op;
    node->binary.left = left;
    node->binary.right = right;
    return node;
}

ASTNode* parse(Parser* parser)
{
    // need to do some filtering, look ahead for second token 
    if (parser->current_token.type == TOK_IDENT) // identifier
    {   
        if (parser->tokens[parser->pos+1].type == TOK_ASSIGN) // look ahead, be careful of spaces
        {
            return parse_assign(parser);
        }
    }
    
    return parse_expr(parser);
}

ASTNode* parse_assign(Parser* parser)
{
    ASTNode* node = malloc(sizeof(ASTNode));

    node->assign.name = strdup(parser->current_token.name);
    // first token must be ident
    next_token(parser); // IDENT 
    next_token(parser); // '='
    
    ASTNode* rhs = parse_expr(parser);

    node->type = AST_ASSIGN;
    node->assign.expr = rhs;
    return node;
}

ASTNode* parse_expr(Parser* parser)
{
    ASTNode* left = parse_term(parser);

    while (parser->current_token.type == TOK_PLUS || parser->current_token.type == TOK_MINUS)
    {
        TokenType op = parser->current_token.type;
        next_token(parser);

        ASTNode* right = parse_term(parser);

        ASTNode* new_node = make_op_node(op, left, right);
        left = new_node;
    }
    return left;
}

ASTNode* parse_term(Parser* parser)
{

    ASTNode* left = parse_factor(parser);

    while (parser->current_token.type == TOK_MUL || parser->current_token.type == TOK_DIV)
    {
        TokenType op = parser->current_token.type;
        next_token(parser);

        ASTNode* right = parse_factor(parser);

        ASTNode* new_node = make_op_node(op, left, right);
        left = new_node;
    }
    return left;
}

ASTNode* parse_factor(Parser* parser)
{
    ASTNode* left = parse_base(parser);

    if (parser->current_token.type == TOK_POW)
    {
        TokenType op = TOK_POW;
        next_token(parser);
        
        ASTNode* right = parse_factor(parser);
        ASTNode* op_node = make_op_node(op, left, right);
        left = op_node;
    }

    return left;
}

ASTNode* parse_base(Parser* parser)
{
    ASTNode* new_node = malloc(sizeof(ASTNode));
    if (parser->current_token.type == TOK_NUM) // number
    {
        new_node->type = AST_NUM;
        new_node->value = parser->current_token.value;
        next_token(parser);
    }
    else if (parser->current_token.type == TOK_IDENT) // if first token identifier
    {
        if (parser->tokens[parser->pos+1].type == TOK_LPAREN) // second token LPAREN
        {
            parse_func(parser);
        }
        else // identifier
        {
            new_node->type = AST_IDENT;
            new_node->name = strdup(parser->current_token.name); // need to free memory later
            next_token(parser);
        }
    }
    else if (parser->current_token.type == TOK_LPAREN) // LPAREN expr RPARAN
    {
        next_token(parser);
        new_node = parse_expr(parser);

        if (parser->current_token.type != TOK_RPAREN) // SYNTAX ERROR!!!
        {
            fprintf(stderr, "Syntax error: expected ')'\n");
            return NULL;
        }

        next_token(parser);
    }
    return new_node;
}

ASTNode* parse_func(Parser* parser)
{
    printf("enter func");
    // grammar
    // func -> IDENT LPARAN (expr (COMMA expr)*)? RPARAN
    ASTNode* node = malloc(sizeof(ASTNode));
    node->func.name = strdup(parser->current_token.name);
    next_token(parser); // IDENT
    next_token(parser); // LPARAN
    
    // expr
    if (parser->current_token.type != TOK_RPAREN) // if not RPAREN, then its an expr
    {
        node->func.args[0] = parse_expr(parser);
        node->func.arg_size = 1;
        while (parser->current_token.type == TOK_COMMA)
        {
            // consume comma first, then parse expr
            next_token(parser);
            node->func.args[node->func.arg_size++] = parse_expr(parser);
        }
    }
    // consume RPAREN
    next_token(parser);
    
    node->type = AST_FUNC;

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
        default:
            printf("UNKNOWN\n");
    }
}

void print_ast_type(ASTNode* node)
{
    switch (node->type)
    {
    case AST_ASSIGN:
        printf("ASSIGN\n");
        break;
    case AST_BINOP:
        printf("BINOP\n");
        break;
    case AST_FUNC:
        printf("FUNC\n");
        break;
    case AST_IDENT:
        printf("IDENT\n");
        break;
    case AST_NUM:
        printf("NUM\n");
        break;
    default:
        printf("UNKNOWN\n");
        break;
    }
}

int main()
{
    char input[256];
    int tok_count;

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
        // print_token(parser.current_token);
        // print_ast_type(root);
        if (root->type == -1)
        {
            printf("NULL");
        }
        
        print_ast(root, 0);
    }
}