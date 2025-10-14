#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include "eval.h"

#define VAR_MAX 100

int main()
{
    char input[256];
    int tok_count;

    Variable vars[VAR_MAX];
    Env env = {vars, 0};

    while (1)
    {
        int err = 0;
        printf("> ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';
        if (strcmp(input, "quit") == 0)
        {
            break;
        }
        else if (strcmp(input, "help") == 0)
        {
            print_help();
            continue;
        }

        Token* tokens = generate_tokens(input, &tok_count, &err); 
        if (err) continue;


        Parser parser = {tokens, tok_count, 0, tokens[0]};
        
        ASTNode* root = parse(&parser, &err);
        if (err) continue;

        if (parser.current_token.type != TOK_EOF) {
            fprintf(stderr, "Syntax error: extra tokens at end of input\n");
            continue;
        }
        
        Value result = eval(root, &env, &err);
        if (!err && tok_count != 1)
        {   
            print_value(result, &err);

            if (root->type != AST_ASSIGN)
            {
                free_value(result);
            }
        }

        // at the end of a line, free the tokens and ast nodes
        free_tokens(tokens, tok_count);
        free_ast(root);
    }
}
