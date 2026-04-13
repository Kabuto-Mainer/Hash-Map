#ifndef HASH_FUNC_H
#define HASH_FUNC_H

#include "type.h"
#include "func.h"

NOT_INLINE KDS_Hash own_cell_hash(const char *string);
NOT_INLINE KDS_Hash own_list_hash(const char *string);

constexpr KDS_Hash (* kds_hm_get_cell_hash)(const char *string) = &own_cell_hash;
constexpr KDS_Hash (* kds_hm_get_list_hash)(const char *string) = &own_list_hash;


#endif /* HASH_FUNC_H */
