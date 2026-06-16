#ifndef EDS__H
#define EDS__H
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef EDS_NO_LIST
#define EDS_LIST_INITIAL_CAPACITY 10

typedef struct list list_t;

#define list(type) _list_create(EDS_LIST_INITIAL_CAPACITY, sizeof(type), NULL, #type)
#define list_with_capacity(type, capacity) _list_create((capacity), sizeof(type), NULL, #type)
#define list_with_free(type, free_fn) _list_create(EDS_LIST_INITIAL_CAPACITY, sizeof(type), (free_fn), #type)
#define list_with_capacity_free(type, capacity, free_fn) _list_create((capacity), sizeof(type), (free_fn), #type)

#define list_destroy(list) _list_destroy(&(list))

#if defined(EDS_NO_CHECKS)
#define list_assert_type(list, type, action) ((void)0)
#else
#define list_assert_type(list, type, action) _list_assert_type(list, #type, action)
#endif

#define list_of_type(list, type) _list_of_type(list, #type)

#define list_get_as(list, index, type) \
  (list_assert_type(list, type, "get"), (*(type *)list_at((list), (index))))

#define list_set_unchecked(list, index, data) \
  do {                                        \
    typeof((data)) lvalue = (data);           \
    _list_set((list), (index), &lvalue);      \
  } while (0)

#define list_set_checked(list, index, data, type) \
  do {                                            \
    list_assert_type(list, type, "set");          \
    type lvalue = (data);                         \
    _list_set((list), (index), &lvalue);          \
  } while (0)

#define EDS_SET_MACRO(_1, _2, _3, _4, name, ...) name
#define list_set(...) EDS_SET_MACRO(__VA_ARGS__, list_set_checked, list_set_unchecked)(__VA_ARGS__)

#define list_append_unchecked(list, data) \
  do {                                    \
    typeof((data)) lvalue = (data);       \
    _list_append((list), &lvalue);        \
  } while (0)

#define list_append_checked(list, data, type) \
  do {                                        \
    list_assert_type(list, type, "append");   \
    type lvalue = (data);                     \
    _list_append((list), &lvalue);            \
  } while (0)

#define EDS_APPEND_MACRO(_1, _2, _3, name, ...) name
#define list_append(...) EDS_APPEND_MACRO(__VA_ARGS__, list_append_checked, list_append_unchecked)(__VA_ARGS__)

#define list_insert_unchecked(list, index, data) \
  do {                                           \
    typeof((data)) lvalue = (data);              \
    _list_insert((list), index, &lvalue);        \
  } while (0)

#define list_insert_checked(list, index, data, type) \
  do {                                               \
    list_assert_type(list, type, "insert");          \
    type lvalue = (data);                            \
    _list_insert((list), index, &lvalue);            \
  } while (0)

#define EDS_INSERT_MACRO(_1, _2, _3, _4, name, ...) name
#define list_insert(...) EDS_INSERT_MACRO(__VA_ARGS__, list_insert_checked, list_insert_unchecked)(__VA_ARGS__)

#define list_clear(list) list_set_size(list, 0)
#define list_trim(list) list_set_capacity(list, list_size(list))

#define list_foreach(list, type, item)                                                   \
  for (size_t EDS_ITERATOR = (list_assert_type(list, type, "foreach"), 0), EDS_KEEP = 1; \
       EDS_KEEP && EDS_ITERATOR < list_size(list);                                       \
       EDS_KEEP = !EDS_KEEP, EDS_ITERATOR++)                                             \
    for (type item = list_get_as(list, EDS_ITERATOR, type); EDS_KEEP; EDS_KEEP = !EDS_KEEP)

size_t list_size(list_t *list);
size_t list_capacity(list_t *list);
bool list_is_empty(list_t *list);

void list_set_size(list_t *list, size_t size);
void list_set_capacity(list_t *list, size_t capacity);

void *list_at(list_t *list, size_t index);
void _list_set(list_t *list, size_t index, void *data);
void _list_append(list_t *list, void *data);
void _list_insert(list_t *list, size_t index, void *data);

void list_remove(list_t *list, size_t index);
void list_pop(list_t *list, size_t index, void *out_target);

list_t *_list_create(size_t capacity, size_t data_size, void (*free_fn)(void *), char *type_name);
void _list_destroy(list_t **list);

void _list_assert_type(list_t *list, char *check, char *action);
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

void eds_free(void *ptr) {
  free(*(void **)ptr);
  *(void **)ptr = NULL;
}

