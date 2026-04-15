#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <immintrin.h>

#include "type.h"
#include "func.h"
#include "hash_func.h"

/*
    Немного об устройстве хеш-таблицы

    Пока предполагается хранить в таблице строки длинной либо меньше 32, либо 64 символов.
    Более длинные пока не обрабатывать, в дальнейшем хранить их в массивах.

    У каждой строки есть 2 различных хеша:
        Хеш ячейки, по которому происходит адресация в таблице
        Хеш списка, по которому идет сравнение в списках

    У хеша списка для ускорения сравнения будет одна особенность.
    Один старший байт (из 8 возможных) будет выделен для хранения длины строки.
    Благодаря этому можно производить быстрое сравнение строк длинной до 31/63 байтов,
    используя векторные инструкции.

    Соответственно при сравнениях этот байт использовать будет нельзя.

    Важно!  В хеш функции не должно быть вычисления длины, по крайней мере ради заполнения байта длины.
    При сравнении этот бит отбрасывается.

    Для ускорения реализации в условиях отсутствия поддержки AVX_512 максимальная длина поддерживаемой строчки - 31 байт (+ '\0').
*/

constexpr int SUPPORTED_SIZE_STRING = 31;
constexpr KDS_Hash HASH_LIST_LEN_MASK = (KDS_Hash) 0xFF << ((sizeof(KDS_Hash) - 1) * 8);
constexpr KDS_Hash HASH_LIST_HASH_MASK = ~HASH_LIST_LEN_MASK;

extern "C" int KDS_HM_CmpString(const char *str_from_hm, const char *out_string, uint8_t len_hm);

// ====================================================================
static inline uint8_t hl_get_len(const KDS_Hash hash) {
    return (uint8_t) ((hash & HASH_LIST_LEN_MASK) >> ((sizeof(KDS_Hash) - 1) * 8));
}

static inline KDS_Hash hl_get_hash(const KDS_Hash hash) {
    return hash & HASH_LIST_HASH_MASK;
}

static inline KDS_Hash hl_set_len(const KDS_Hash in_hash, const uint8_t len) {
    return ((KDS_Hash) len) << ((sizeof(KDS_Hash) - 1) * 8) | hl_get_hash(in_hash);
}


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
static int kds_hm_verifier_list(KDS_HashMapList *list, int size);
#endif /* VERIFIER */


// constexpr unsigned char POLINOM =  0x1D;

