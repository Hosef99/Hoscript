#include <stdio.h>
#include "lexer.h"

int main()
{
    const char *input = "x = [[2,3,4], [5,5,7]]";
    eval_line(input);
}