#ifndef EDS_NO_LIST

struct list {
  void *data;
  size_t size;
  size_t capacity;
  size_t data_size;

  const char *type_name;

  void (*free_fn)(void *);
};

list_t *_list_create(size_t capacity, size_t data_size, void (*free_fn)(void *), char *type_name) {
  if (!capacity) {
    eds_error("list_t cannot have 0 capacity");
  }
  list_t *list = eds_malloc(sizeof(list_t));
  list->capacity = capacity;
  list->size = 0;
  list->data_size = data_size;
  list->free_fn = free_fn;
  list->data = eds_malloc(data_size * capacity);
  list->type_name = type_name;
  return list;
}

void _list_destroy(list_t **listptr) {
  list_t *l = *listptr;
  if (!l)
    return;

  if (l->free_fn) {
    for (size_t i = 0; i < l->size; i++) {
      void *item_to_remove = (char *)l->data + l->data_size * i;
      l->free_fn(item_to_remove);
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

void _list_assert_type(list_t *list, char *check, char *action) {
  if (strcmp(check, list->type_name) != 0) {
    if (action)
      eds_error("Type mismatch. Trying to use %s with type %s on list of type %s.", action, check, list->type_name);
    else
      eds_error("Type mismatch. List of type %s does not match type %s.\n", list->type_name, check);
  }
}

void list_remove(list_t *list, size_t index) {
  if (index >= list->size)
    eds_error("Index %zu is out of bounds in list of size %zu.", index, list->size);

  void *item_to_remove = (char *)list->data + index * list->data_size;
  if (list->free_fn)
    list->free_fn(item_to_remove);

  size_t item_to_shift = list->size - index - 1;
  if (item_to_shift) {
    size_t bytes_to_shift = item_to_shift * list->data_size;
    memmove(item_to_remove, (char *)item_to_remove + list->data_size, bytes_to_shift);
  }

  list->size--;
}

void list_pop(list_t *list, size_t index, void *out_target) {
  if (index >= list->size)
    eds_error("Index %zu is out of bounds in list of size %zu.", index, list->size);

  void *item_to_remove = (char *)list->data + index * list->data_size;
  memcpy(out_target, item_to_remove, list->data_size);

  size_t item_to_shift = list->size - index - 1;
  if (item_to_shift) {
    size_t bytes_to_shift = item_to_shift * list->data_size;
    memmove(item_to_remove, (char *)item_to_remove + list->data_size, bytes_to_shift);
  }

  list->size--;
}

void _list_insert(list_t *list, size_t index, void *data) {
  if (index == list->size) {
    _list_append(list, data);
    return;
  }

  if (index > list->size)
    eds_error("Index %zu is out of bounds in list of size %zu.", index, list->size);

  if (list->size >= list->capacity) {
    list->data = eds_realloc(list->data, list->data_size * list->capacity * 2);
    list->capacity *= 2;
  }

  size_t item_to_shift = list->size - index;
  if (!item_to_shift)
    return;

  size_t bytes_to_shift = item_to_shift * list->data_size;
  memmove((char *)list->data + (index + 1) * list->data_size, (char *)list->data + list->data_size * index, bytes_to_shift);
  memcpy((char *)list->data + index * list->data_size, data, list->data_size);
  list->size++;
}

void list_set_size(list_t *list, size_t size) {
  if (size > list->size)
    eds_error("Cannot use list_set_size with size %zu which is larger than list size %zu. \nTo extend the list use list_set_capacity.", size, list->size);

  if (!list->free_fn) {
    list->size = size;
    return;
  }

  for (size_t i = size; i < list->size; i++) {
    void *item_to_free = (char *)list->data + i * list->data_size;
    list->free_fn(item_to_free);
  }

  list->size = size;
}

void list_set_capacity(list_t *list, size_t capacity) {
  if (capacity == 0) {
    free(list->data);
    list->data = NULL;
    list->capacity = 0;
    return;
  }

  if (capacity == list->capacity)
    return;

  if (capacity < list->size && list->free_fn) {
    for (size_t i = capacity; i < list->size; i++) {
      list->free_fn((char *)list->data + i * list->data_size);
    }
    list->size = capacity;
  }

  list->data = eds_realloc(list->data, capacity * list->data_size);
  list->capacity = capacity;
}

bool _list_of_type(list_t *l, char *type_name) {
  return strcmp(l->type_name, type_name) == 0;
}

#endif // EDS_NO_LIST

#endif // EDS_IMPLEMENTATION
