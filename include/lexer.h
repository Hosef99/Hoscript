#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOK_NUM,        // numbers
    TOK_PLUS,       // +
    TOK_MINUS,      // -
    TOK_MUL,        // *
    TOK_DIV,        // /
    TOK_POW,        // ^
    TOK_LPAREN,     // (
    TOK_RPAREN,     // )
    TOK_LBRACK,     // [
    TOK_RBRACK,     // ]
    TOK_LBRACE,     // {
    TOK_RBRACE,     // }
    TOK_IDENT,      // variable or function names
    TOK_COMMA,      // ,
    TOK_SEMI,       // ;
    TOK_COLON,      // :
    TOK_STRING,     // strings
    TOK_ASSIGN,     // =
    TOK_LESSER,     // <
    TOK_GREATER,    // >
    TOK_LAND,       // <=
    TOK_GAND,       // >=
    TOK_EQUAL,      // ==
    TOK_NEQUAL,     // !=
    TOK_NOT,        // !
    TOK_OR,         // ||
    TOK_AND,        // &&
    TOK_NEWLINE,    // \n
    TOK_IF,         // if
    TOK_ELSE,       // else
    TOK_FOR,        // for
    TOK_IN,
    TOK_EOF         // end of input
} TokenType;

typedef struct {
    TokenType type;
    double value;
    char* name;
} Token;

Token lex(const char* input, int* pos, int* err); 
char* type_to_string(TokenType type);
void eval_line(Token* tokens, int token_count);
Token* generate_tokens(const char* input, int* out_count, int* err);
void print_token(Token token);
void print_tokens(Token* tokens, int size);
void free_tokens(Token* tokens, int count);
#endif
