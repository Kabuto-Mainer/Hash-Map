#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "type.h"
#include "func.h"
#include "hash_func.h"


// ====================================================================
//  HELPER FUNCTIONS DECLARATION
static void kds_hm_destroy_list(KDS_HashMapList *list);
static KDS_HashMapList *kds_hm_allocate_list(void);


#ifdef VERIFIER
static int kds_hm_verifier(KDS_HashMap *map);

// --------------------------------------------------------------------
/**
 * @brief Verifier List in Hash Map
 *
 * @param list Pointer to checking List
 * @param hash_list Pointer to function 'get_hash_list'
 * @param hash_cell Pointer to function 'get_hash_cell'
 * @param size Size of Hash Map
 * @return int Value with bits from KDS_HashMapErrorFlag
 */
static int kds_hm_verifier_list(KDS_HashMapList *list, KDS_Hash (*hash_list)(const char *),
            KDS_Hash (*hash_cell)(const char *), int size);
#endif /* VERIFIER */



// ====================================================================
// HASH FUNCTIONS
KDS_Hash own_cell_hash(const char *string) {
    assert(string);

    KDS_Hash hash = (KDS_Hash) string[0];
    int idx = 0;

    constexpr KDS_Hash len = (sizeof(KDS_Hash) * 8) - 1;
    while (string[idx] != '\0') {
        hash ^= hash >> 33;
        hash = (hash << 1) | (hash >> len);
        hash *= 0xff51afd7ed588ccd;
        // hash = (hash >> 1) | (hash << len);
        hash ^= (KDS_Hash) string[idx++];
    }

    return hash;
}
// --------------------------------------------------------------------
KDS_Hash own_list_hash(const char *string) {
    assert(string);

    KDS_Hash hash = 5137;
    size_t len = strlen(string);

    for (size_t i = 0; i < len; i++) {
        hash += (KDS_Hash) string[i];
        hash *= 33;
    }

    return hash;
}


// ====================================================================
// API FUNCTIONS

// --------------------------------------------------------------------
/**
 * @brief Create Hash Map Object
 *
 * @param map Pointer to Hash Map
 * @param size Size of Hash Map
 * @return int 0 on success
 * @return int -1 on error
 */
int KDS_HM_Create(KDS_HashMap *map, int size) {
    assert(map);
    if (size <= 0)  ExitF("Bad Address", -1);

    map->data = (KDS_HashMapList *)calloc((size_t) size, sizeof(KDS_HashMapList));
    if (map->data == NULL)  ExitF("NULL Calloc", -1);

    map->size = size;

    return 0;
}

// --------------------------------------------------------------------
/**
 * @brief Add String to Hash Map
 *
 * @param map Pointer to Has Map
 * @param string Adding String
 * @return int 0 on success
 * @return int -1 on error
 */
int KDS_HM_AddString(KDS_HashMap *map, const char *string) {
    assert(map);
    assert(string);

#ifdef VERIFIER
    if (kds_hm_verifier(map) != 0)  ExitF("Incorrect Hash Map", -1);
#endif /* VERIFIER */

    KDS_Hash hash_cell = kds_hm_get_cell_hash(string);
    KDS_Hash hash_list = kds_hm_get_list_hash(string);

    KDS_HashMapList *list = &(map->data[hash_cell % (KDS_Hash) map->size]);
    int error = -1;

    // printf("ADD STRING 2\n");
    if (list->string == NULL) {
        list->hash_list = hash_list;
        list->string = strdup(string);
        list->counter = 1;
        list->next = NULL;
        return 0;
    }

    while (true) {
        if (list->hash_list == hash_list && strcmp(list->string, string) == 0) {
            list->counter++;
            error = 0;
            break;
        }
        if (list->next != NULL) {
            list = list->next;
            continue;
        }
        list->next = kds_hm_allocate_list();
        list = list->next;
        if (list == NULL) {
            error = -1;
            break;
        };

        list->hash_list = hash_list;
        list->counter = 1;
        list->string = strdup(string);
        list->next = NULL;

        error = 0;
        break;
    }

#ifdef VERIFIER
    if (kds_hm_verifier(map) != 0)  ExitF("Incorrect Hash Map", -1);
#endif /* VERIFIER */

    return error;
}

