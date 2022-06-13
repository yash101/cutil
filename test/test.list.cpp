#include <gtest/gtest.h>

#include "list.h"

#include <vector>
#include <iostream>

TEST(list_node, init_get_destroy)
{
  struct cutil_list_node_t node;
  node.data = (void*) &node;
  node.next = (struct cutil_list_node_t*) &node + 8;
  node.prev = (struct cutil_list_node_t*) &node + 16;

  cutil_list_node_init(&node);
  
  EXPECT_TRUE(node.data == NULL);
  EXPECT_TRUE(node.prev == NULL);
  EXPECT_TRUE(node.next == NULL);

  node.data = (void*) &node;
  node.next = (struct cutil_list_node_t*) &node + 8;
  node.prev = (struct cutil_list_node_t*) &node + 16;

  EXPECT_TRUE(*cutil_list_node_data(&node) == &node);

  cutil_list_node_destroy(&node);

  EXPECT_TRUE(node.data == NULL);
  EXPECT_TRUE(node.prev == NULL);
  EXPECT_TRUE(node.next == NULL);

  cutil_list_node_init(NULL);
  cutil_list_node_destroy(NULL);
}

TEST(list, insert_test_0)
{
  return;
  struct cutil_list_t list;
  list.root = (struct cutil_list_node_t*) 0xDEADBEEF;
  list.end = (struct cutil_list_node_t*) 0xDEADBEEF + 8;
  list.length = 69;

  cutil_list_init(&list);
  EXPECT_TRUE(list.root == NULL);
  EXPECT_TRUE(list.end == NULL);
  EXPECT_TRUE(list.length == 0);

  int insarr[] = { 1, 2, 3, 4, 5, 6 };
  for (size_t i = 0; i < sizeof(insarr) / sizeof(*insarr); i++)
  {
    cutil_list_insert_front(&list, &insarr[i]);
  }

  EXPECT_TRUE(cutil_list_size(&list) == list.length);
  EXPECT_TRUE(list.length == 6);

  size_t idx = 0;
  struct cutil_list_node_t* tmp = list.root;
  while (tmp)
  {
    EXPECT_TRUE(tmp->data != NULL);
    EXPECT_EQ(insarr[(sizeof(insarr) / sizeof(*insarr)) - idx++ - 1], *(int*) tmp->data);
    tmp = tmp->next;
  }
  EXPECT_EQ(idx, 6);

  // insert 12 into the 4th position
  int arr2[] = { 1, 2, 3, 4, 12, 5, 6 };
  cutil_list_insert(&list, &arr2[3], 4);
  tmp = list.root;
  idx = 0;
  while (tmp)
  {
    EXPECT_TRUE(tmp->data != NULL);
    EXPECT_EQ(*(int*) tmp->data, arr2[(sizeof(arr2) / sizeof(*arr2)) - idx++ - 1]);
    std::cout << *(int*) tmp->data << std::endl;
    tmp = tmp->next;
  }

  cutil_list_destroy(&list, NULL);
  EXPECT_TRUE(list.root == NULL);
  EXPECT_TRUE(list.end == NULL);
  EXPECT_TRUE(list.length == 0);
}