// ====================================================================
// HASH FUNCTIONS
KDS_Hash own_cell_hash(const char *string) {
    assert(string);

    KDS_Hash hash = 0x02B2AE3D27D4EB4Fll;
    uint64_t idx = 0;
    // KDS_Hash tmp = 0;

    while (string[idx] != '\0') {
        // asm volatile(
        //     ".intel_syntax noprefix\n\t"
        //     "mov %[t], %[h]\n\t"
        //     "shl %[h], 7\n\t"
        //     "add %[h], %[t]\n\t"
        //     ".att_syntax prefix\n\t"
        //     : [h] "+r"(hash), [t] "=&r"(tmp)
        //     :
        //     : "cc"
        // );
        hash *= 129;
        hash += (KDS_Hash) string[idx++];
    }

    return hash;
}
// --------------------------------------------------------------------
KDS_Hash own_list_hash(const char *string) {
    assert(string);

// /*
// это самая быстрая хеi-функция
    KDS_Hash hash = 0x082EFA98EC4E6C89ul;
    int idx = 0;

    while (string[idx] != '\0') {
        hash *= 33;
        hash += (KDS_Hash) string[idx++];
    }
// */

/*
    alignas(32) uint8_t buf[32] = {};

    int n = 0;
    while (n < 32 && string[n] != '\0') {
        buf[n] = (uint8_t)string[n];
        n++;
    }

    __m256i str_v = _mm256_load_si256((const __m256i*)buf);

    __m256i mixer_1 = _mm256_set1_epi64x(0x082EFA98EC4E6C89ul);
    __m256i mixer_2 = _mm256_set1_epi64x(0x02B2AE3D27D4EB4Ful);

    str_v = _mm256_xor_si256(str_v, mixer_1);
    str_v = _mm256_add_epi8(str_v, mixer_2);

    str_v = _mm256_shuffle_epi32(str_v, _MM_SHUFFLE(2, 3, 0, 1));
    int mask_1 = _mm256_movemask_epi8(str_v);

    str_v = _mm256_mul_epu32(str_v, mixer_1);
    str_v = _mm256_xor_si256(str_v, mixer_2);

    int mask_2 = _mm256_movemask_epi8(str_v);

    constexpr int ROT = 32;
    constexpr int BITS = sizeof(KDS_Hash) * 8;

    KDS_Hash hash = (KDS_Hash) 0xA4093822299F31D0ULL ^ (KDS_Hash) mask_1;
    hash = (hash >> ROT) | (hash << (BITS - ROT));
    hash ^= hash ^ (KDS_Hash) mask_2;
    hash *= 0xc4ceb9fe1a85ec53ul;
*/

    return hash;

//
//     KDS_Hash hash = 0xA4093822299F31D0ul;
//     constexpr KDS_Hash len_7 = (sizeof(KDS_Hash) * 8) - 7;
//
//     for (int i = 0; string[i] != '\0'; i++) {
//         char sym = string[i];
//         sym ^= 0xEF;
//         sym += 0xB2;
//         sym ^= 0xAF;
//         hash ^= (uint64_t) sym;
//         hash = (hash >> 7) | (hash << len_7);
//     }
//
//     return hash;
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

    /* Get All Hash */
    KDS_Hash hash_cell = kds_hm_get_cell_hash(string);
    KDS_Hash hash_list = kds_hm_get_list_hash(string);

    KDS_HashMapList *list = &(map->data[hash_cell % (KDS_Hash) map->size]);
    int error = -1;

    /* Check Len */
    size_t len_string = strlen(string);
    if (len_string > SUPPORTED_SIZE_STRING) {
        ExitF("Too Big String", -1);
    }
    hash_list = hl_get_hash(hash_list);
    KDS_Hash hash_list_with_len = hl_set_len(hash_list, (uint8_t) len_string);

    /* If it first string in sell */
    if (list->string == NULL) {
        list->hash_list = hash_list_with_len;
        list->string = strdup(string);
        list->counter = 1;
        list->next = NULL;
        return 0;
    }

    while (true) {
        if (hl_get_hash(list->hash_list) == hash_list && strcmp(list->string, string) == 0) {
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

        list->hash_list = hash_list_with_len;
        list->counter = 1;
        list->string = strdup(string);

        if (list->string == NULL)   ExitF("NULL strdup", -1);
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
KDS_HashMapList *KDS_HM_FindString32(KDS_HashMap *map, const char *string) {
    assert(map);
    assert(string);

#ifdef VERIFIER
    if (kds_hm_verifier(map) != 0)  ExitF("Incorrect Hash Map", NULL);
#endif /* VERIFIER */

    KDS_Hash hash_cell = kds_hm_get_cell_hash(string);
    KDS_Hash hash_list = kds_hm_get_list_hash(string);
    // uint8_t len = hl_get_len(hash_list);
    hash_list = hl_get_hash(hash_list);

    KDS_HashMapList *list = &(map->data[hash_cell % (KDS_Hash) map->size]);
    KDS_HashMapList *value = NULL;
    if (list->string == NULL)   return value;

    while (true) {
        uint8_t len = hl_get_len(list->hash_list);
        if (hl_get_hash(list->hash_list) == hash_list && KDS_HM_CmpString(list->string, string, len) == 0) {
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
        // printf("I=%d\n", i);
        KDS_HashMapList *list = &(map->data[i]);
        if (list->next) {
            kds_hm_destroy_list(list->next);
        }
        if (list->string)   free(list->string);
    }
    free(map->data);

    return 0;
}

// ====================================================================
// HELPER FUNCTIONS
static void kds_hm_destroy_list(KDS_HashMapList *list) {
    assert(list);

    if (list->next) kds_hm_destroy_list(list->next);

    // printf("STR: %p\nLIST: %p\n", list->string, list);
    if (list->string) {
        free(list->string);
    }
    free(list);

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
        KDS_Hash hash_c = kds_hm_get_cell_hash(list->string);
        KDS_Hash hash_l = kds_hm_get_list_hash(list->string);
        hash_l = hl_get_hash(hash_l);

        if (hash_l != hl_get_hash(list->hash_list)) {
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
