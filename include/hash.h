#ifndef _CUTIL_HASH_H
#define _CUTIL_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef size_t (*cutil_hash_func_t)(void* data, size_t length);

size_t cutil_hash_arb_add_chained(void* data, size_t length);
size_t cutil_hash_arb_xor_chained(void* data, size_t length);

typedef int (*cutil_compare_func_t)(void* d0, void* d1, size_t l1, size_t l2);
int cutil_compare_lex(void* data, void* data2, size_t len1, size_t len2);

#ifdef __cplusplus
}
#endif

#endif
