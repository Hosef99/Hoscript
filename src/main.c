#include "eval.h"

int main(int argc, char* argv[])
{
    if (argc == 1) {
        start_repl();
    }
<<<<<<< HEAD
    else {
        int err = 0;
        run_file(argv[1], &err);
    }
    
||||||| parent of ad60aa4 (merged)

    write_history(history_file);

    printf("\nExiting Hoscript.\n");
    return 0;
=======
    else {

    }
    
>>>>>>> ad60aa4 (merged)
}

