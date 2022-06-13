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
  EXPECT_EQ(cutil_hmap_insert(NULL, cutil_hmap_tuple_t(), 0), 0);
  EXPECT_TRUE(cutil_hmap_get(NULL, cutil_hmap_key_t()) == NULL);
  EXPECT_EQ(cutil_hmap_del(NULL, cutil_hmap_key_t(), 0), 0);
}

TEST(hmap, basic0)
{
  struct cutil_hmap_t map;
  cutil_hmap_init(&map);
  cutil_hmap_set_destructor(&map, NULL);

  EXPECT_TRUE(map.mapData != NULL);
  EXPECT_EQ(map.size, 0);

  int k0 = 0;
  const char* str = "test0";

  EXPECT_EQ(1,
    cutil_hmap_insert(
      &map,
      cutil_hmap_make_tuple(
        cutil_hmap_make_key(&k0, sizeof(k0)),
        (void*) str
      ),
      0
    )
  );
  EXPECT_EQ(1, cutil_hmap_size(&map));

  EXPECT_EQ(0,
    cutil_hmap_insert(
      &map,
      cutil_hmap_make_tuple(
        cutil_hmap_make_key(&k0, sizeof(k0)),
        (void*) str
      ),
      0
    )
  );
  EXPECT_EQ(1, cutil_hmap_size(&map));

  const char* str2 = "test1";
  EXPECT_EQ(1,
    cutil_hmap_insert(
      &map,
      cutil_hmap_make_tuple(
        cutil_hmap_make_key(&k0, sizeof(k0)),
        (void*) str2
      ),
      1
    )
  );
  EXPECT_EQ(2, cutil_hmap_size(&map));

  const char* x = *(const char**) cutil_hmap_get(&map, cutil_hmap_key(&k0));
  EXPECT_TRUE(x == str2 || x == str);

  EXPECT_EQ(cutil_hmap_del(&map, cutil_hmap_make_key(&k0, sizeof(k0)), 1), 2);
  EXPECT_EQ(cutil_hmap_size(&map), 0);
  EXPECT_EQ(0, cutil_hmap_del(&map, cutil_hmap_make_key(&k0, sizeof k0), 0));

  int k1 = 69;
  EXPECT_EQ(0, cutil_hmap_del(&map, cutil_hmap_make_key(&k1, sizeof k0), 0));

  EXPECT_EQ(1,
    cutil_hmap_insert(
      &map,
      cutil_hmap_make_tuple(
        cutil_hmap_make_key(&k0, sizeof(k0)),
        (void*) str
      ),
      0
    )
  );
  EXPECT_EQ(1, cutil_hmap_size(&map));

  EXPECT_EQ(1,
    cutil_hmap_insert(
      &map,
      cutil_hmap_make_tuple(
        cutil_hmap_make_key(&k0, sizeof(k0)),
        (void*) str
      ),
      1
    )
  );
  EXPECT_EQ(2, cutil_hmap_size(&map));

  EXPECT_EQ(1,
    cutil_hmap_insert(
      &map,
      cutil_hmap_make_tuple(
        cutil_hmap_make_key(&k0, sizeof(k0)),
        (void*) str2
      ),
      1
    )
  );
  EXPECT_EQ(3, cutil_hmap_size(&map));
  EXPECT_EQ(1, cutil_hmap_del(&map, cutil_hmap_make_key(&k0, sizeof k0), 0));
}
