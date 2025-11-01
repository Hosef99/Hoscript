#include "lexer.h"
#define _POSIX_C_SOURCE 200809L
#include <stdio.h> // printf, scanf
#include <stdlib.h> // calloc, free, atof, exit 
#include <string.h> // strcmp, strdup, strlen
#include <ctype.h> // isdigit, isalpha, isspace
#include "parser.h"
#include "value.h"

ASTNode* parse_stmt(Parser* parser, int* err);
ASTNode* parse_block(Parser* parser, int* err);
ASTNode* parse_or(Parser* parser, int* err); 
ASTNode* parse_and(Parser* parser, int* err);   
ASTNode* parse_if(Parser* parser, int* err);
ASTNode* parse_for(Parser* parser, int* err);
ASTNode* parse_assign(Parser* parser, int* err); 
ASTNode* parse_comp(Parser* parser, int* err); 
ASTNode* parse_expr(Parser* parser, int* err); 
ASTNode* parse_term(Parser* parser, int* err); 
ASTNode* parse_factor(Parser* parser, int* err); 
ASTNode* parse_mul(Parser* parser, int* err); 
ASTNode* parse_power(Parser* parser, int* err); 
ASTNode* parse_base(Parser* parser, int* err); 
ASTNode* parse_func(Parser* parser, int* err); 
ASTNode* parse_list(Parser* parser, int* err); 
ASTNode* parse_postfix(Parser* parser, int* err); 
ASTNode* parse_slice(Parser* parser, int* err);
int implicit_mul(Parser* parser);

static Token peek(Parser* parser, int offset) // offset tells the parser how many tokens ahead it should look 
{
    int pos = parser->pos + offset; // offset = 0 , remain position | offset = 1 , next position | offset = 2 , next two position | offset = -1 , previous position
    if (pos >= parser->num_tokens) // while position reach last token | handle condition when u call peek(parser, "5") while there are only "4" tokens
        return (Token){TOK_EOF, 0, NULL}; // return EOF
    return parser->tokens[pos]; // return the token at specific position
}

static void advance(Parser* parser) // next_token
{
    if (parser->pos < parser->num_tokens) // checking validation
        parser->pos++; // next position
    parser->current_token = peek(parser, 0); // ni
}

static int match(Parser* parser, TokenType type) // match current token type
{
    return parser->current_token.type == type; // see whether return 0 or 1 (conditions)
}

static int look_ahead(Parser* parser, TokenType type, int n) // match next token type (e.g. after "func", call look_ahead(parser, TOK_LPAREN, 1) which should be returning 1 otherwise it can't be a function)
{
    return peek(parser, n).type == type; // ni
}

static ASTNode* make_op_node(TokenType op, ASTNode* left, ASTNode* right) // didn't change
{
    ASTNode* node = calloc(1, sizeof(ASTNode)); // Allocate memory for 1 node and initialize all data to 0 or NULL
    node->type = AST_BINOP; 
    node->binary.op = op;
    node->binary.left = left;
    node->binary.right = right;
    return node;
}

static ASTNode* make_unary_node(TokenType op, ASTNode* right) // will be called in parse_factor() if (sign == -1)
{
    ASTNode* node = calloc(1, sizeof(ASTNode));
    node->type = AST_UNOP;
    node->unary.op = op;
    node->unary.right = right;
    return node;
}

static ASTNode* make_index_node(ASTNode* left, ASTNode* index)  // Creates an indexing node (like A[3]), will be called in parse_postfix() looking for []
{
    ASTNode* node = calloc(1, sizeof(ASTNode));
    node->type = AST_INDEX;
    node->binary.left = left; // "A"
    node->binary.right = index; // '3'

    left->parent = node;
    index->parent = node;

    return node;
}

ASTNode* parse(Parser* parser, int* err) 
{
    int program_size = 4;
    ASTNode* program = malloc(sizeof(ASTNode));
    program->type = AST_PROGRAM;
    program->program.count = 0;
    program->program.nodes = malloc(program_size*sizeof(ASTNode*));

    parser->current_token = peek(parser, 0); // ni
    ASTNode* stmt = parse_stmt(parser, err); // take over parse_assign()
    if (*err) return NULL;

    program->program.nodes[program->program.count++] = stmt;

    while (match(parser, TOK_NEWLINE)) {
        advance(parser);
        if (program->program.count == program_size)
        {
            program_size *= 2;
            program->program.nodes = realloc(program->program.nodes, program_size * sizeof(ASTNode*));
        }
        stmt = parse_stmt(parser, err);
        if (stmt->type == AST_EOF)
        {
            free_ast(stmt);
            continue;
        }
        if (*err) return NULL;
        program->program.nodes[program->program.count++] = stmt;
    }

    if (!match(parser, TOK_EOF)) 
    {
        fprintf(stderr, "Syntax error: extra tokens at end of input\n");
        *err = 1;
    }
    return program;
}

