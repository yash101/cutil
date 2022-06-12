#ifndef _CUTIL_HASH_H
#define _CUTIL_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

size_t hash_arb_add_chained(void* data, size_t length);
size_t hash_arb_xor_chained(void* data, size_t length);

#ifdef __cplusplus
}
#endif

#endif
