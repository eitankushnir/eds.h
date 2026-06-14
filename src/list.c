#include <eds/list.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct list_header *_list_header(void *lst) {
  struct list_header *header = (struct list_header *)lst - 1;
  return header;
}

size_t _list_capacity(void *lst) {
  if (!lst)
    return 0;

  return _list_header(lst)->capacity;
}

size_t _list_size(void *lst) {
  if (!lst)
    return 0;

  return _list_header(lst)->size;
}

void (*_list_free_fn(void *lst))(void *) {
  if (!lst)
    return NULL;

  return _list_header(lst)->free_fn;
}

void _list_free_item(void *lst, void *item) {
  void (*free_fn)(void *) = _list_free_fn(lst);
  if (free_fn)
    free_fn(item);
}

void _list_set_capacity(void *lst, size_t capacity) {
  _list_header(lst)->capacity = capacity;
}
void _list_set_size(void *lst, size_t size) {
  _list_header(lst)->size = size;
}

void _list_set_free_fn(void **lp, void (*free_fn)(void *), size_t ds) {
  void *lst = *lp;
  if (!lst) {
    lst = _list_grow(lst, EDS_LIST_INITIAL_CAPACITY, ds);
    *lp = lst;
  }

  _list_header(lst)->free_fn = free_fn;
}

void *_list_grow(void *lst, size_t n, size_t ds) {
  void *ptr;
  size_t alloc = n * ds + sizeof(struct list_header);
  if (!lst) {
    ptr = malloc(alloc);
    struct list_header *header = (struct list_header *)ptr;
    header->size = 0;
    header->capacity = n;
    header->free_fn = NULL;
  } else {
    ptr = realloc(_list_header(lst), alloc);
    struct list_header *header = (struct list_header *)ptr;
    header->capacity = n;
  }

  void *new_lst = (struct list_header *)ptr + 1;
  return new_lst;
}

void _list_append(void **lp, void *data, size_t ds) {
  void *lst = *lp;
  size_t cap = _list_capacity(lst);
  size_t size = _list_size(lst);

  if (size >= cap) {
    size_t new_cap = cap == 0 ? EDS_LIST_INITIAL_CAPACITY : cap * 2;
    lst = _list_grow(lst, new_cap, ds);
    *lp = lst;
  }

  char *new_elem_ptr = (char *)lst + ds * size;
  memcpy(new_elem_ptr, data, ds);
  _list_set_size(lst, size + 1);
}

void _list_destroy(void **lp, size_t ds) {
  void *lst = *lp;
  if (!lst)
    return;

  struct list_header *header = _list_header(lst);
  size_t size = header->size;
  void (*free_fn)(void *) = header->free_fn;

  if (free_fn) {
    char *lst_char = (char *)lst;
    for (size_t i = 0; i < size; i++) {
      void *item = *(void **)(lst_char + i * ds);
      free_fn(item);
    }
  }

  free(header);
  *lp = NULL;
}

void _list_reserve(void **lp, size_t n, size_t ds) {
  if (!n || !ds)
    *lp = NULL;

  void *lst = *lp;
  *lp = _list_grow(lst, n, ds);
}

void _list_trim(void **lp, size_t ds) {
  void *lst = *lp;
  if (!lst)
    return;

  if (_list_size(lst) == 0) {
    _list_destroy(lp, ds);
    return;
  }

  *lp = _list_grow(lst, _list_size(lst), ds);
}

void _list_clear(void *lst, size_t ds) {
  if (!lst)
    return;

  struct list_header *header = _list_header(lst);
  if (header->free_fn) {
    char *lst_char = (char *)lst;
    for (size_t i = 0; i < _list_size(lst); i++) {
      void *item = *(void **)(lst_char + i * ds);
      header->free_fn(item);
    }
  }

  header->size = 0;
}

void _list_insert(void **lp, size_t i, void *data, size_t ds) {
  void *lst = *lp;
  size_t cap = _list_capacity(lst);
  size_t size = _list_size(lst);

  if (size >= cap) {
    size_t new_cap = cap == 0 ? EDS_LIST_INITIAL_CAPACITY : cap * 2;
    lst = _list_grow(lst, new_cap, ds);
    *lp = lst;
  }

  char *lst_char = (char *)lst;
  for (size_t j = size; j > i; j--) {
    memcpy(lst_char + ds * j, lst_char + ds * (j - 1), ds);
  }

  char *new_elem_ptr = lst_char + ds * i;
  memcpy(new_elem_ptr, data, ds);
  _list_set_size(lst, size + 1);
}

void _list_remove(void *lst, size_t i, size_t ds) {
  size_t size = _list_size(lst);

  char *lst_char = (char *)lst;
  struct list_header *header = _list_header(lst);
  if (header->free_fn) {
    free(*(void **)(lst_char + ds * i));
  }

  for (size_t j = i; j < size - 1; j++) {
    memcpy(lst_char + ds * j, lst_char + ds * (j + 1), ds);
  }

  _list_set_size(lst, size - 1);
}

void _list_pop(void *lst, size_t i, size_t ds, void *out) {
  size_t size = _list_size(lst);

  char *lst_char = (char *)lst;
  memcpy(out, lst_char + ds * i, ds);

  for (size_t j = i; j < size - 1; j++) {
    memcpy(lst_char + ds * j, lst_char + ds * (j + 1), ds);
  }

  _list_set_size(lst, size - 1);
}

void *_list_copy(void *lst, size_t ds) {
  if (!lst)
    return NULL;

  struct list_header *header = _list_header(lst);
  size_t total_alloc = header->capacity * ds + sizeof(struct list_header);
  void *copy = malloc(total_alloc);
  memcpy(copy, header, total_alloc);

  struct list_header *copy_header = copy;
  copy_header->free_fn = NULL;
  return (void *)(copy_header + 1);
}