ASTNode* parse_stmt(Parser* parser, int* err)
{
    if (match(parser, TOK_IF)) return parse_if(parser, err);
    if (match(parser, TOK_FOR)) return parse_for(parser, err);
    if (match(parser, TOK_LBRACE)) return parse_block(parser, err);
    return parse_assign(parser, err);
}

ASTNode* parse_if(Parser* parser, int* err)
{
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_IF;
    advance(parser); // consume IF
    advance(parser); // consume LPAREN
    node->if_node.cond = parse_assign(parser, err);
    advance(parser); // consume RPAREN
    while (match(parser, TOK_NEWLINE)) {
        advance(parser);
    }
    node->if_node.if_stmt = parse_stmt(parser, err);
    if (match(parser, TOK_ELSE)) {
        advance(parser); // consume ELSE
        while (match(parser, TOK_NEWLINE)) {
            advance(parser);
        }
        node->if_node.else_stmt = parse_stmt(parser, err);
    }
    else {
        node->if_node.else_stmt = NULL;
    }

    return node;
}

ASTNode* parse_for(Parser* parser, int* err)
{
    ASTNode* for_node = malloc(sizeof(ASTNode));
    for_node->type = AST_FOR;
    advance(parser); // consume FOR
    advance(parser); // consume LPAREN
    for_node->for_node.var = strdup(parser->current_token.name);
    advance(parser); // consume IDENT
    advance(parser); // consume IN
    for_node->for_node.iterator = parse_expr(parser, err);
    advance(parser);
    while (match(parser, TOK_NEWLINE)) {
        advance(parser);
    }
    for_node->for_node.body = parse_stmt(parser, err);
    return for_node;
}

ASTNode* parse_block(Parser* parser, int* err){
    advance(parser);
    int block_size = 4;
    ASTNode* block = malloc(sizeof(ASTNode));
    block->type = AST_PROGRAM;
    block->program.count = 0;
    block->program.nodes = malloc(block_size*sizeof(ASTNode*));

    while (match(parser, TOK_NEWLINE)) {
        advance(parser);
    }

    parser->current_token = peek(parser, 0); // ni
    ASTNode* assign = parse_stmt(parser, err); // take over parse_assign()
    if (*err) return NULL;

    block->program.nodes[block->program.count++] = assign;

    while (match(parser, TOK_NEWLINE)) {
        advance(parser);
        if (match(parser, TOK_RBRACE)) {
            advance(parser);
            break;
        }
        if (block->program.count == block_size)
        {
            block_size *= 2;
            block->program.nodes = realloc(block->program.nodes, block_size * sizeof(ASTNode*));
        }
        assign = parse_stmt(parser, err);
        if (assign->type == AST_EOF)
        {
            free_ast(assign);
            continue;
        }
        if (*err) return NULL;
        block->program.nodes[block->program.count++] = assign;
    }
    return block;
}

// Top-level
ASTNode* parse_assign(Parser* parser, int* err) 
{
    ASTNode* left = parse_or(parser, err);
    if (*err) return NULL;

    if (match(parser, TOK_ASSIGN)) 
    {   
        if (left->type != AST_IDENT && left->type != AST_INDEX) 
        {
            fprintf(stderr, "Syntax Error: invalid assignment target\n");
            *err = 1;
            return NULL;
        }
        advance(parser);
        ASTNode* rhs = parse_assign(parser, err); 
        if (*err) return NULL;

        ASTNode* assign = calloc(1, sizeof(ASTNode));
        assign->type = AST_ASSIGN;
        assign->assign.left = left;
        assign->assign.expr = rhs;
        return assign;
    }

    return left;
}

ASTNode* parse_or(Parser* parser, int* err)
{
    ASTNode* left = parse_and(parser, err);
    if (*err) return NULL;

    while (match(parser, TOK_OR)) {
        TokenType op = parser->current_token.type;
        advance(parser);
        ASTNode* right = parse_and(parser, err);
        if (*err) return NULL;
        left = make_op_node(op, left, right);
    }

    return left;
}

