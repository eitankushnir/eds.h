#include <eds/list.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

size_t _list_capacity(void *lst) {
  if (!lst)
    return 0;

  return ((size_t *)lst)[-1];
}

size_t _list_size(void *lst) {
  if (!lst)
    return 0;

  return ((size_t *)lst)[-2];
}

void _list_set_capacity(void *lst, size_t capacity) {
  ((size_t *)lst)[-1] = capacity;
}
void _list_set_size(void *lst, size_t size) {
  ((size_t *)lst)[-2] = size;
}

void _list_init(void **lp, size_t ds) {
  size_t initial_cap = 2 * sizeof(size_t) + ds * EDS_LIST_INITIAL_CAPACITY;
  *lp = (size_t *)malloc(initial_cap) + 2;
  _list_set_capacity(*lp, EDS_LIST_INITIAL_CAPACITY);
  _list_set_size(*lp, 0);
}

void *_list_grow(void *lst, size_t n, size_t ds) {
  void *ptr;
  size_t cap = n * ds + 2 * sizeof(size_t);
  if (!lst)
    ptr = malloc(cap);
  else
    ptr = realloc((size_t *)lst - 2, cap);

  void *new_lst = (size_t *)ptr + 2;
  _list_set_capacity(new_lst, n);
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
  free((size_t *)*lp - 2);
  *lp = NULL;
}
