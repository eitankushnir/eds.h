#ifndef EDS_LIST_H
#define EDS_LIST_H

#define EDS_LIST_INITIAL_CAPACITY 10
#define IS_POINTER_TO(a, b) _Generic((a), typeof((b)) *: 1, default: 0)
#define ASSERT_POINTER(a, b) static_assert(IS_POINTER_TO((a), (b)), "Error: type mismatch")

#define SAME_TYPE(a, b) _Generic((a), typeof((b)): 1, default: 0)
#define ASSERT_SAME_TYPE(a, b) static_assert(SAME_TYPE((a), (b)), "Type mismatch")

#define IS_DYNAMIC_ARRAY(a) _Generic((&a), typeof(*(a)) **: 1, default: 0)
#define ASSERT_DYNAMIC_ARRAY(a) static_assert(IS_DYNAMIC_ARRAY((a)), "Cannot append to static array")

#define IS_STRING_ARRAY(a) _Generic((&a), char (*)[]: 1, default: 0)
#define ASSERT_STR_ARR(a) static_assert(IS_STRING_ARRAY((a)), "NOT STRING ARRAY")

#define IS_FREE_FUNC(a) _Generic((a), void (*)(void *): 1, default: 0)
#define ASSERT_FREE_FUNC(a) static_assert(IS_FREE_FUNC((a)), "Error: Function cannot be used as a free function. Signature mismatch")
#include <stddef.h>

#define list_append(list, value)                             \
  do {                                                       \
    ASSERT_DYNAMIC_ARRAY(list);                              \
    typeof_unqual((value)) lvalue = (value);                 \
    typeof_unqual(*(list)) type_check;                       \
    ASSERT_SAME_TYPE(type_check, lvalue);                    \
    _list_append((void **)&(list), &lvalue, sizeof(lvalue)); \
  } while (0)

#define list_size(list) _list_size((list))
#define list_capacity(list) _list_capacity((list))
#define list_destroy(list) \
  _list_destroy((void **)&(list), sizeof(typeof(*list)))

#define list_set_free_fn(list, free_fn) \
  _list_set_free_fn((void **)&(list), free_fn, sizeof(typeof(*list)))

#define list_isempty(list) _list_size((list)) == 0

#define list_foreach(list, item)                     \
  for (size_t EDS_ITERATOR = 0, EDS_KEEP = 1;        \
       EDS_KEEP && EDS_ITERATOR < list_size((list)); \
       EDS_KEEP = !EDS_KEEP, EDS_ITERATOR++)         \
    for (typeof(*(list)) item = (list)[EDS_ITERATOR]; EDS_KEEP; EDS_KEEP = !EDS_KEEP)

#define list_reserve(list, capacity) \
  _list_reserve((void **)&(list), capacity, sizeof(typeof(*list)))

#define list_trim(list) \
  _list_trim((void **)&(list), sizeof(typeof(*list)))

#define list_clear(list) \
  _list_clear((list), sizeof(typeof(*list)))

struct list_header {
  size_t size;
  size_t capacity;
  void (*free_fn)(void *); // Function to free complex data types.
};

struct list_header *_list_header(void *lst);
size_t _list_capacity(void *lst);
size_t _list_size(void *lst);

void _list_set_capacity(void *lst, size_t capacity);
void _list_set_size(void *lst, size_t size);
void _list_set_free_fn(void **lp, void (*free_fn)(void *), size_t ds);

void *_list_grow(void *lst, size_t n, size_t ds);

void _list_reserve(void **lp, size_t n, size_t ds);

void _list_append(void **lp, void *data, size_t ds);
void _list_destroy(void **lp, size_t ds);

void _list_trim(void **lp, size_t ds);
void _list_clear(void *lst, size_t ds);

#endif
