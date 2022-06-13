#include "hash.h"
#include "cutil.h"

size_t cutil_hash_arb_add_chained(void* data, size_t length)
{
  size_t hash = 0;
  size_t* view = data;
  for (size_t i = 0; i < length / sizeof(size_t); i++)
    hash += view[i];

  size_t rem = length % sizeof(size_t);
  char* rem_view = (char*) data;
  size_t last = 0;
  for (size_t i = 0; i < rem; i++)
    last |= rem_view[i] << (i * 8);
  
  return hash + last;
}

size_t cutil_hash_arb_xor_chained(void* data, size_t length)
{
  size_t hash = 0;
  size_t* view = data;
  for (size_t i = 0; i < length / sizeof(size_t); i++)
    hash ^= view[i];
  
  size_t rem = length % sizeof(size_t);
  char* rem_view = (char*) data;
  size_t last = 0;
  for (size_t i = 0; i < rem; i++)
    last |= rem_view[i] << (i * 8);
  
  return hash ^ last;
}

int cutil_compare_lex(void* data, void* data2, size_t len1, size_t len2)
{
  char* a = (char*) data;
  char* b = (char*) data2;

  for (size_t i = 0; i < (len1 < len2 ? len1 : len2); i++)
  {
    if (a[i] == b[i])
      continue;
    return (a[i] < b[i]) ? CUTIL_LT : CUTIL_GT;
  }

  return (len1 == len2) ? CUTIL_EQ : (len1 < len2) ? CUTIL_LT : CUTIL_GT;
}
