#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

char* read_file(const char* filename) 
{
    FILE *f = fopen(filename, "rb"); // open in binary mode
    if (!f) {
        perror("fopen");
        return NULL;
    }

    // Move to end to find file size
    if (fseek(f, 0, SEEK_END) != 0) {
        perror("fseek");
        fclose(f);
        return NULL;
    }

    long filesize = ftell(f);
    if (filesize < 0) {
        perror("ftell");
        fclose(f);
        return NULL;
    }
    rewind(f);

    // Allocate buffer
    char *buffer = malloc(filesize + 1); // +1 for null terminator
    if (!buffer) {
        perror("malloc");
        fclose(f);
        return NULL;
    }

    // Read file
    size_t read_bytes = fread(buffer, 1, filesize, f);
    if (read_bytes != (size_t)filesize) {
        perror("fread");
        free(buffer);
        fclose(f);
        return NULL;
    }

    buffer[filesize] = '\0'; // null-terminate
    fclose(f);
    return buffer;
}

int write_file(const char *filename, const char *buffer, size_t size) {
    FILE *f = fopen(filename, "wb"); // open in binary mode
    if (!f) {
        perror("fopen");
        return -1;
    }

    size_t written = fwrite(buffer, 1, size, f);
    if (written != size) {
        perror("fwrite");
        fclose(f);
        return -1;
    }

    fclose(f);
    return 0;
}
