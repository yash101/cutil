#include "cutil.h"
#include "hmap.h"
#include "hash.h"

#include <stdlib.h>

typedef struct hmap_node
{
  struct cutil_hmap_key_t key;
  void* data;
  struct hmap_node* next;
} hmap_node;

typedef struct hmap_bucket
{
  struct hmap_node* start;
} hmap_bucket;

void cutil_hmap_init(struct cutil_hmap_t* map)
{
  if (!map)
    return;

  map->minBuckets = 16;
  map->hashFn = cutil_hash_arb_xor_chained;
  map->size = 0;
  map->buckets = map->minBuckets;
  map->loadFactorMin = 0.50;
  map->loadFactorMax = 0.75;
  map->destuctor = NULL;
  map->mapData = malloc(sizeof(struct hmap_bucket) * map->buckets);

  if (!map->mapData)
  {
    map->buckets = 0;
  }
}

/** /todo actually implement!
 */
void cutil_hmap_destroy(struct cutil_hmap_t* map)
{
  if (!map)
    return;
  
  struct hmap_bucket* buckets = (struct hmap_bucket*) map->mapData;
  for (size_t i = 0; i < map->buckets; i++)
  {
    // free the bucket
    struct hmap_node* n = buckets[i].start;
    while (n)
    {
      struct cutil_hmap_tuple_t t = cutil_hmap_make_tuple(n->key, n->data);
      if (map->destuctor)
        map->destuctor(&t);
      struct hmap_node* tmp = n;
      n = n->next;
      free(tmp);
    }
  }
  
  free(buckets);

  map->minBuckets = 0;
  map->hashFn = NULL;
  map->size = 0;
  map->buckets = map->minBuckets;
  map->loadFactorMax = 0.f;
  map->loadFactorMin = 0.f;
  map->destuctor = NULL;
  map->mapData = NULL;
}

void cutil_hmap_set_destructor(struct cutil_hmap_t* map, cutil_destructor_func_t dest)
{
  if (map)
    map->destuctor = dest;
}

void cutil_hmap_set_hash(struct cutil_hmap_t* map, cutil_hash_func_t hash_fn)
{
  if (map)
    map->hashFn = hash_fn;
}

void cutil_hmap_set_loadfactor(struct cutil_hmap_t* map, float min, float max)
{
  if (map && min < max)
  {
    map->loadFactorMin = min;
    map->loadFactorMax = max;
  }
}

void cutil_hmap_set_min_buckets(struct cutil_hmap_t* map, size_t min)
{
  if (map)
    map->minBuckets = min;
}

int cutil_hmap_probe_hashfn(struct cutil_hmap_t* map, struct cutil_hmap_key_t key)
{
  if (!map)
    return 0;
  
  size_t hash = map->hashFn(key.key, key.len) % map->buckets;
  struct hmap_bucket* buckets = map->mapData;

  return (buckets[hash].start != NULL) ? 1 : 0;
}

int cutil_hmap_probe_key(struct cutil_hmap_t* map, struct cutil_hmap_key_t key)
{
  if (!map)
    return 0;
  
  size_t hash = map->hashFn(key.key, key.len) % map->buckets;
  struct hmap_bucket* buckets = map->mapData;

  if (!buckets[hash].start)
    return 0;
  
  struct hmap_node* n = buckets[hash].start;
  int comp;
  while (n && (comp = map->compareFn(key.key, n->key.key, key.len, n->key.len)) != CUTIL_EQ)
    n = n->next;
  
  return (comp == CUTIL_EQ) ? 1 : 0;
}

