#ifndef _CUTIL_LIST_H
#define _CUTIL_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef struct cutil_list_node_t
{
  void* data;
  struct cutil_list_node_t* next;
  struct cutil_list_node_t* prev;
} cutil_list_node_t;

typedef void (*cutil_list_node_destructor_t)(void*);

typedef struct cutil_list_t
{
  struct cutil_list_node_t* root;
  struct cutil_list_node_t* end;
  size_t length;
} cutil_list_t;

typedef struct cutil_list_iterator_t
{
  struct cutil_list_t* list;
  struct cutil_list_node_t* current;
} cutil_list_iterator_t;

void cutil_list_node_init(struct cutil_list_node_t* node);
void cutil_list_node_destroy(struct cutil_list_node_t* node);
void** cutil_list_node_data(struct cutil_list_node_t* node);

void cutil_list_init(struct cutil_list_t* list);
void cutil_list_destroy(struct cutil_list_t* list, cutil_list_node_destructor_t element_destructor_fn);
size_t cutil_list_size(struct cutil_list_t* list);

struct cutil_list_node_t* cutil_list_get(struct cutil_list_t* list, ptrdiff_t pos);
int cutil_list_insert(struct cutil_list_t* list, void* data, ptrdiff_t pos);
void* cutil_list_remove(struct cutil_list_t* list, ptrdiff_t pos);

struct cutil_list_node_t* cutil_list_back(struct cutil_list_t* list);
int cutil_list_insert_back(struct cutil_list_t* list, void* data);
void* cutil_list_remove_back(struct cutil_list_t* list);

struct cutil_list_node_t* cutil_list_front(struct cutil_list_t* list);
int cutil_list_insert_front(struct cutil_list_t* list, void* data);
void* cutil_list_remove_front(struct cutil_list_t* list);

void cutil_list_iterator_init(struct cutil_list_iterator_t* iterator, struct cutil_list_t* list, struct cutil_list_node_t* start);
void cutil_list_iterator_destroy(struct cutil_list_iterator_t* iterator);

struct cutil_list_node_t* cutil_list_iterator_get(struct cutil_list_iterator_t* iterator);
struct cutil_list_node_t* cutil_list_iterator_next(struct cutil_list_iterator_t* iterator);
struct cutil_list_node_t* cutil_list_iterator_back(struct cutil_list_iterator_t* iterator);
struct cutil_list_node_t* cutil_list_iterator_peek(struct cutil_list_iterator_t* iterator);
struct cutil_list_node_t* cutil_list_iterator_peek_back(struct cutil_list_iterator_t* iterator);

#ifdef __cplusplus
}
#endif
#endif
