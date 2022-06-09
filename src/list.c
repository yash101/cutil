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
    cutil_list_node_destroy(tmp);
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

  if (pos == 0)
    return cutil_list_insert_front(list, data);
  
  if (pos == list->end || pos == SIZE_MAX)
    return cutil_list_insert_back(list, data);

  if (pos > list->end)
    return 0;

  struct cutil_list_node_t* insert = malloc(sizeof *insert);
  cutil_list_node_init(insert);
  if (!insert)
    return 0;

  struct cutil_list_node_t* tmp = list->root;
  size_t cur = 0;
  while (++cur < pos && tmp)
    tmp = tmp->next;
  
  insert->next = tmp->next;
  insert->prev = tmp;
  tmp->next = insert;
  list->length++;

  return 1;
}

int cutil_list_insert_iterator(struct cutil_list_iterator_t* iterator, void* data)
{
  if (!iterator)
    return 0;

  if (iterator->current == iterator->list->root)
    return cutil_list_insert_front(iterator->list, data);

  if (iterator->current == iterator->list->end)
    return cutil_list_insert_back(iterator->list, data);

  struct cutil_list_node_t* insert = malloc(sizeof *insert);
  if (!insert)
    return 0;
  cutil_list_node_init(insert);
  insert->data = data;
  insert->prev = iterator->current->prev;
  insert->next = iterator->current->next;
  iterator->list->length++;

  if (insert->prev == NULL)
    iterator->list->root = insert->prev;

  if (insert->next == NULL)
    iterator->list->end = insert;
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
  
  struct cutil_list_node_t* del = tmp;
  tmp->prev->next = tmp->next;
  tmp->next->prev = tmp->prev;
  void* data = del->data;
  cutil_list_node_destroy(del);
  free(del);
  list->length--;

  return data;
}

void* cutil_list_remove_iterator(struct cutil_list_iterator_t* iterator)
{
  if (!iterator)
    return NULL;
  
  if (iterator->current == iterator->list->root)
    return cutil_list_remove_front(iterator->list);
  
  if (iterator->current == iterator->list->end)
    return cutil_list_remove_back(iterator->list);
  
  iterator->current->prev->next = iterator->current->next;
  iterator->current->next->prev = iterator->current->prev;
  void* data = iterator->current->data;
  cutil_list_node_destroy(iterator->current);
  free(iterator->current);
  iterator->list->length--;
}

struct cutil_list_node_t* cutil_list_back(struct cutil_list_t* list)
{
  if (!list || !list->end)
    return NULL;

  return list->end;
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
  if (!list->root)
  {
    list->root = add;
    list->end = add;
    return 1;
  }

  add->prev = list->end;
  list->end = add;

  return 1;
}

void* cutil_list_remove_back(struct cutil_list_t* list)
{
  if (!list || !list->end)
    return NULL;
  
  struct cutil_list_node_t* del = list->end;
  void* data = list->end->data;
  list->end = list->end->prev;
  list->length--;
  if (list->length == 0)
    list->root = NULL;
  
  cutil_list_node_destroy(del);
  free(del);
  return data;
}

struct cutil_list_node_t* cutil_list_back(struct cutil_list_t* list)
{
  return (!list) ? NULL : list->root;
}

int cutil_list_insert_front(struct cutil_list_t* list, void* data)
{
  if (!list)
    return 0;
  
  struct cutil_list_node_t* add = malloc(sizeof *add);
  if (!add)
    return 0;

  cutil_list_node_init(add);
  
  add->next = list->root;
  list->root = add;
  list->length++;
  if (add->next == NULL)
    list->end = add;
}

void* cutil_list_remove_front(struct cutil_list_t* list)
{
  if (!list || !list->root)
    return NULL;
  
  struct cutil_list_node_t* del = list->root;
  list->root = list->root->next;
  list->root->prev = NULL;
  void* data = del->data;
  list->length--;
  cutil_list_node_destroy(del);
  free(del);
  return data;
}


