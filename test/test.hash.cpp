#include <gtest/gtest.h>

#include "cutil.h"
#include "hash.h"

#include <iostream>

TEST(arb_add_chained, test_basic)
{
  size_t test0[] = { 0, 1, 2, 3, 4, 5 };
  size_t expected = 15;
  size_t hash = cutil_hash_arb_add_chained((void*) test0, sizeof(test0));
  EXPECT_TRUE(expected == hash);
}

TEST(arb_xor_chained, test_basic)
{
  size_t test0[] = { 0, 1, 2, 3, 4, 5, 69 };
  size_t expected = 68;
  size_t hash = cutil_hash_arb_xor_chained((void*) test0, sizeof(test0));
  EXPECT_TRUE(expected == hash);
}

TEST(compare_lex, lt)
{
  const char* a = "abcdefg";
  const char* b = "bcdef";
  const char* c = "abcd";
  const char* d = "test1";
  const char* e = "test2";

  EXPECT_EQ(cutil_compare_lex((void*) a, (void*) b, strlen(a), strlen(b)), CUTIL_LT);
  EXPECT_EQ(cutil_compare_lex((void*) c, (void*) a, strlen(a), strlen(b)), CUTIL_LT);
  EXPECT_EQ(cutil_compare_lex((void*) d, (void*) e, strlen(d) + 1, strlen(e) + 1), CUTIL_LT);
}

TEST(compare_lex, gt)
{
  const char* a = "abcdefg";
  const char* b = "bcdef";
  const char* c = "abcd";

  EXPECT_EQ(cutil_compare_lex((void*) b, (void*) a, strlen(a), strlen(b)), CUTIL_GT);
  EXPECT_EQ(cutil_compare_lex((void*) a, (void*) c, strlen(a), strlen(b)), CUTIL_GT);
}

TEST(compare_lex, eq)
{
  const char* a = "abcdefg";
  const char* b = "bcdef";
  const char* c = "abcd";

  EXPECT_EQ(cutil_compare_lex((void*) a, (void*) a, strlen(a), strlen(a)), CUTIL_EQ);
  EXPECT_EQ(cutil_compare_lex(NULL, NULL, 0, 0), CUTIL_EQ);
}