ASTNode* parse_and(Parser* parser, int* err)
{
    ASTNode* left = parse_comp(parser, err);
    if (*err) return NULL;

    while (match(parser, TOK_AND)) {
        TokenType op = parser->current_token.type;
        advance(parser);
        ASTNode* right = parse_comp(parser, err);
        if (*err) return NULL;
        left = make_op_node(op, left, right);
    }

    return left;
}

ASTNode* parse_comp(Parser* parser, int* err)
{
    ASTNode* left = parse_expr(parser, err); // parse the first term
    while (match(parser, TOK_LESSER) || match(parser, TOK_GREATER) || match(parser, TOK_LAND) || match(parser, TOK_GAND) || match(parser, TOK_EQUAL) || match(parser, TOK_NEQUAL)) 
    {   
        TokenType op = parser->current_token.type; // assign operator
        advance(parser); // next token
        ASTNode* right = parse_expr(parser, err);
        left = make_op_node(op, left, right);        
    }

    return left;
}

ASTNode* parse_expr(Parser* parser, int* err) // Handle +, -
{
    ASTNode* left = parse_term(parser, err); 
    if (*err) return NULL;

    while (match(parser, TOK_PLUS) || match(parser, TOK_MINUS)) 
    {
        TokenType op = parser->current_token.type; // assign operator
        advance(parser); // next token
        ASTNode* right = parse_term(parser, err); // parse the next term
        if (*err) return NULL;
        left = make_op_node(op, left, right);
    }

    return left;
}

ASTNode* parse_term(Parser* parser, int* err)
{
    ASTNode* left = parse_mul(parser, err); // parse the first term
    while (match(parser, TOK_MUL) || match(parser, TOK_DIV)) 
    {
        TokenType op = parser->current_token.type; // assign operator
        advance(parser); // next token
        ASTNode* right = parse_mul(parser, err);
        left = make_op_node(op, left, right);
    }
    return left;
}

ASTNode* parse_mul(Parser* parser, int* err) // e.g. 7a 
{
    ASTNode* left = parse_factor(parser, err); // 7 will be assigned
    while (implicit_mul(parser)) // a is the current parser, 7 is the previous one, so it should be 1
    {
        ASTNode* right = parse_factor(parser, err); // set right equal a
        left = make_op_node(TOK_MUL, left, right); // left = 7 , mid = * , right = a
    }
    return left;
}

ASTNode* parse_factor(Parser* parser, int* err)
{
    while (match(parser, TOK_NOT)) 
    {
        TokenType op = parser->current_token.type;
        advance(parser);
        ASTNode* right = parse_factor(parser, err);
        if (*err) return NULL;
        return make_unary_node(op, right);
    }
    int sign = 1;
    while (match(parser, TOK_PLUS) || match(parser, TOK_MINUS)) 
    {
        if (match(parser, TOK_MINUS))
            sign = -sign;
        advance(parser);
    }

    ASTNode* node = parse_power(parser, err);
    if (*err) return NULL;

    if (sign == -1)
        node = make_unary_node(TOK_MINUS, node);

    return node;
}

ASTNode* parse_power(Parser* parser, int* err)
{
    ASTNode* left = parse_postfix(parser, err);
    if (match(parser, TOK_POW)) 
    {
        advance(parser);
        ASTNode* right = parse_factor(parser, err);
        left = make_op_node(TOK_POW, left, right);
    }
    return left;
}

ASTNode* parse_postfix(Parser* parser, int* err) // parsing []
{
    ASTNode* node = parse_base(parser, err);
    while (match(parser, TOK_LBRACK)) 
    {
        advance(parser);
        ASTNode* index = parse_slice(parser, err); // returning an integer from parse_base()
        if (!match(parser, TOK_RBRACK)) 
        {
            fprintf(stderr, "Syntax Error: expected ']'\n");
            *err = 1;
            return NULL;
        }
        advance(parser);
        node = make_index_node(node, index);
    }
    return node;
}

ASTNode* parse_slice(Parser* parser, int* err)
{
    ASTNode* left = parse_expr(parser, err);
    if (match(parser, TOK_COLON)) {
        advance(parser);
        ASTNode* right = parse_expr(parser, err);
        left = make_op_node(TOK_COLON, left, right);
        left->type = AST_SLICE;
    }
    return left;
}

