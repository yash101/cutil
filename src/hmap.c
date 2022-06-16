#include "cutil.h"
#include "hmap.h"
#include "hash.h"

#include <stdlib.h>
#include <stdio.h>

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

static int cutil_hmap_rebucket(struct cutil_hmap_t* map)
{
  // invalid action
  if (!map)
    return 0;

  size_t target_buckets = map->buckets;
  float lf = (float) map->size / (float) (map->buckets + 1);
  if (lf > map->loadFactorMax)
    target_buckets = map->size * map->loadFactorMax;
  if (lf < map->loadFactorMin)
    target_buckets = map->size * map->loadFactorMin;
  
  if (target_buckets < map->minBuckets)
    target_buckets = map->minBuckets;

  if (target_buckets == map->buckets)
    return 1;

  size_t current_bkts = map->buckets;
  struct hmap_bucket* current = (struct hmap_bucket*) map->mapData;
  struct hmap_bucket* repl = (struct hmap_bucket*) malloc(sizeof(*repl) * target_buckets);
  if (!repl)
    return 0;

  // initialize all new buckets
  for (size_t i = 0; i < target_buckets; i++)
  {
    repl[i].start = NULL;
  }

  map->buckets = target_buckets;
  map->mapData = (void*) repl;
  if (!current)
    return 1;
  
  for (size_t i = 0; i < current_bkts; i++)
  {
    struct hmap_node* n = current[i].start;
    while (n)
    {
      // rehash and move to new bucket
      size_t hash_nw = map->hashFn(n->key.key, n->key.len) % target_buckets;
      struct hmap_node* tmp_next = n->next;
      current[i].start = tmp_next;
      n->next = repl[hash_nw].start;
      repl[hash_nw].start = n;
      n = tmp_next;
    }
  }

  free(current);
//  printf("Rebucket: %ld -> %ld\n", buckets_start, map->buckets);

  return 1;
}

void cutil_hmap_init(struct cutil_hmap_t* map)
{
  if (!map)
    return;

  map->mapData = NULL;
  map->minBuckets = 16;
  map->size = 0;
  map->loadFactorMin = 0.50;
  map->loadFactorMax = 0.75;
  map->hashFn = cutil_hash_arb_xor_chained;
  map->compareFn = cutil_compare_lex;
  map->destuctor = NULL;

  cutil_hmap_rebucket(map);

  if (!map->mapData)
  {
    map->buckets = 0;
  }
}

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
      struct hmap_node* cur = n;
      n = n->next;

      struct cutil_hmap_tuple_t t = cutil_hmap_make_tuple(cur->key, cur->data);
      if (map->destuctor)
        map->destuctor(&t);
      
      free(cur);
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
  map->compareFn = NULL;
}

void cutil_hmap_set_destructor(struct cutil_hmap_t* map, cutil_destructor_func_t dest)
{
  if (map)
    map->destuctor = dest;
}

void cutil_hmap_set_hashfn(struct cutil_hmap_t* map, cutil_hash_func_t hash_fn)
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
    cutil_hmap_rebucket(map);
  }
}

void cutil_hmap_set_min_buckets(struct cutil_hmap_t* map, size_t min)
{
  if (map)
    map->minBuckets = min;
  cutil_hmap_rebucket(map);
}

size_t cutil_hmap_size(struct cutil_hmap_t* map)
{
  return (map) ? map->size : 0;
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

int cutil_hmap_insert(struct cutil_hmap_t* map, struct cutil_hmap_tuple_t t)
{
  if (!map)
    return 0;
  
  size_t hash = map->hashFn(t.key.key, t.key.len) % map->buckets;
  struct hmap_bucket* buckets = map->mapData;

  // check to see if found
  int found = CUTIL_LT;
  struct hmap_node* tmp = buckets[hash].start;
  while (tmp)
  {
    found = map->compareFn(tmp->key.key, t.key.key, tmp->key.len, t.key.len);
    if (found)
      break;
    
    tmp = tmp->next;
  }


  // Element already exists. Don't insert
  if (found == CUTIL_EQ)
    return 0;

  struct hmap_node* ins = malloc(sizeof *ins);
  if (!ins)
    return 0;

  ins->data = t.value;
  ins->key = t.key;
  ins->next = buckets[hash].start;
  buckets[hash].start = ins;

  map->size++;
  cutil_hmap_rebucket(map);

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

int cutil_hmap_del(struct cutil_hmap_t* map, struct cutil_hmap_key_t key)
{
  if (!map)
    return 0;
  
  size_t hash = map->hashFn(key.key, key.len) % map->buckets;
  struct hmap_bucket* buckets = (struct hmap_bucket*) map->mapData;

  // hash search failed
  if (!buckets[hash].start)
    return 0;

  // begin chained search
  struct hmap_node* n = buckets[hash].start;
  struct hmap_node* prev = NULL;
  while (n)
  {
    int comp = map->compareFn(key.key, n->key.key, key.len, n->key.len);
    // key found!
    if (comp == CUTIL_EQ)
    {
      // if start node
      if (prev == NULL)
        buckets[hash].start = n->next;
      else
        prev->next = n->next;
      map->size--;

      if (map->destuctor)
      {
        struct cutil_hmap_tuple_t rm = cutil_hmap_make_tuple(n->key, n->data);
        map->destuctor(&rm);
      }

      free(n);

      return 1;
    }
  }

  return 0;
}

struct cutil_hmap_iterator_t cutil_hmap_iterator_create(struct cutil_hmap_t* hmap)
{
  struct cutil_hmap_iterator_t it;
  if (!hmap)
    return it;
  
  it.hmap = hmap;
  it.cur_bkt = 0;
  it.cur_nd = NULL;
}

struct cutil_hmap_tuple_t* cutil_hmap_iterator_peek(struct cutil_hmap_iterator_t* iterator)
{
  // if (!iterator || !iterator->hmap)
  //   return NULL;
  
  // struct hmap_bucket* buckets = (struct hmap_bucket*) iterator->hmap->mapData;

  // // find the first node
  // if (iterator->cur_nd == NULL && iterator->cur_bkt == 0)
  // {
  //   for (size_t i = iterator->cur_bkt; i < iterator->hmap->buckets; i++)
  //   {
  //     if (!buckets[i].start)
  //       continue;
  //   }
  // }
}

struct cutil_hmap_tuple_t* cutil_hmap_iterator_next(struct cutil_hmap_iterator_t* iterator)
{
}

struct cutil_hmap_key_t cutil_hmap_make_key(void* key, size_t len)
{
  struct cutil_hmap_key_t k;
  k.key = key;
  k.len = len;
  return k;
}

struct cutil_hmap_tuple_t cutil_hmap_make_tuple(struct cutil_hmap_key_t key, void* data)
{
  struct cutil_hmap_tuple_t t;
  t.key.key = key.key;
  t.key.len = key.len;
  t.value = data;

  return t;
}
