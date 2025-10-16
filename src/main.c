#define _POSIX_C_SOURCE 200809L
#include "lexer.h"
#include "parser.h"
#include "eval.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32

#else
    #include <readline/readline.h>
    #include <readline/history.h>
#endif
#define VAR_MAX 100

void clear_screen(void) 
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int main()
{
    Variable vars[VAR_MAX];
    Env env = {vars, 0};
    char *input;
    int tok_count;

    const char *history_file = ".interpreter_history";
    read_history(history_file);

    while (1)
    {
        int err = 0;

        input = readline("> ");
        if (!input) break; 

        if (input[0] == '\0') {
            free(input);
            continue;
        }

        add_history(input);

        if (strcmp(input, "quit") == 0) {
            free(input);
            break;
        } 
        else if (strcmp(input, "help") == 0) {
            print_help();
            free(input);
            continue;
        } 
        else if (strcmp(input, "cls") == 0) {
            clear_screen();
            free(input);
            continue;
        }
        else if (strcmp(input, "printvar") == 0) 
        {
            if (env.var_count > 0) 
            {
                for (int i = 0; i < env.var_count; i++) 
                {
                    printf("%s = ", env.var[i].name);
                    print_value(env.var[i].value, &err);
                }
            } 
            else
            {
                printf("No variables defined.\n");
            }
            continue; 
        }

        Token* tokens = generate_tokens(input, &tok_count, &err); 
        if (err) {
            free(input);
            continue;
        }

        Parser parser = {tokens, tok_count, 0, tokens[0]};
        ASTNode* root = parse(&parser, &err);
        if (err) {
            free_tokens(tokens, tok_count);
            free(input);
            continue;
        }

        if (parser.current_token.type != TOK_EOF) 
        {
            fprintf(stderr, "Syntax error: extra tokens at end of input\n");
            free_tokens(tokens, tok_count);
            free_ast(root);
            free(input);
            continue;
        }

        Value result = eval(root, &env, &err);
        if (!err && tok_count != 1)
        {   
            print_value(result, &err);
            if (root->type != AST_ASSIGN)
                free_value(result);
        }

        free_tokens(tokens, tok_count);
        free_ast(root);
        free(input);
    }

    write_history(history_file);

    printf("\nExiting Hoscript.\n");
    return 0;
}