// --------------------------------------------------------------------
/**
 * @brief Find String in Hash Map
 *
 * @param map Pointer to Hash Map
 * @param string Finding String
 * @return KDS_HashMapList* Pointer to List with needed String on success
 * @return KDS_HashMapList* NULL on error
 */
KDS_HashMapList *KDS_HM_FindString(KDS_HashMap *map, const char *string) {
    assert(map);
    assert(string);

#ifdef VERIFIER
    if (kds_hm_verifier(map) != 0)  ExitF("Incorrect Hash Map", NULL);
#endif /* VERIFIER */

    KDS_Hash hash_cell = kds_hm_get_cell_hash(string);
    KDS_Hash hash_list = kds_hm_get_list_hash(string);

    KDS_HashMapList *list = &(map->data[hash_cell % (KDS_Hash) map->size]);
    KDS_HashMapList *value = NULL;

    while (true) {
        if (list->hash_list == hash_list && strcmp(list->string, string) == 0) {
            value = list;
            break;
        }
        if (list->next != NULL) {
            list = list->next;
            continue;
        }
        value = NULL;
        break;
    }
    return value;
}

// --------------------------------------------------------------------
/**
 * @brief Destroy Hash Map
 *
 * @param map Pointer to Hash Map
 * @return int 0 on success
 */
int KDS_HM_Destroy(KDS_HashMap *map) {
    assert(map);

    for (int i = 0; i < map->size; i++) {
        kds_hm_destroy_list(&(map->data[i]));
    }
    free(map->data);

    return 0;
}

// ====================================================================
// HELPER FUNCTIONS
static void kds_hm_destroy_list(KDS_HashMapList *list) {
    assert(list);

    if (list->next != NULL) kds_hm_destroy_list(list->next);

    free(list->string);

    return ;
}

// --------------------------------------------------------------------
static KDS_HashMapList *kds_hm_allocate_list(void) {
    KDS_HashMapList *list = (KDS_HashMapList *)calloc(1, sizeof(KDS_HashMapList));
    if (list == NULL) {
        ExitF("NULL Calloc", NULL);
    }
    return list;
}

#ifdef VERIFIER
// --------------------------------------------------------------------
/**
 * @brief Verifier Hash Map
 *
 * @param map Pointer to Hash Map
 * @return int 0 on success
 * @return int -1 on error
 */
static int kds_hm_verifier(KDS_HashMap *map) {
    assert(map);
    for (int i = 0; i < map->size; i++) {
        int error = kds_hm_verifier_list(&(map->data[i]), map->hash_list, map->hash_cell, map->size);
        if (error != KDS_HM_ERROR_NOT) {
            printf("I+%d\n", i);
            return -1;
        }
    }
    return 0;
}

// --------------------------------------------------------------------
/**
 * @brief Verifier List in Hash Map
 *
 * @param list Pointer to checking List
 * @param hash_list Pointer to function 'get_hash_list'
 * @param hash_cell Pointer to function 'get_hash_cell'
 * @return int Value with bits from KDS_HashMapErrorFlag
 */
static int kds_hm_verifier_list(KDS_HashMapList *list, int size) {
    assert(list);
    assert(size);

    int error = KDS_HM_ERROR_NOT;
    if (list->string == NULL) {
        return error;
    }
    KDS_Hash needed_hash_cell = kds_hm_get_cell_hash(list->string);

    while (true) {
        KDS_Hash hash_l = kds_hm_get_list_hash(list->string);
        KDS_Hash hash_c = kds_hm_get_cell_hash(list->string);

        if (hash_l != list->hash_list) {
            error |= KDS_HM_ERROR_BAD_HASH_LIST;
            // printf("%s\n", list->string);
        }
        if (hash_c % (KDS_Hash) size != needed_hash_cell % (KDS_Hash) size) {
            error |= KDS_HM_ERROR_BAD_HASH_CELL;
            // printf("%s\n", list->string);
        }

        if (list->next != NULL) {
            list = list->next;
            continue;
        }
        break;
    }
    return error;
}
#endif /* VERIFIER */
