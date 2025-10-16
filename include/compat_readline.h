#ifndef COMPAT_READLINE_H
#define COMPAT_READLINE_H

#ifdef _WIN32
    // -------- Windows fallback implementation --------
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    static inline char* readline(const char* prompt) {
        static char buffer[1024];

        if (prompt) {
            fputs(prompt, stdout);
            fflush(stdout);
        }

        if (!fgets(buffer, sizeof(buffer), stdin))
            return NULL;

        buffer[strcspn(buffer, "\n")] = '\0';
        return strdup(buffer);  // mimic GNU readline behavior
    }

    static inline void add_history(const char* line) {
        (void)line; // no-op on Windows
    }

    static inline void read_history(const char* filename) {
        (void)filename; // no history persistence
    }

    static inline void write_history(const char* filename) {
        (void)filename; // no history persistence
    }

#else
    // -------- Linux/macOS: use GNU Readline --------
    #include <readline/readline.h>
    #include <readline/history.h>
#endif

#endif // COMPAT_READLINE_H
