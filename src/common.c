#include <stdio.h>
#include <errno.h>

#include "SDL_log.h"

char *readFileToString(const char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to open '%s', error: %s", path, strerror(errno));
        exit(1);
    }

    // Determine file size
    fseek(file, 0, SEEK_END);
    size_t stringSize = (size_t) ftell(file);
    rewind(file);

    // Allocate buffer and read file into it
    char *buffer = (char *) calloc(stringSize + 1, sizeof(char));
    size_t readSize = fread(buffer, sizeof(char), stringSize, file);
    fclose(file);

    // Validate that the correct amount of data was read
    if (stringSize != readSize) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to read file '%s'", path);
        free(buffer);
        exit(1);
    }

    return buffer;
}
