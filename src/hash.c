#include "hash.h"

size_t hash_arb_add_chained(void* data, size_t length)
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

size_t hash_arb_xor_chained(void* data, size_t length)
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
