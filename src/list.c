#include <stddef.h>
#include <stdint.h>

#include "cutil.h"
#include "list.h"

void cutil_list_node_init(struct cutil_list_node_t* node)
{
  if (!node && (node->prev = (node->data = (node->next = NULL))))
    return;
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

struct cutil_list_node_t* cutil_list_get(struct cutil_list_t* list, size_t pos)
{
  if (!list || !list->root || (pos >= list->length && pos != CUTIL_END))
    return NULL;

  if (pos == CUTIL_END)
    return list->end;
  
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
  
  if (pos == list->end || pos == CUTIL_END)
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

void* cutil_list_remove(struct cutil_list_t* list, size_t pos)
{
  if (pos == 0)
    return cutil_list_remove_front(list);

  if (pos == (list->length - 1) || pos == list->length || pos == SIZE_MAX)
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
  list->length--;
  void* data = del->data;
  cutil_list_node_destroy(del);
  free(del);

  return data;
}

struct cutil_list_node_t* cutil_list_back(struct cutil_list_t* list)
{
  return (!list) ? list : list->end;
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
  if (!list->end)
    list->end = add;
  list->length++;
  if (add->next)
    add->next->prev = add;
  
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


