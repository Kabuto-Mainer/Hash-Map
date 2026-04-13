#ifndef TYPE_H
#define TYPE_H

#include <stdint.h>

typedef uint64_t KDS_Hash;
typedef uint64_t KDS_Count;

#define KDS_HASH_PRINT_SPEC "%lu"
#define KDS_COUNT_PRINT_SPEC "%lu"

extern const int SIZE_STRING;

enum KDS_HashMapErrorFlag {
    KDS_HM_ERROR_NOT = 0x0000,
    KDS_HM_ERROR_BAD_HASH_CELL = 0x0001,
    KDS_HM_ERROR_BAD_HASH_LIST = 0x0002,
};

struct KDS_HashMapList {
    KDS_Hash hash_list;
    char string;

    KDS_Count counter;
    KDS_HashMapList *next;
};

struct KDS_HashMap {
    int size;   /* Size of map */

    KDS_HashMapList *data;  /* Data hash map */

    KDS_Hash (*hash_cell)(const char *);  /* Hash function for get cell */
    KDS_Hash (*hash_list)(const char *);  /* Hash function for compare strings in list */
};

struct KDS_TextContext {
    struct {
        char **list;
        int size;
        int capacity;
        int pose;
    } ptr;
    struct {
        char *buffer;
        int size;
        int capacity;
        int pose;
    } sym;
};


#endif /* TYPE_H */
