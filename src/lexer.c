#define _POSIX_C_SOURCE 200809L
#include <stdio.h>   // printf, scanf
#include <stdlib.h>  // malloc, free, atof, exit
#include <string.h>  // strcmp, strdup, strlen
#include <ctype.h>   // isdigit, isalpha, isspace
#include "lexer.h" // lexer header file

// ----------------------------- START FUNCTION ------------------------------------

Token lex_number(const char* input, int* pos, int* err);
Token lex_identifier(const char* input, int* pos, int* err);
Token lex_string(const char* input, int* pos, int* err);

char* type_to_string(TokenType type)
{
    switch (type)
    {
    case TOK_NUM:       return "Number";
    case TOK_PLUS:      return "Plus";
    case TOK_MINUS:     return "Minus";
    case TOK_MUL:       return "Multiply";
    case TOK_DIV:       return "Divide";
    case TOK_POW:       return "Power";
    case TOK_LPAREN:    return "LeftParen";
    case TOK_RPAREN:    return "RightParen";
    case TOK_LBRACK:    return "LeftBrack";
    case TOK_RBRACK:    return "RightBrack";
    case TOK_LBRACE:    return "LeftBrace";
    case TOK_RBRACE:    return "RightBrace";
    case TOK_IDENT:     return "Identifier";
    case TOK_COMMA:     return "Comma";
    case TOK_SEMI:      return "Semicolon";
    case TOK_COLON:     return "Colon";
    case TOK_STRING:    return "String";
    case TOK_ASSIGN:    return "Equals";
    case TOK_LESSER:    return "Lesser";
    case TOK_GREATER:   return "Greater";
    case TOK_LAND:      return "LesserEqual";
    case TOK_GAND:      return "GreaterEqual";
    case TOK_EQUAL:     return "Equal";
    case TOK_NEQUAL:    return "NotEqual";
    case TOK_NEWLINE:   return "Newline";
    case TOK_IF:        return "If";
    case TOK_ELSE:      return "Else";
    case TOK_FOR:       return "For";
    case TOK_IN:        return "In";
    case TOK_EOF:       return "EOF";
    default:            return "Unknown";
    }
}

void eval_line(Token* tokens, int token_count)
{
    for (int i = 0; i < token_count; i++)
    {
        printf("Token %d\nType: %s\nName: %s\nValue: %lf\n\n", i, type_to_string(tokens[i].type) , tokens[i].name ? tokens[i].name : "(none)", tokens[i].value);
    }

    for (int i = 0; i < token_count; i++) {
        if (tokens[i].name) {
            free(tokens[i].name);
        }
    }
}

Token lex(const char* input, int* pos, int* err)
{
    Token tok = {TOK_EOF, 0, NULL};
    while (input[*pos] != '\0')
    {
        if (input[*pos] == ' ') 
        {
            (*pos)++; 
            continue;
        }

        if (isdigit(input[*pos])) return lex_number(input, pos, err);
        if (input[*pos] == '+') { (*pos)++; return (Token){TOK_PLUS, 0, NULL}; }
        if (input[*pos] == '-') { (*pos)++; return (Token){TOK_MINUS, 0, NULL}; }
        if (input[*pos] == '*') { (*pos)++; return (Token){TOK_MUL, 0, NULL}; }
        if (input[*pos] == '/') { (*pos)++; return (Token){TOK_DIV, 0, NULL}; }
        if (input[*pos] == '^') { (*pos)++; return (Token){TOK_POW, 0, NULL}; }
        if (input[*pos] == '(') { (*pos)++; return (Token){TOK_LPAREN, 0, NULL}; }
        if (input[*pos] == ')') { (*pos)++; return (Token){TOK_RPAREN, 0, NULL}; }
        if (input[*pos] == '[') { (*pos)++; return (Token){TOK_LBRACK, 0, NULL}; }
        if (input[*pos] == ']') { (*pos)++; return (Token){TOK_RBRACK, 0, NULL}; }
        if (input[*pos] == '{') { (*pos)++; return (Token){TOK_LBRACE, 0, NULL}; }
        if (input[*pos] == '}') { (*pos)++; return (Token){TOK_RBRACE, 0, NULL}; }
        if (input[*pos] == ',') { (*pos)++; return (Token){TOK_COMMA, 0, NULL}; }
        if (input[*pos] == ';') { (*pos)++; return (Token){TOK_SEMI, 0, NULL}; }
        if (input[*pos] == ':') { (*pos)++; return (Token){TOK_COLON, 0, NULL}; }
        if (input[*pos] == '"') { (*pos)++; return lex_string(input, pos, err);}
        if (input[*pos] == '\n') { (*pos)++; return (Token){TOK_NEWLINE, 0, NULL}; }

        if (input[*pos] == '<')
        {
            if (input[*pos + 1] == '=') 
            {
                *pos += 2;
                return (Token){TOK_LAND, 0, NULL};  // <=
            } 
            else 
            {
                (*pos)++;
                return (Token){TOK_LESSER, 0, NULL}; // <
            }

        }
        if (input[*pos] == '>') 
        {
            if (input[*pos + 1] == '=') 
            {
                *pos += 2;
                return (Token){TOK_GAND, 0, NULL};  // >=
            } 
            else 
            {
                (*pos)++;
                return (Token){TOK_GREATER, 0, NULL}; // >
            }
        }
        if (input[*pos] == '=')
        {
            if (input[*pos + 1] == '=')
            {
                *pos += 2;
                return (Token){TOK_EQUAL, 0, NULL}; // ==
            }
            else
            {
                (*pos)++;
                return (Token){TOK_ASSIGN, 0, NULL}; // =
            }
        }
        if (input[*pos] == '!')
        {
            if (input[*pos + 1] == '=')
            {
                *pos += 2;
                return (Token){TOK_NEQUAL, 0, NULL}; // !=
            }
        }
        if (input[*pos] == '=') { (*pos)++; return (Token){TOK_ASSIGN, 0, NULL}; }
        if (isalpha(input[*pos]) || input[*pos] == '_') return lex_identifier(input, pos, err);
        
        printf("Syntax error, or not yet implemented.\n");
        *err = 1;
        return tok;
    }
    return tok;
}

