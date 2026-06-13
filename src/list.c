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

void _list_set_capacity(void *lst, size_t capacity) {
  _list_header(lst)->capacity = capacity;
}
void _list_set_size(void *lst, size_t size) {
  _list_header(lst)->size = size;
}

void *_list_grow(void *lst, size_t n, size_t ds) {
  void *ptr;
  size_t alloc = n * ds + sizeof(struct list_header);
  if (!lst) {
    ptr = malloc(alloc);
    struct list_header *header = (struct list_header *)ptr;
    header->size = 0;
    header->capacity = n;
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

void _list_destroy(void **lp) {
  void *lst = *lp;
  if (!lst)
    return;

  free(_list_header(lst));
  *lp = NULL;
}

void _list_destroy_complex(void **lp, size_t ds, void (*free_func)(void *)) {
  void *lst = *lp;
  if (!lst)
    return;

  size_t size = _list_size(lst);

  char *lst_char = (char *)lst;
  for (size_t i = 0; i < size; i++) {
    void *item = *(void **)(lst_char + i * ds);
    if (free_func)
      free_func(item);
  }

  _list_destroy(lp);
}

void _list_reserve(void **lp, size_t n, size_t ds) {
  if (!n || !ds)
    *lp = NULL;

  void *lst = *lp;
  *lp = _list_grow(lst, n, ds);
}
