#include <gtest/gtest.h>

#include "hmap.h"

#include <string.h>

TEST(hmap_other, make_key)
{
  const char* key = "hello mf";
  size_t len = strlen(key) + 1;

  auto k = cutil_hmap_make_key((void*) key, len);
  EXPECT_TRUE(k.key == key);
  EXPECT_TRUE(k.len == len);

  size_t k2 = 8400525;

  k = cutil_hmap_key(&k2);
  EXPECT_TRUE(k.key == &k2);
  EXPECT_TRUE(k.len == sizeof(k2));
}

TEST(hmap_other, make_tuple)
{
  size_t k0 = 3428904;
  int val = 34243;
  
  auto t = cutil_hmap_make_tuple(cutil_hmap_key(&k0), &val);

  EXPECT_TRUE(&k0 == t.key.key);
  EXPECT_TRUE(sizeof(k0) == t.key.len);
  EXPECT_TRUE(val = *(int*) t.value);
}

TEST(hmap, null_oops)
{
  EXPECT_EQ(cutil_hmap_insert(NULL, cutil_hmap_tuple_t()), 0);
  EXPECT_TRUE(cutil_hmap_get(NULL, cutil_hmap_key_t()) == NULL);
  EXPECT_EQ(cutil_hmap_del(NULL, cutil_hmap_key_t()), 0);
}

TEST(hmap, basic0)
{
  struct cutil_hmap_t map;
  cutil_hmap_init(&map);
  cutil_hmap_set_destructor(&map, NULL);

  EXPECT_TRUE(map.mapData != NULL);
  EXPECT_EQ(map.size, 0);

  const char* keys[] = {
    "test0",
    "test1",
    "test2"
  };

  const char* vals[] = {
    "val0",
    "val1",
    "val2"
  };

  size_t n = sizeof(keys) / sizeof(*keys);
  // insert all
  for (size_t i = 0; i < n; i++)
  {
    struct cutil_hmap_key_t k = cutil_hmap_make_key((void*) keys[i], strlen(keys[i]) + 1);
    struct cutil_hmap_tuple_t ins = cutil_hmap_make_tuple(k, (void*) vals[i]);
    EXPECT_EQ(1, cutil_hmap_insert(&map, ins));
    EXPECT_EQ(i + 1, cutil_hmap_size(&map));
  }

  // delete all
  for (size_t i = 0; i < n; i++)
  {
    struct cutil_hmap_key_t k = cutil_hmap_make_key((void*) keys[i], strlen(keys[i]) + 1);
    EXPECT_EQ(1, cutil_hmap_del(&map, k));
    EXPECT_EQ(n - i - 1, cutil_hmap_size(&map));
  }

  // insert twice
  for (size_t i = 0; i < n; i++)
  {
    struct cutil_hmap_key_t k = cutil_hmap_make_key((void*) keys[i], strlen(keys[i]) + 1);
    struct cutil_hmap_tuple_t ins = cutil_hmap_make_tuple(k, (void*) vals[i]);
    EXPECT_EQ(1, cutil_hmap_insert(&map, ins));
    EXPECT_EQ(i + 1, cutil_hmap_size(&map));
  }

  for (size_t i = 0; i < n; i++)
  {
    struct cutil_hmap_key_t k = cutil_hmap_make_key((void*) keys[i], strlen(keys[i]) + 1);
    struct cutil_hmap_tuple_t ins = cutil_hmap_make_tuple(k, (void*) vals[i]);
    EXPECT_EQ(0, cutil_hmap_insert(&map, ins));
    EXPECT_EQ(n, cutil_hmap_size(&map));
  }

  cutil_hmap_destroy(&map);
}

static void destructor(struct cutil_hmap_tuple_t* t)
{
  if (t->key.key)
    free(t->key.key);
  if (t->value)
    free(t->value);
}

// hash fn for size_t
static size_t hashfn(void* data, size_t len)
{
  return *(size_t*) data;
}

TEST(hmap, resize_buckets)
{
  struct cutil_hmap_t map;
  cutil_hmap_init(&map);
  cutil_hmap_set_destructor(&map, (cutil_destructor_func_t) destructor);
  cutil_hmap_set_hashfn(&map, hashfn);

  EXPECT_EQ(map.buckets, map.minBuckets);

  size_t lastBuckets = 0;
  // insert a ton of data
  for (size_t i = 0; i < 1024; i++)
  {
    size_t* k = new size_t;
    *k = i;

    EXPECT_EQ(1, cutil_hmap_insert(&map, cutil_hmap_tuple(k, NULL)));
    EXPECT_EQ(i + 1, cutil_hmap_size(&map));
    size_t lf_fixp = (map.size * 1000) / (map.buckets + 1);
    float lf = lf_fixp / 1000.f;
    EXPECT_GE(map.buckets, map.minBuckets);
    if (map.buckets > lastBuckets)
    {
      printf("%ld -> %ld @ %ld\n", lastBuckets, map.buckets, i);
      lastBuckets = map.buckets;
    }
    // if (map.buckets != map.minBuckets)
    //   EXPECT_LT(lf, map.loadFactorMax);
  }
}