Token lex_number(const char* input, int* pos, int* err)
{
    // use atof to turn string into double
    Token token = {TOK_NUM, 0, NULL};
    char buffer[64]; // 63 characters including the terminator '\0'
    int index = 0;
    int has_dot = 0;
    int has_e = 0;
    while (isdigit(input[*pos]) || input[*pos] == '.' || input[*pos] == 'e' || input[*pos] == '+' || input[*pos] == '-')
    {
        if (input[*pos] == '.')
        {
            if (has_dot)
            {
                fprintf(stderr, "Number Error: multiple decimal point in number.\n");
                *err = 1;
                return token;
            }
            has_dot = 1;
        }

        if (input[*pos] == 'e')
        {
            if (has_e)
            {
                fprintf(stderr, "Number Error: multiple e in number\n");
                *err = 1;
                return token;
            }
            has_e = 1;
        }

        if (input[*pos] == '+' || input[*pos] == '-')
        {
            if (input[(*pos)-1] != 'e')
            {
                break;
            }
        }

        if (index >= 63)
        {
            printf("Number Error: Too long.\n");
            *err = 1;
            return token;
        }
        
        buffer[index++] = input[(*pos)++];
    }
    buffer[index] = '\0';

    char* end;
    token.value = strtod(buffer, &end);

    if (*end != '\0')
    {
        printf("Number Error: invalid characters in number.\n");
        *err = 1;
        return token;
    }

    return token;
}

Token lex_identifier(const char* input, int* pos, int* err)
{
    Token token;
    token.type = TOK_IDENT;
    char buffer[64];
    int index = 0;
    while (isalpha(input[*pos]) || isdigit(input[*pos]) || input[*pos] == '_')
    {
        if (index >= 63)
        {
            printf("Number Error: Too long.\n");
            *err = 1;
            return token;
        }
        
        buffer[index++] = input[(*pos)++];
    }
    buffer[index] = '\0';
    if (strcmp(buffer, "if") == 0) return (Token) {TOK_IF, 0, NULL};
    if (strcmp(buffer, "else") == 0) return (Token) {TOK_ELSE, 0, NULL};
    if (strcmp(buffer, "for") == 0) return (Token) {TOK_FOR, 0, NULL};
    if (strcmp(buffer, "in") == 0) return (Token) {TOK_IN, 0, NULL};
    token.name = strdup(buffer); // NEED TO BE CAREFUL OF MEMORY LEAK
    token.value = 0;
    return token;
}

Token lex_string(const char* input, int* pos, int* err)
{
    Token token;
    token.type = TOK_STRING;
    int size = 1024;
    char* buffer = malloc(size * sizeof(char));
    int index = 0;
    while(input[*pos] != '"') {
        if (index >= size) {
            size *= 2;
            buffer = realloc(buffer, size * sizeof(char));
        }

        if (input[*pos] == '\0') {
            fprintf(stderr, "String Error: no closing \" found\n");
            *err = 1;
            return token;
        }

        if (input[*pos] == '\\') {
            switch (input[(*pos) + 1]) {
                case '\\':
                    buffer[index++] = '\\';
                    (*pos) += 2;
                    break;
                case 'n':
                    buffer[index++] = '\n';
                    (*pos) += 2;
                    break;
                default:
                    fprintf(stderr, "String error: no suitable escape character for '\\'");
                    *err = 1;
                    return token;
            }
            continue;
        }

        buffer[index++] = input[(*pos)++];
    }
    (*pos)++;
    buffer[index] = '\0';
    token.name = strdup(buffer);
    token.value = 0;
    return token;
}

Token* generate_tokens(const char* input, int* out_count, int* err) {
    int capacity = 16;
    int size = 0;
    Token* tokens = malloc(capacity * sizeof(Token)); // free tokens in the future
    
    int pos = 0;
    while (1) {
        Token tok = lex(input, &pos, err);
        if (*err) return NULL;

        if (size >= capacity) {
            capacity *= 2;
            tokens = realloc(tokens, capacity * sizeof(Token));
        }

        tokens[size++] = tok;
        if (tok.type == TOK_EOF) break;
    }

    *out_count = size;
    return tokens;
}

void print_token(Token token)
{
    printf("Type: %s\nName: %s\nValue: %.2lf\n\n", type_to_string(token.type), token.name, token.value);
}

void print_tokens(Token* tokens, int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("Token %d\n", i);
        print_token(tokens[i]);
    }
}

void free_tokens(Token* tokens, int count)
{
    for (int i = 0; i < count; i++)
    {
        free(tokens[i].name);
    }
    free(tokens);
}
