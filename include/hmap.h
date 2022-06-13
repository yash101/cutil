#ifndef _CUTIL_HASH_MAP_H
#define _CUTIL_HASH_MAP_H
#ifdef __cplusplus
extern "C" {
#endif

#include "hash.h"
#include <stddef.h>

/**
 * @brief CUtil Hash Map
 * 
 * This hash map uses chaining to handle collisions.
 * 
 * Initialize using the cutil_hmap_init() function
 * Destroy using the cutil_hmap_destroy() function
 */
typedef struct cutil_hmap_t
{
  void* mapData;                    /// Holds the hash map buckets
  size_t buckets;                   /// Number of buckets in the hashmap
  size_t size;                      /// Number of items in the hashmap
  float loadFactorMax;              /// Maximum load factor before expanding buckets
  float loadFactorMin;              /// Minimum load factor before contracting buckets
  size_t minBuckets;                /// Minimum number of buckets to keep
  cutil_hash_func_t hashFn;         /// Hash function to hash the keys with
  cutil_compare_func_t compareFn;   /// Equality comparison function to compare to see if two keys are identical
  cutil_destructor_func_t destuctor;/// Method to dellocate data and cleanup an entry
} cutil_hmap_t;

/**
 * @brief Holds the hash map key and its length
 * 
 */
typedef struct cutil_hmap_key_t
{
  void* key;  /// Key
  size_t len; /// Length of key
} cutil_hmap_key_t;

/**
 * @brief Holds the key and the value in a tuple
 * 
 */
typedef struct cutil_hmap_tuple_t
{
  cutil_hmap_key_t key;   /// Key
  void* value;            /// Value
} cutil_hmap_tuple_t;

/**
 * @brief Creates a cutil key from the key and the length
 * 
 * @param key arbitrary data to use as a hash map key
 * @param len length of the data to use as a hash map key
 * @return struct cutil_hmap_key_t key
 */
inline struct cutil_hmap_key_t cutil_hmap_make_key(void* key, size_t len)
{
  struct cutil_hmap_key_t k;
  k.key = key;
  k.len = len;
  return k;
}

/**
 * @brief Creates a cutil tuple with a key and its value
 * 
 * @param key   /// Key of the item
 * @param data  /// Value of the item
 * @return struct cutil_hmap_tuple_t Tuple to use
 */
inline struct cutil_hmap_tuple_t cutil_hmap_make_tuple(struct cutil_hmap_key_t key, void* data)
{
  struct cutil_hmap_tuple_t t;
  t.key = key;
  t.value = data;
}

/**
 * @brief Make a key simply
 * 
 * @param key   /// Key object
 */
#define cutil_hmap_key(key) cutil_hmap_make_key(key, sizeof *key);
/**
 * @brief Make a tuple simply
 * 
 * @param key   /// Key of the tuple
 * @param value /// Value of the tuple
 */
#define cutil_hmap_tuple(key, value) cutil_hmap_make_tuple(cutil_hmap_key(key), (void*) value);

/**
 * @brief Constructor for the hmap object
 * 
 * @param map pointer to a hmap
 */
void cutil_hmap_init(struct cutil_hmap_t* map);

/**
 * @brief Destructor for the hmap object
 * 
 * @param map pointer to a hmap
 */
void cutil_hmap_destroy(struct cutil_hmap_t* map);


/**
 * @brief Sets the destructor for each entry
 * 
 * The method signature for the destructor is as follows:
 * `void node_destructor(struct cutil_hmap_tuple_t* tuple)`
 * 
 * @param map pointer to the hmap
 * @param destructor destructor function to use
 */
void cutil_hmap_set_destructor(struct cutil_hmap_t* map, cutil_destructor_func_t destructor);

/**
 * @brief Sets the hash function to hash the keys with.
 * 
 * Default is `cutil_hash_arb_xor_chained`.
 * 
 * Function signature is `size_t hash(void* data, size_t len)`
 * 
 * @param map pointer to the hmap
 * @param hash_fn hash function
 */
void cutil_hmap_set_hashfn(struct cutil_hmap_t* map, cutil_hash_func_t hash_fn);

/**
 * @brief Sets the min and max load factor of the hash map.
 * 
 * Default: min = 0.25, max = 0.75
 * 
 * * If the load factor increases past 0.75, the hmap will be grown to contain more buckets.
 * * If the load factor decreases past 0.25, the hmap _may_ be shrunk to free space
 * 
 * Lower load factor means there are generally more empty buckets. This means that hashing is more effective
 * Higher load factor means a higher possibility of a hash collision, so chaining will be used. Chaining is slow.
 * 
 * @param map pointer to the hmap
 * @param min min load factor
 * @param max max load factor
 */
void cutil_hmap_set_loadfactor(struct cutil_hmap_t* map, float min, float max);

/**
 * @brief Set the minimum number of buckets to keep allocated
 * 
 * Default: 16
 * 
 * @param map pointer to the hmap
 * @param min minimum buckets to keep
 */
void cutil_hmap_set_min_buckets(struct cutil_hmap_t* map, size_t min);

int cutil_hmap_probe_key(struct cutil_hmap_t* map, struct cutil_hmap_key_t key);

/**
 * @brief Computes the hash of the key and checks if the hash exists in the hash map without checking equality.
 * 
 * This does not check if the key is in the hash map. If there is a hash collision, this will return true even if
 * the key does not exist in the map.
 * 
 * @param map hash map
 * @param key key
 * @return int boolean
 */
int cutil_hmap_probe_hash(struct cutil_hmap_t* map, struct cutil_hmap_key_t key);
int cutil_hmap_insert(struct cutil_hmap_t* map, struct cutil_hmap_tuple_t insert, int allowDuplicates);
void** cutil_hmap_get(struct cutil_hmap_t* map, struct cutil_hmap_key_t key);
int cutil_hmap_del(struct cutil_hmap_t* map, struct cutil_hmap_key_t key, int checkDuplicates);

#ifdef __cplusplus
}
#endif
#endif