static int cutil_hmap_rebucket(struct cutil_hmap_t* map, size_t buckets)
{
  // invalid action
  if (!map || map->buckets == buckets || buckets == 0)
    return 0;
  
  struct hmap_bucket* fut = malloc(buckets * sizeof(*fut));
  struct hmap_bucket* cur = (struct hmap_bucket*) map->mapData;
  if (!fut)
    return 0;

  for (size_t i = 0; i < buckets; i++)
  {
    fut[i].start = NULL;
  }

  for (size_t i = 0; i < map->buckets; i++)
  {
    struct hmap_bucket ths = cur[i];
    struct hmap_node* n = ths.start;
    struct hmap_node* tmp = NULL;
    while (n)
    {
      // rehash this node
      size_t hash = map->hashFn(n->key.key, n->key.len) % buckets;
      tmp = n->next;
      n->next = fut[hash].start;
      fut[hash].start = n;
      n = tmp;
    }
  }

  map->mapData = cur;
  map->buckets = buckets;
  free(cur);
}

int cutil_hmap_insert(struct cutil_hmap_t* map, struct cutil_hmap_tuple_t t, int allowDuplicates)
{
  if (!map)
    return 0;
  
  size_t hash = map->hashFn(t.key.key, t.key.len) % map->buckets;
  struct hmap_bucket* buckets = map->mapData;

  if (allowDuplicates)
  {
    struct hmap_node* n = malloc(sizeof *n);
    if (!n)
      return 0;
    
    n->data = t.value;
    n->key = t.key;
    n->next = buckets[hash].start;
    buckets[hash].start = n;
    
    map->size++;
    return 1;
  }

  // find the bucket
  struct hmap_bucket* bucket = &buckets[hash];
  // check to see if the hash is there
  struct hmap_node* tmp = bucket->start;
  int comp = CUTIL_GT;
  while (tmp && (comp = map->compareFn(tmp->key.key, t.key.key, tmp->key.len, t.key.len)) != CUTIL_EQ)
    tmp = tmp->next;
  
  // check if the node already exists
  if (tmp && comp == CUTIL_GT)
    return 0;
  
  struct hmap_node* n = malloc(sizeof *n);
  if (!n)
    return 0;
  
  n->data = t.value;
  n->key = t.key;
  n->next = bucket->start;
  bucket->start = n;

  map->size++;
  return 1;
}

void** cutil_hmap_get(struct cutil_hmap_t* map, struct cutil_hmap_key_t key)
{
  if (!map)
    return NULL;
  
  size_t hash = map->hashFn(key.key, key.len) % map->buckets;

  struct hmap_bucket* buckets = (struct hmap_bucket*) map->mapData;
  
  struct hmap_node* tmp = buckets[hash].start;
  int comp = CUTIL_GT;
  while (tmp && (comp = map->compareFn(key.key, tmp->key.key, key.len, tmp->key.len)) != CUTIL_EQ)
    tmp = tmp->next;
  
  if (!tmp || comp != CUTIL_EQ)
    return NULL;
  
  return &tmp->data;
}

int cutil_hmap_del(struct cutil_hmap_t* map, struct cutil_hmap_key_t key, int checkDuplicates)
{
  if (!map)
    return 0;
  
  size_t hash = map->hashFn(key.key, key.len) % map->buckets;
  struct hmap_bucket* buckets = (struct hmap_bucket*) map->mapData;
  struct hmap_node* node = buckets[hash].start;
  struct hmap_node* prev = NULL;
  if (!node)
    return 0;
  
  int deleted = 0;
  
  while (node)
  {
    int comp = map->compareFn(key.key, node->key.key, key.len, node->key.len);
    if (comp == CUTIL_EQ)
    {
      struct hmap_node* tmp = node;
      if (prev)
        prev->next = tmp->next;
      else
        buckets[hash].start = tmp->next;
      
      struct cutil_hmap_tuple_t tup;
      tup.key = tmp->key;
      tup.value = tmp->data;

      if (map->destuctor)
        map->destuctor((void*) &tup);

      free(tmp);
      deleted++;

      if (!checkDuplicates)
        break;
    }

    prev = node;
    node = node->next;
  }

  map->buckets -= (size_t) deleted;
  return deleted;
}