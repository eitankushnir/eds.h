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

void _list_grow(void **lp, size_t n, size_t ds) {
  void *ptr = realloc((size_t *)*lp - 2, n * ds + 2 * sizeof(size_t));
  *lp = (size_t *)ptr + 2;
  _list_set_capacity(*lp, n);
}

void _list_append(void **lp, void *data, size_t ds) {
  if (!_list_capacity(*lp)) {
    _list_init(lp, ds);
  } else if (list_capacity(*lp) < list_size(*lp) + 1) {
    _list_grow(lp, _list_size(*lp) * 2, ds);
  }

  void *lst = *lp;
  memcpy(lst + ds * _list_size(lst), data, ds);
  _list_set_size(lst, _list_size(lst) + 1);
}

void _list_free(void **lp) {
  free((size_t *)*lp - 2);
  *lp = NULL;
}
