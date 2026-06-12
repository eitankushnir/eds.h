#ifndef EDS_LIST_H
#define EDS_LIST_H

#define EDS_LIST_INITIAL_CAPACITY 10
#define IS_POINTER_TO(a, b) _Generic((a), typeof((b)) *: 1, default: 0)
#define ASSERT_POINTER(a, b) static_assert(IS_POINTER_TO((a), (b)), "Error: type mismatch");

#include <stddef.h>

#define list_append(list, value)                       \
  do {                                                 \
    typeof((value)) lvalue = (value);                  \
    ASSERT_POINTER(list, lvalue)                       \
    typeof(list) *p = &(list);                         \
    _list_append((void **)p, &lvalue, sizeof(lvalue)); \
  } while (0)

#define list_size(list) _list_size((list))
#define list_capacity(list) _list_capacity((list))
#define list_destroy(list)     \
  do {                         \
    typeof(list) *p = &(list); \
    _list_destroy((void **)p); \
  } while (0)

#define list_isempty(list) _list_size((list)) == 0
#define list_clear(list) _list_set_size((list), 0)

size_t _list_capacity(void *lst);
size_t _list_size(void *lst);

void _list_set_capacity(void *lst, size_t capacity);
void _list_set_size(void *lst, size_t size);

void *_list_grow(void *lst, size_t n, size_t ds);

void _list_init(void **lp, size_t ds);
void _list_append(void **lp, void *data, size_t ds);
void _list_destroy(void **lp);

#endif
