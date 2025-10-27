#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

char* read_file(const char* filename, int* err) 
{
    FILE *f = fopen(filename, "rb"); // open in binary mode
    if (!f) {
        fprintf(stderr, "File Error: no file named %s\n", filename);
        *err = 1;
        return NULL;
    }

    // Move to end to find file size
    if (fseek(f, 0, SEEK_END) != 0) {
        fprintf(stderr, "File Error: unable to seek to end of file %s\n", filename);
        fclose(f);
        *err = 1;
        return NULL;
    }

    long filesize = ftell(f);
    if (filesize < 0) {
        fprintf(stderr, "File Error: unable to determine size of file %s\n", filename);
        fclose(f);
        *err = 1;
        return NULL;
    }
    rewind(f);

    // Allocate buffer
    char *buffer = malloc(filesize + 1); // +1 for null terminator
    if (!buffer) {
        fprintf(stderr, "Memory Error: unable to allocate %ld bytes for file %s\n", filesize + 1, filename);
        fclose(f);
        *err = 1;
        return NULL;
    }

    // Read file
    size_t read_bytes = fread(buffer, 1, filesize, f);
    if (read_bytes != (size_t)filesize) {
        fprintf(stderr, "File Error: failed to read file %s completely (read %zu of %ld bytes)\n",
                filename, read_bytes, filesize);
        free(buffer);
        fclose(f);
        *err = 1;
        return NULL;
    }

    buffer[filesize] = '\0'; // null-terminate
    fclose(f);
    return buffer;
}

int write_file(const char *filename, const char *buffer, size_t size) {
    FILE *f = fopen(filename, "wb"); // open in binary mode
    if (!f) {
        fprintf(stderr, "File Error: unable to create file\n");
        return -1;
    }

    size_t written = fwrite(buffer, 1, size, f);
    if (written != size) {
        fprintf(stderr, "File Error: failed to write to file %s\n", filename);
        fclose(f);
        return -1;
    }

    fclose(f);
    return 0;
}
