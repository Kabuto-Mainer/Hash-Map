#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

size_t kds_h_get_file_size(const char *name);
char *kds_h_create_file_buffer(const char *name, size_t size);

#endif /* COMMON_H */
