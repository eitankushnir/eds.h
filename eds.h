#ifndef EDS__H
#define EDS__H
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef EDS_NO_LIST
#define EDS_LIST_INITIAL_CAPCITY 10

typedef struct list list_t;

#define list(type) _list_create(EDS_LIST_INITIAL_CAPCITY, sizeof(type), NULL)
#define list_with_capcity(type, capacity) _list_create((capacity), sizeof((type)), NULL)
#define list_with_free(type, free_fn) _list_create(EDS_LIST_INITIAL_CAPCITY, sizeof((type)), (free_fn))
#define list_with_capacity_free(type, capacity, free_fn) _list_create((capacity), sizeof((type)), (free_fn))

#define list_destroy(list) _list_destroy(&(list))

#define list_at_as(list, index, type) (*(type *)list_at((list), (index)))
#define list_set(list, index, data)      \
  do {                                   \
    typeof((data)) lvalue = data;        \
    _list_set((list), (index), &lvalue); \
  } while (0)

#define list_append(list, data)    \
  do {                             \
    typeof((data)) lvalue = data;  \
    _list_append((list), &lvalue); \
  } while (0)

size_t list_size(list_t *list);
size_t list_capacity(list_t *list);
bool list_is_empty(list_t *list);

void *list_at(list_t *list, size_t index);
void _list_set(list_t *list, size_t index, void *data);
void _list_append(list_t *list, void *data);

list_t *_list_create(size_t capacity, size_t data_size, void (*free_fn)(void *));
void _list_destroy(list_t **list);
#endif // EDS_NO_LIST

#endif

#define EDS_IMPLEMENTATION
#ifdef EDS_IMPLEMENTATION

#include <stdarg.h>

void eds_error(char *format, ...) {
  va_list vargs;
  va_start(vargs, format);
  fprintf(stderr, "EDS ERROR: ");
  vfprintf(stderr, format, vargs);
  fprintf(stderr, "\n");
  va_end(vargs);
  exit(128);
}

void *eds_malloc(size_t bytes) {
  void *p = malloc(bytes);
  if (!p)
    eds_error("Failed to allocate %zu bytes.", bytes);
  return p;
}

void *eds_realloc(void *p, size_t bytes) {
  p = realloc(p, bytes);
  if (!p)
    eds_error("Failed to reallocate %zu bytes.", bytes);
  return p;
}

#ifndef EDS_NO_LIST

struct list {
  void *data;
  size_t size;
  size_t capacity;
  size_t data_size;

  bool is_sotring_pointers;

  void (*free_fn)(void *);
};

list_t *_list_create(size_t capacity, size_t data_size, void (*free_fn)(void *)) {
  if (!capacity) {
    eds_error("list_t cannot have 0 capacity");
  }
  list_t *list = eds_malloc(sizeof(list_t));
  list->capacity = capacity;
  list->size = 0;
  list->data_size = data_size;
  list->free_fn = free_fn;
  list->data = eds_malloc(data_size * capacity);
  return list;
}

void _list_destroy(list_t **listptr) {
  list_t *l = *listptr;
  if (!l)
    return;

  if (l->free_fn) {
    for (size_t i = 0; i < l->size; i++) {
    }
  }

  free(l->data);
  free(l);

  *listptr = NULL;
}

size_t list_size(list_t *list) {
  return list->size;
}
size_t list_capacity(list_t *list) {
  return list->capacity;
}
bool list_is_empty(list_t *list) {
  return list->size == 0;
}

void *list_at(list_t *list, size_t index) {
  if (index >= list->size)
    eds_error("Index %zu is out of bounds in list of size %zu.", index, list->size);

  return (char *)list->data + list->data_size * index;
}

void _list_set(list_t *list, size_t index, void *data) {
  if (index >= list->size)
    eds_error("Index %zu is out of bounds in list of size %zu.", index, list->size);

  memcpy((char *)list->data + index * list->data_size, data, list->data_size);
}

void _list_append(list_t *list, void *data) {
  if (list->size >= list->capacity) {
    list->data = eds_realloc(list->data, list->data_size * list->capacity * 2);
    list->capacity *= 2;
  }

  memcpy((char *)list->data + list->size * list->data_size, data, list->data_size);
  list->size++;
}
#endif // EDS_NO_LIST

#endif // EDS_IMPLEMENTATION