ASTNode* parse_base(Parser* parser, int* err)
{
    if (match(parser, TOK_NUM)) 
    {
        ASTNode* node = calloc(1, sizeof(ASTNode));
        node->type = AST_NUM;
        node->value = parser->current_token.value;
        advance(parser);
        return node;
    }

    if (match(parser, TOK_IDENT)) 
    {
        if (look_ahead(parser, TOK_LPAREN, 1))
            return parse_func(parser, err);

        ASTNode* node = calloc(1, sizeof(ASTNode));
        node->type = AST_IDENT;
        node->name = strdup(parser->current_token.name);
        advance(parser);
        return node;
    }

    if (match(parser, TOK_LPAREN)) 
    {
        advance(parser);
        ASTNode* node = parse_assign(parser, err);
        if (!match(parser, TOK_RPAREN)) 
        {
            fprintf(stderr, "Syntax error: expected ')'\n");
            *err = 1;
            return NULL;
        }
        advance(parser);
        return node;
    }

    if (match(parser, TOK_LBRACK))
        return parse_list(parser, err);

    if (match(parser, TOK_STRING))
    {
        ASTNode* node = malloc(sizeof(ASTNode));
        node->name = strdup(parser->current_token.name);
        node->type = AST_STRING;
	advance(parser);
        return node;
    }

    if (match(parser, TOK_EOF) || match(parser, TOK_NEWLINE))
    {
        ASTNode* node = malloc(sizeof(ASTNode));
        node->type = AST_EOF;
        return node;
    }

    fprintf(stderr, "Syntax error: unexpected token '%s'\n",
            type_to_string(parser->current_token.type));
    *err = 1;
    return NULL;
}

ASTNode* parse_func(Parser* parser, int* err)
{
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_FUNC;
    node->func.name = strdup(parser->current_token.name);
    node->func.arg_size = 0;

    advance(parser); // consume IDENT

    if (parser->current_token.type != TOK_LPAREN) 
    {
        fprintf(stderr, "Syntax Error: expected '('\n");
        *err = 1;
        return node;
    }

    advance(parser); // consume '('
    int cap = 4;
    node->func.args = malloc(cap * sizeof(ASTNode*));

    while (parser->current_token.type != TOK_RPAREN && !*err) 
    {
        if (node->func.arg_size == cap) 
        {
            cap *= 2;
            node->func.args = realloc(node->func.args, cap * sizeof(ASTNode*));
        }

        node->func.args[node->func.arg_size++] = parse_assign(parser, err);
        if (*err) return node;

        if (parser->current_token.type == TOK_COMMA)
            advance(parser); // consume ','
        else if (parser->current_token.type != TOK_RPAREN) 
        {
            fprintf(stderr, "Syntax Error: expected ',' or ')'\n");
            *err = 1;
            return node;
        }
    }

    advance(parser); // consume ')'
    return node;
}

ASTNode* parse_list(Parser* parser, int* err)
{
    advance(parser); // consume '['

    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_LIST;
    node->list.max_size = 4;
    node->list.elements = malloc(node->list.max_size * sizeof(ASTNode*));
    node->list.size = 0;

    if (match(parser, TOK_RBRACK)) {
        advance(parser);
        return node;
    }

    node->list.elements[0] = parse_assign(parser, err);
    node->list.size = 1;


    while (parser->current_token.type == TOK_COMMA && !*err) 
    {
        advance(parser); // consume ','

        if (node->list.size == node->list.max_size) 
        {
            node->list.max_size *= 2;
            node->list.elements = realloc(node->list.elements, node->list.max_size * sizeof(ASTNode*));
        }

        node->list.elements[node->list.size++] = parse_assign(parser, err);
    }

    if (parser->current_token.type != TOK_RBRACK) 
    {
        fprintf(stderr, "Syntax Error: expected ']'\n");
        *err = 1;
        return node;
    }

    advance(parser); // consume ']'
    return node;
}

int implicit_mul(Parser* parser)
{
    TokenType prev = parser->tokens[parser->pos - 1].type;
    TokenType curr = parser->current_token.type;

    int left_ok = (prev == TOK_NUM || prev == TOK_RPAREN || prev == TOK_RBRACK);
    int right_ok = (curr == TOK_IDENT || curr == TOK_LPAREN || curr == TOK_LBRACK);

    return left_ok && right_ok;
}

