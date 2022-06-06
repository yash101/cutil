#include <stddef.h>
#include <stdint.h>

#include "list.h"

void cutil_list_node_init(struct cutil_list_node_t* node)
{
  if (!node)
    return;
  
  node->prev = NULL;
  node->next = NULL;
  node->data = NULL;
}

void cutil_list_node_destroy(struct cutil_list_node_t* node)
{
}

void** cutil_list_node_data(struct cutil_list_node_t* node)
{
  return node ? node->data : NULL;
}



void cutil_list_init(struct cutil_list_t* list)
{
  if (!list)
    return;

  list->root = NULL;
  list->end = NULL;
  list->length = 0;
}

void cutil_list_destroy(struct cutil_list_t* list, cutil_list_node_destructor_t destructor)
{
  if (!list)
    return;
  
  while (list->root)
  {
    struct cutil_list_node_t* tmp = list->root;
    list->root = list->root->next;
    destructor(*cutil_list_node_data(tmp));
    free(tmp);
    list->length--;
  }
  list->end = NULL;
}

size_t cutil_list_size(struct cutil_list_t* list)
{
  if (!list)
    return 0;
  return list->length;
}

void cutil_list_iterator(struct cutil_list_t* list, struct cutil_list_iterator_t* iterator)
{
  if (!list || !iterator)
    return;
  
  iterator->list = list;
  iterator->current = list->root;
}

struct cutil_list_node_t* cutil_list_get(struct cutil_list_t* list, size_t pos)
{
  if (!list || !list->root || pos >= list->length)
    return NULL;
  
  struct cutil_list_node_t* tmp = list->root;
  size_t cur = 0;
  while (cur++ < pos && tmp)
    tmp = tmp->next;
  
  return tmp;
}

int cutil_list_insert(struct cutil_list_t* list, void* data, size_t pos)
{
  // list is not defined, or position is greater than the end
  if (!list || pos > list->length)
    return 0;
  
  if (pos == list->end || pos == SIZE_MAX)
  {
    cutil_list_insert_back(list, data);
    return 1;
  }

  if (pos == 0)
  {
    cutil_list_insert_front(list, data);
    return 1;
  }

  if (pos > list->end)
    return 0;

  struct cutil_list_node_t* insert = malloc(sizeof *insert);
  if (!insert)
    return 0;

  struct cutil_list_node_t* tmp = list->root;
  size_t cur = 0;
  while (++cur < pos && tmp)
    tmp = tmp->next;
  
  insert->next = tmp->next;
  insert->prev = tmp;
  tmp->next = insert;

  return 1;
}

void* cutil_list_remove(struct cutil_list_t* list, size_t pos)
{
  if (pos == 0)
    return cutil_list_remove_front(list);

  if (pos == list->length || pos == SIZE_MAX)
    return cutil_list_remove_back(list);
  
  if (pos > list->length)
    return NULL;

  struct cutil_list_node_t* tmp = list->root;
  size_t cur = 0;
  while (++cur < pos && tmp)
    tmp = tmp->next;
  
  struct cutil_list_t* del = tmp;
}
