#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "cutil.h"
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
  if (!node)
    return;

  node->prev = NULL;
  node->next = NULL;
  node->data = NULL;
}

void** cutil_list_node_data(struct cutil_list_node_t* node)
{
  return (!node) ? NULL : node->data;
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
    if (destructor)
      destructor(*cutil_list_node_data(tmp));
    cutil_list_node_destroy(tmp);
    free(tmp);
    list->length--;
  }
  list->end = NULL;
}

size_t cutil_list_size(struct cutil_list_t* list)
{
  return (!list) ? 0 : list->length;
}

struct cutil_list_node_t* cutil_list_get(struct cutil_list_t* list, ptrdiff_t pos)
{
  ptrdiff_t abs = (pos < 0) ? (-pos) - 1 : pos;
  if (!list || !list->root)
    return NULL;
  
  struct cutil_list_node_t* ret = NULL;
  if (pos == CUTIL_BEG || pos == -(list->length))
    return list->root;
  if (pos == CUTIL_END || pos == list->length - 1)
    return list->end;
  if (abs >= list->length)
    return NULL;
  
  if (pos < 0)
  {
    ret = list->end;
    ptrdiff_t ctr = 0;
    while (++ctr < abs && ret)  // iterate to the correct position
      ret = ret->prev;
    
    return ret;
  }
  else
  {
    ret = list->root;
    ptrdiff_t ctr = 0;
    while (++ctr < abs && ret)  // iterate to the correct position
      ret = ret->next;
    
    return ret;
  }
}

int cutil_list_insert(struct cutil_list_t* list, void* data, ptrdiff_t pos)
{
  ptrdiff_t abs = (pos < 0) ? -pos - 1 : pos;
  if (!list || !list->root)
    return 0;
  
  if (pos == CUTIL_BEG || pos == -(list->length + 1))
    return cutil_list_insert_front(list, data);
  
  if (pos == CUTIL_END || pos == list->length || pos == -1)
    return cutil_list_insert_back(list, data);

  if (abs > list->length)
    return 0;

  struct cutil_list_node_t* add = malloc(sizeof *add);
  if (!add)
    return 0;

  cutil_list_node_init(add);
  add->data = data;
  
  if (pos < 0)
  {
    struct cutil_list_node_t* it = list->end;
    ptrdiff_t ctr = 0;
    while (++ctr < abs && it)
      it = it->prev;

    add->next = it->next;
    it->next = add;
    add->prev = it;
    add->next->prev = add;
  }
  else
  {
    struct cutil_list_node_t* it = list->root;
    ptrdiff_t ctr = 0;

    // iterate to position before add location
    while (++ctr < abs && it)
      it = it->next;
    
    add->next = it->next;
    it->next = add;
    add->prev = it;
    add->next->prev = add;
  }
  list->length++;

  return 1;
}

void* cutil_list_remove(struct cutil_list_t* list, ptrdiff_t pos)
{
  ptrdiff_t abs = (pos < 0) ? (-pos) - 1 : pos;
  if (!list || !list->root)
    return NULL;

  if (!(abs < list->length || pos == CUTIL_END))
    return NULL;

  struct cutil_list_node_t* tmp = list->root;
  ptrdiff_t ctr = 0;

  while (ctr++ < abs && tmp)
    tmp = (pos < 0) ? tmp->prev : tmp->next;
  
  if (!tmp)
    return NULL;

  // remove node tmp
  if (tmp->next)
    tmp->next->prev = tmp->prev;
  else
    list->end = tmp->next;
  
  if (tmp->prev)
    tmp->prev->next = tmp->prev;
  else
    list->root = tmp->prev;
  
  list->length--;
  void* data = tmp->data;
  cutil_list_node_destroy(tmp);
  free(tmp);
  return data;
}

struct cutil_list_node_t* cutil_list_back(struct cutil_list_t* list)
{
  return (!list) ? NULL : list->end;
}

int cutil_list_insert_back(struct cutil_list_t* list, void* data)
{
  if (!list)
    return 0;
  
  struct cutil_list_node_t* add = malloc(sizeof *add);
  if (!add)
    return 0;
  cutil_list_node_init(add);
  add->data = data;
  list->length++;

  if (!list->end)
  {
    list->root = add;
    list->end = add;
    return 1;
  }

  add->prev = list->end;
  list->end->next = add;
  list->end = add;

  return 1;
}

void* cutil_list_remove_back(struct cutil_list_t* list)
{
  if (!list || !list->end)
    return NULL;
  
  struct cutil_list_node_t* del = list->end;
  void* data = list->end->data;

  if (list->end == list->root)
    list->root = (list->end = NULL);
  list->length--;
  list->end = del->prev;
  if (list->end)
    list->end->next = NULL;
  
  cutil_list_node_destroy(del);
  free(del);
  return data;
}

int cutil_list_insert_front(struct cutil_list_t* list, void* data)
{
  if (!list)
    return 0;
  
  struct cutil_list_node_t* add = malloc(sizeof *add);
  if (!add)
    return 0;

  cutil_list_node_init(add);
  add->data = data;
  add->next = list->root;
  if (list->root)
    list->root->prev = add;
  list->root = add;

  list->length++;

  return 1;
}

void* cutil_list_remove_front(struct cutil_list_t* list)
{
  if (!list || !list->root)
    return NULL;
  
  struct cutil_list_node_t* del = list->root;
  void* data = del->data;

  list->root = del->next;
  if (list->root)
  {
    list->root->prev = NULL;
    list->end = list->root;
  }

  cutil_list_node_destroy(del);
  free(del);
  return data;
}

void cutil_list_iterator_init(struct cutil_list_iterator_t* iterator, struct cutil_list_t* list, struct cutil_list_node_t* node)
{
  if (!iterator || !list || !node)
    return;
  iterator->current = node;
  iterator->list = list;
}

void cutil_list_iterator_destroy(struct cutil_list_iterator_t* iterator)
{
  if (!iterator)
    return;
  
  iterator->current = NULL;
  iterator->list = NULL;
}

struct cutil_list_node_t* cutil_list_iterator_get(struct cutil_list_iterator_t* iterator)
{
  return (iterator && iterator->list) ? iterator->current : NULL;
}

struct cutil_list_node_t* cutil_list_iterator_next(struct cutil_list_iterator_t* iterator)
{
  if (!iterator || !iterator->list || !iterator->current)
    return NULL;
  
  iterator->current = iterator->current->next;
  return iterator->current;
}

struct cutil_list_node_t* cutil_list_iterator_back(struct cutil_list_iterator_t* iterator)
{
  if (!iterator || !iterator->list || !iterator->current)
    return NULL;
  
  iterator->current = iterator->current->prev;
  return iterator->current;
}

struct cutil_list_node_t* cutil_list_iterator_peek(struct cutil_list_iterator_t* iterator)
{
  return (iterator && iterator->list && iterator->current) ? iterator->current->next : NULL;
}

struct cutil_list_node_t* cutil_list_iterator_peek_back(struct cutil_list_iterator_t* iterator)
{
  return (iterator && iterator->list && iterator->current) ? iterator->current->prev : NULL;
}
