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
  _list_set_capacity(*lp, initial_cap);
  _list_set_size(*lp, 0);
}

void _list_append(void *lst, void *data, size_t ds) {
  memcpy(lst + ds * _list_size(lst), data, ds);
  _list_set_size(lst, _list_size(lst) + 1);
}

void _list_free(void *lst) {
  free((size_t *)lst - 2);
}
