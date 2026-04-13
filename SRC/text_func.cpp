#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>

#include "func.h"
#include "type.h"
#include "common.h"

// ====================================================================
//  HELPER FUNCTIONS DECLARATION
static void kds_tc_delete_syntax(KDS_TextContext *cont);
static void kds_tc_add_ptr(KDS_TextContext *cont, char *pointer);

int KDS_HM_LoadData(KDS_HashMap *map, KDS_TextContext *cont) {
    assert(map);
    assert(cont);

    // printf("Step 3\n");

    char *string = NULL;
    KDS_TC_SetWord(cont, 0);
    while ((string = KDS_TC_GetNextWord(cont)) != NULL) {
        // printf("Step 4\n");
        // printf("String: %s\n", string);
        KDS_HM_AddString(map, string);
    }

    return 0;
}

int KDS_HM_DumpText(KDS_HashMap *map, const char *file) {
    assert(map);
    assert(file);

    FILE *stream = fopen(file, "wb");
    if (stream == NULL) ExitF("NULL File", -1);

    for (int i = 0; i < map->size; i++) {
        fprintf(stream, "[%5d]", i);

        KDS_HashMapList *list = &(map->data[i]);
        while (true) {
            if (list->string == NULL) {
                fprintf(stream, "nil");
            } else {
                fprintf(stream, "[\"%s\"{" KDS_COUNT_PRINT_SPEC
                    "}, C:" KDS_HASH_PRINT_SPEC ", L:" KDS_HASH_PRINT_SPEC "]",
                    list->string, list->counter, map->hash_cell(list->string), list->hash_list);
            }

            if (list->next != NULL) {
                fprintf(stream, "->");
                list = list->next;
                continue;
            }
            fprintf(stream, ";\n");
            break;
        }
    }
    fclose(stream);
    return 0;
}

int KDS_HM_DumpDat(KDS_HashMap *map, const char *file) {
    assert(map);
    assert(file);

    FILE *stream = fopen(file, "wb");
    if (stream == NULL) ExitF("NULL File", -1);

    for (int i = 0; i < map->size; i++) {
        fprintf(stream, "%d", i);

        KDS_HashMapList *list = &(map->data[i]);
        int counter = 0;

        while (true) {
            if (list->string == NULL) {
                counter = 0;
                break;
            }
            if (list->next != NULL) {
                list = list->next;
                counter++;
                continue;
            }
            break;
        }
        fprintf(stream, " %d\n", counter);
    }

    fclose(stream);
    return 0;
}



int KDS_TC_Create(KDS_TextContext *cont, const char *file) {
    assert(cont);
    assert(file);

    size_t size = kds_h_get_file_size(file);
    char *buffer = kds_h_create_file_buffer(file, size);

    cont->sym.buffer = buffer;
    cont->sym.size = (int) size;
    cont->sym.capacity = (int) size;
    cont->sym.pose = 0;
    cont->ptr.capacity = 0;
    cont->ptr.pose = 0;
    cont->ptr.size = 0;
    cont->ptr.list = NULL;

    kds_tc_delete_syntax(cont);
    return 0;
}

int KDS_TC_SetWord(KDS_TextContext *cont, int pose) {
    assert(cont);
    if (pose >= cont->ptr.size)     ExitF("Bad Pose", -1);

    cont->ptr.pose = pose;
    return 0;
}

int KDS_TC_Destroy(KDS_TextContext *cont) {
    assert(cont);

    free(cont->sym.buffer);
    if (cont->ptr.list != NULL)     free(cont->ptr.list);

    return 0;
}

static void kds_tc_add_ptr(KDS_TextContext *cont, char *pointer) {
    assert(cont);
    assert(pointer);

    if (cont->ptr.list == NULL) {
        cont->ptr.list = (char **)calloc(10, sizeof(char *));
        if (cont->ptr.list == NULL)     ExitF("NULL Calloc", );
        cont->ptr.capacity = 10;
        cont->ptr.size = 0;
        cont->ptr.pose = 0;
    } else if (cont->ptr.capacity == cont->ptr.size) {
        cont->ptr.list = (char **) realloc(cont->ptr.list, (size_t) cont->ptr.capacity * 2 * sizeof(char *));
        if (cont->ptr.list == NULL)     ExitF("NULL Calloc", );
        cont->ptr.capacity *= 2;
    }

    cont->ptr.list[cont->ptr.size++] = pointer;
    return ;
}

static void kds_tc_delete_syntax(KDS_TextContext *cont) {
    assert(cont);

    char *buffer = cont->sym.buffer;
    int new_p = 0;
    int old_p = 0;
    bool added_space = false;

    while (buffer[old_p] != '\0') {
        char sym = buffer[old_p++];
        if ((sym >= 'a' && sym <= 'z') ||
            (sym >= 'A' && sym <= 'Z') ||
            (sym >= '0' && sym <= '9') ||
            sym == '-' || sym == '\'') {
            if (added_space) {
                kds_tc_add_ptr(cont, buffer + new_p);
                added_space = false;
            }
                buffer[new_p++] = sym;
                continue;
        }
        if (added_space == false) {
            buffer[new_p++] = '\0';
            added_space = true;
        }
    }

    buffer[new_p] = '\0';
    cont->sym.size = new_p;
    cont->sym.pose = 0;

    return ;
}

char *KDS_TC_GetNextWord(KDS_TextContext *cont) {
    assert(cont);

    if (cont->ptr.pose < cont->ptr.size) {
        return cont->ptr.list[cont->ptr.pose++];
    }
    return NULL;
}
