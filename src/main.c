#include "eval.h"

int main(int argc, char* argv[])
{
    if (argc == 1) {
        start_repl();
    }
    else {
        int err = 0;
        run_file(argv[1], &err);
    }
    
}

