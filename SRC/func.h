#ifndef FUNC_H
#define FUNC_H

#include "type.h"

// ====================================================================
// USED MACROS
// ====================================================================

// --------------------------------------------------------------------
/**
 * @brief Macros Error Handler
 *
 */
#define ExitF(__text__, __ret_val__) \
do {printf("ERROR in %s:%d||%s\n", __FILE__,__LINE__,__text__); \
    return __ret_val__; } while (0)

// --------------------------------------------------------------------
/**
 * @brief Create Hash Map Object
 *
 * @param map Pointer to Hash Map
 * @param size Size of Hash Map
 * @return int 0 on success
 * @return int -1 on error
 */
int KDS_HM_Create(KDS_HashMap *map, int size);
// --------------------------------------------------------------------
/**
 * @brief Add String to Hash Map
 *
 * @param map Pointer to Has Map
 * @param string Adding String
 * @return int 0 on success
 * @return int -1 on error
 */
int KDS_HM_AddString(KDS_HashMap *map, const char *string);
// --------------------------------------------------------------------
/**
 * @brief Find String in Hash Map
 *
 * @param map Pointer to Hash Map
 * @param string Finding String
 * @return KDS_HashMapList* Pointer to List with needed String on success
 * @return KDS_HashMapList* NULL on error
 */
// --------------------------------------------------------------------
KDS_HashMapList *KDS_HM_FindString(KDS_HashMap *map, const char *string);
/**
 * @brief Destroy Hash Map
 *
 * @param map Pointer to Hash Map
 * @return int 0 on success
 */
int KDS_HM_Destroy(KDS_HashMap *map);

int KDS_HM_LoadData(KDS_HashMap *map, KDS_TextContext *cont);
int KDS_HM_DumpText(KDS_HashMap *map, const char *file);
int KDS_HM_DumpDat(KDS_HashMap *map, const char *file);

int KDS_TC_Create(KDS_TextContext *cont, const char *file);
int KDS_TC_Destroy(KDS_TextContext *cont);
int KDS_TC_SetWord(KDS_TextContext *cont, int pose);
char *KDS_TC_GetNextWord(KDS_TextContext *cont);

// ====================================================================
// HASH FUNCTIONS DECLARATION
KDS_Hash kds_hm_get_cell_hash(const char *string);
KDS_Hash kds_hm_get_list_hash(const char *string);



#endif /* FUNC_H */
