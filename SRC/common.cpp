#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"
#include "func.h"

// ------------------------------------------------------------------
size_t kds_h_get_file_size(const char *name) {
    assert(name);

    struct stat file_stat = {};
    if (stat(name, &file_stat) == -1)   ExitF("Bad Stat", 0);

    return (size_t) file_stat.st_size;
}

// ------------------------------------------------------------------
char *kds_h_create_file_buffer(const char *name, size_t size) {
    assert(name);

    FILE *stream = fopen(name, "rb");
    if (stream == NULL) ExitF("NULL File", NULL);

    char *buffer = (char *)calloc(size + 1, sizeof(char));
    if (buffer == NULL) {
        fclose(stream);
        ExitF("NULL Calloc", NULL);
    }

    fread(buffer, sizeof(char), size, stream);
    fclose(stream);

    return buffer;
}