void print_ast(ASTNode* node, int indent)
{
    for (int i = 0; i < indent; i++) printf("  ");
    if (!node) {
        printf("NULL\n");
        return;
    }

    switch (node->type) {
        case AST_NUM:
            printf("NUM: %lf\n", node->value);
            break;
	case AST_STRING:
	    printf("STR: %s\n", node->name);
	    break;
        case AST_IDENT:
            printf("VAR: %s\n", node->name);
            break;
        case AST_BINOP:
            printf("BINOP: %s\n", type_to_string(node->binary.op));
            print_ast(node->binary.left, indent + 1);
            print_ast(node->binary.right, indent + 1);
            break;
        case AST_INDEX:
            printf("INDEX:\n");
            print_ast(node->binary.left, indent + 1);
            print_ast(node->binary.right, indent + 1);
            break;
        case AST_SLICE:
            printf("SLICE:\n");
            print_ast(node->binary.left, indent + 1);
            print_ast(node->binary.right, indent + 1);
            break;
        case AST_UNOP:
            printf("UNOP: %d\n", node->unary.op);
            print_ast(node->unary.right, indent + 1);
            break;
        case AST_ASSIGN:
            printf("ASSIGN:\n");
            print_ast(node->assign.left, indent + 1);
            print_ast(node->assign.expr, indent + 1);
            break;
        case AST_FUNC:
            printf("FUNC: %s (%d args)\n", node->func.name, node->func.arg_size);
            for (int i = 0; i < node->func.arg_size; i++)
                print_ast(node->func.args[i], indent + 1);
            break;
        case AST_LIST:
            printf("LIST (%d elements)\n", node->list.size);
            for (int i = 0; i < node->list.size; i++)
                print_ast(node->list.elements[i], indent + 1);
            break;
        case AST_IF:
            printf("IF:\n");
            print_ast(node->if_node.cond, indent + 1);
            for (int i = 0; i < indent+1; i++) printf("  ");
            printf("IF_STMT:\n");
            print_ast(node->if_node.if_stmt, indent + 1);
            for (int i = 0; i < indent+1; i++) printf("  ");
            printf("ELSE_STMT:\n");
            print_ast(node->if_node.else_stmt, indent + 1);
            break;
        case AST_FOR:
            printf("FOR:\n");
            for (int i = 0; i < indent+1; i++) printf("  ");
            printf("Var: %s\n", node->for_node.var);
            for (int i = 0; i < indent+1; i++) printf("  ");
            printf("ITERATOR:\n");
            print_ast(node->for_node.iterator, indent + 1);
            for (int i = 0; i < indent+1; i++) printf("  ");
            printf("BODY:\n");
            print_ast(node->for_node.body, indent + 1);
            break;
        case AST_PROGRAM:
            printf("PROGRAM:\n");
            for (int i = 0; i < node->program.count; i++)
                print_ast(node->program.nodes[i], indent + 1);
            break;
        default:
            printf("UNKNOWN\n");
    }
}

void free_ast(ASTNode* node)
{
    if (!node) return;

    switch (node->type) {
        case AST_FUNC:
            free(node->func.name);
            for (int i = 0; i < node->func.arg_size; i++)
                free_ast(node->func.args[i]);
            free(node->func.args);
            break;

        case AST_ASSIGN:
            free_ast(node->assign.left);
            free_ast(node->assign.expr);
            break;

        case AST_IDENT:
            free(node->name);
            break;
        case AST_BINOP:
        case AST_INDEX:
        case AST_SLICE:
            free_ast(node->binary.left);
            free_ast(node->binary.right);
            break;
        case AST_UNOP:
            free_ast(node->unary.right);
            break;
        case AST_LIST:
            for (int i = 0; i < node->list.size; i++)
                free_ast(node->list.elements[i]);
            free(node->list.elements);
            break;
        case AST_IF:
            free_ast(node->if_node.cond);
            free_ast(node->if_node.if_stmt);
            free_ast(node->if_node.else_stmt);
            break;
        case AST_FOR:
            free(node->for_node.var);
            free_ast(node->for_node.iterator);
            free_ast(node->for_node.body);
            break;
        case AST_PROGRAM:
            for (int i = 0; i < node->program.count; i++)
                free_ast(node->program.nodes[i]);
            free(node->program.nodes);
            break;
        default:
            break;
    }

    free(node);
}
