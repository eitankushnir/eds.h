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

list_t *_list_create(size_t capacity, size_t data_size, void (*free_fn)(void *), const char *type_name);
void _list_destroy(list_t **list);

void _list_assert_type(list_t *list, char *check, char *action);

#endif // EDS_NO_LIST

#ifndef EDS_NO_HASHMAP

typedef struct hashmap hashmap_t;
#define EDS_HASHMAP_INITIAL_CAPACITY 8
#define EDS_HASHMAP_LOAD_FACTOR 0.6f

#if defined(EDS_NO_CHECKS)
#define hashmap_assert_type(hashmap, KT, VT, action) ((void)0)
#define strmap_assert_type(hashmap, VT, action) ((void)0)
#else
#define hashmap_assert_type(hashmap, KT, VT, action) _hashmap_assert_type(hashmap, #KT, #VT, action)
#define strmap_assert_type(hashmap, VT, action) _strmap_assert_type(hashmap, #VT, action)
#endif

#define hashmap(KT, VT) _hashmap_create(EDS_HASHMAP_INITIAL_CAPACITY, sizeof(KT), sizeof(VT), #KT, #VT, _eds_cmp_bytes, _eds_hash_bytes, EDS_HASHMAP_LOAD_FACTOR)
#define strmap(VT) _hashmap_create(EDS_HASHMAP_INITIAL_CAPACITY, sizeof(char *), sizeof(VT), "strmap", #VT, _eds_cmp_str, _eds_hash_str, EDS_HASHMAP_LOAD_FACTOR)

#define hashmap_destroy(hashmap) _hashmap_destroy(&(hashmap))

#define hashmap_set(KT, VT, HashMap, Key, Value) \
  (hashmap_assert_type(HashMap, KT, VT, "set"), _hashmap_set(HashMap, &(KT){Key}, &(VT){Value}))

#define strmap_set(VT, HashMap, Key, Value) \
  (strmap_assert_type(HashMap, VT, "set"), _hashmap_set(HashMap, &(char *){Key}, &(VT){Value}))

#define EDS_IS_POINTER_TO(var, type) _Generic((var), typeof(type) *: 1, default: 0)
#define EDS_ASSERT_POINTER_TO(var, type, errmsg) \
  EDS_IS_POINTER_TO(var, type) ? (void)0 : eds_error(errmsg)

#define strmap_get(VT, HashMap, Key, Target)                                                                                                      \
  (strmap_assert_type(HashMap, VT, "get"), EDS_ASSERT_POINTER_TO(Target, VT, "strmap_get target pointer does not point to a value of " #VT "\n"), \
   _hashmap_get(HashMap, &(char *){Key}, Target))

#define hashmap_get(KT, VT, HashMap, Key, Target)                                                                                                       \
  (hashmap_assert_type(HashMap, KT, VT, "get"), EDS_ASSERT_POINTER_TO(Target, VT, "hashmap_get target pointer does not point to a value of " #VT "\n"), \
   _hashmap_get(HashMap, &(KT){Key}, Target))

#define strmap_pop(VT, HashMap, Key, Target)                                                                                                      \
  (strmap_assert_type(HashMap, VT, "pop"), EDS_ASSERT_POINTER_TO(Target, VT, "strmap_pop target pointer does not point to a value of " #VT "\n"), \
   _hashmap_pop(HashMap, &(char *){Key}, Target))

#define hashmap_pop(KT, VT, HashMap, Key, Target)                                                                                                       \
  (hashmap_assert_type(HashMap, KT, VT, "pop"), EDS_ASSERT_POINTER_TO(Target, VT, "hashmap_pop target pointer does not point to a value of " #VT "\n"), \
   _hashmap_pop(HashMap, &(KT){Key}, Target))

#define hashmap_remove(KT, VT, HashMap, Key) \
  (hashmap_assert_type(HashMap, KT, VT, "remove"), _hashmap_remove(HashMap, &(KT){Key}))

#define strmap_remove(VT, HashMap, Key) \
  (strmap_assert_type(HashMap, VT, "remove"), _hashmap_remove(HashMap, &(char *){Key}))

hashmap_t *_hashmap_create(
    size_t capacity,
    size_t key_size,
    size_t val_size,
    char *key_name,
    char *val_name,
    bool (*cmp_fn)(const void *, const void *, size_t),
    size_t (*hash_fn)(const void *, size_t),
    float load_factor);

void hashmap_set_key_free_fn(hashmap_t *hashmap, void (*key_free_fn)(void *));
void hashmap_set_val_free_fn(hashmap_t *hashmap, void (*val_free_fn)(void *));

void _hashmap_destroy(hashmap_t **hashmap);

size_t hashmap_capacity(hashmap_t *hashmap);
size_t hashmap_size(hashmap_t *hashmap);

void _hashmap_grow(hashmap_t *hashmap);
void _hashmap_set(hashmap_t *hashmap, void *key, void *value);

bool _hashmap_get(hashmap_t *hashmap, void *key, void *out_target);

bool _hashmap_remove(hashmap_t *hashmap, void *key);
bool _hashmap_pop(hashmap_t *hashmap, void *key, void *out_target);

void _hashmap_assert_type(hashmap_t *hashmap, const char *key_name, const char *val_name, const char *action);
void _strmap_assert_type(hashmap_t *hashmap, const char *val_name, const char *action);
#endif // EDS_NO_HASHMAP

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

list_t *_list_create(size_t capacity, size_t data_size, void (*free_fn)(void *), const char *type_name) {
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

#ifndef EDS_NO_HASHMAP

size_t _eds_hash_bytes(const void *key, size_t key_size) {
  // FNV-1a hashing
  size_t hash = 14695981039346656037ULL; // FNV-1 64-bit offset basis.

  const char *bytes = (const char *)key;
  for (size_t i = 0; i < key_size; i++) {
    hash = (hash ^ bytes[i]);
    hash = hash * 1099511628211; // FNV-1 prime.
  }

  return hash;
}

size_t _eds_hash_str(const void *key, size_t key_size) {
  (void)key_size;
  const char *str = *(const char **)key;
  size_t len = strlen(str);

  return _eds_hash_bytes(str, len);
}

bool _eds_cmp_str(const void *key1, const void *key2, const size_t key_size) {
  (void)key_size;
  char *str1 = *(char **)key1;
  char *str2 = *(char **)key2;

  return strcmp(str1, str2) == 0;
}

bool _eds_cmp_bytes(const void *key1, const void *key2, const size_t key_size) {
  return memcmp(key1, key2, key_size) == 0;
}

struct hashmap {
  void *keys;
  size_t key_size;
  void (*key_free_fn)(void *);
  const char *key_name;

  void *values;
  size_t value_size;
  void (*val_free_fn)(void *);
  const char *val_name;

  char *states;
  size_t capacity;
  size_t size;

  bool (*cmp_fn)(const void *, const void *, size_t key_size);
  size_t (*hash_fn)(const void *, size_t key_size);

  float load_factor;
};

hashmap_t *_hashmap_create(
    size_t capacity,
    size_t key_size,
    size_t val_size,
    char *key_name,
    char *val_name,
    bool (*cmp_fn)(const void *, const void *, size_t),
    size_t (*hash_fn)(const void *, size_t),
    float load_factor) {

  hashmap_t *hm = eds_malloc(sizeof(hashmap_t));
  hm->keys = eds_malloc(key_size * capacity);
  hm->key_size = key_size;
  hm->key_free_fn = NULL;
  hm->key_name = key_name;

  hm->values = eds_malloc(val_size * capacity);
  hm->value_size = val_size;
  hm->val_free_fn = NULL;
  hm->val_name = val_name;

  hm->states = eds_malloc(sizeof(char) * capacity);
  memset(hm->states, 0, sizeof(char) * capacity);

  hm->capacity = capacity;
  hm->size = 0;

  hm->hash_fn = hash_fn;
  hm->cmp_fn = cmp_fn;
  hm->load_factor = load_factor;

  return hm;
}

void hashmap_set_key_free_fn(hashmap_t *hashmap, void (*key_free_fn)(void *)) {
  hashmap->key_free_fn = key_free_fn;
}
void hashmap_set_val_free_fn(hashmap_t *hashmap, void (*val_free_fn)(void *)) {
  hashmap->val_free_fn = val_free_fn;
}

void _hashmap_destroy(hashmap_t **hashmap) {
  hashmap_t *hm = *hashmap;
  if (!hm)
    return;

  if (hm->val_free_fn) {
    for (size_t i = 0; i < hm->capacity; i++) {
      if (hm->states[i] == 1)
        hm->val_free_fn((char *)hm->values + i * hm->value_size);
    }
  }

  if (hm->key_free_fn) {
    for (size_t i = 0; i < hm->capacity; i++) {
      if (hm->states[i] == 1)
        hm->key_free_fn((char *)hm->keys + i * hm->key_size);
    }
  }

  free(hm->values);
  free(hm->keys);
  free(hm->states);
  free(hm);
  *hashmap = NULL;
}

size_t hashmap_capacity(hashmap_t *hashmap) {
  return hashmap->capacity;
}
size_t hashmap_size(hashmap_t *hashmap) {
  return hashmap->size;
}

void _hashmap_grow(hashmap_t *hashmap) {
  size_t cap = hashmap->capacity;
  size_t new_cap = hashmap->capacity * 2;

  void *keys_grow = eds_malloc(hashmap->key_size * new_cap);
  void *values_grow = eds_malloc(hashmap->value_size * new_cap);
  char *states_grow = eds_malloc(sizeof(char) * new_cap);
  memset(states_grow, 0, sizeof(char) * new_cap);

  for (size_t i = 0; i < cap; i++) {
    if (hashmap->states[i] != 1)
      continue;

    void *key = (char *)hashmap->keys + i * hashmap->key_size;
    void *val = (char *)hashmap->values + i * hashmap->value_size;

    size_t hash = hashmap->hash_fn(key, hashmap->key_size);
    for (size_t j = 0; j < new_cap; j++) {
      size_t idx = (hash + j) & (new_cap - 1);
      if (states_grow[idx] != 1) {
        memcpy((char *)keys_grow + idx * hashmap->key_size, key, hashmap->key_size);
        memcpy((char *)values_grow + idx * hashmap->value_size, val, hashmap->value_size);
        states_grow[idx] = 1;
        break;
      }
    }
  }

  free(hashmap->keys);
  free(hashmap->values);
  free(hashmap->states);
  hashmap->keys = keys_grow;
  hashmap->values = values_grow;
  hashmap->states = states_grow;
  hashmap->capacity = new_cap;
}

void _hashmap_set(hashmap_t *hashmap, void *key, void *value) {
  if (hashmap->size >= hashmap->capacity * hashmap->load_factor) {
    _hashmap_grow(hashmap);
  }

  size_t hash = hashmap->hash_fn(key, hashmap->key_size);
  for (size_t i = 0; i < hashmap->capacity; i++) {
    size_t idx = (hash + i) & (hashmap->capacity - 1);
    void *test_key = (char *)hashmap->keys + idx * hashmap->key_size;
    void *valueptr = (char *)hashmap->values + idx * hashmap->value_size;

    if (hashmap->states[idx] != 1) {
      memcpy(test_key, key, hashmap->key_size);
      memcpy(valueptr, value, hashmap->value_size);
      hashmap->states[idx] = 1;
      hashmap->size++;
      break;
    } else if (hashmap->cmp_fn(key, test_key, hashmap->key_size) == true) {
      if (hashmap->val_free_fn)
        hashmap->val_free_fn(valueptr);
      memcpy(valueptr, value, hashmap->value_size);
      break;
    }
  }
}

bool _hashmap_get(hashmap_t *hashmap, void *key, void *out_target) {
  size_t idx = hashmap->hash_fn(key, hashmap->key_size) & (hashmap->capacity - 1);

  for (size_t i = idx; i < hashmap->capacity; i++) {
    char state = hashmap->states[i];
    if (state == 0)
      return false;

    if (state == 1) {
      void *test_key = (char *)hashmap->keys + i * hashmap->key_size;
      if (hashmap->cmp_fn(test_key, key, hashmap->key_size) == true) {
        void *value = (char *)hashmap->values + i * hashmap->value_size;
        memcpy(out_target, value, hashmap->value_size);
        return true;
      }
    }
  }

  return false;
}

bool _hashmap_remove(hashmap_t *hashmap, void *key) {
  size_t hash = hashmap->hash_fn(key, hashmap->key_size);

  for (size_t i = 0; i < hashmap->capacity; i++) {
    size_t idx = (hash + i) & (hashmap->capacity - 1);
    char state = hashmap->states[idx];
    if (state == 0)
      return false;

    if (state == 1) {
      void *test_key = (char *)hashmap->keys + idx * hashmap->key_size;
      if (hashmap->cmp_fn(test_key, key, hashmap->key_size) == true) {
        void *value = (char *)hashmap->values + idx * hashmap->value_size;
        if (hashmap->val_free_fn)
          hashmap->val_free_fn(value);
        if (hashmap->key_free_fn)
          hashmap->key_free_fn(test_key);

        hashmap->states[idx] = 2;
        hashmap->size--;
        return true;
      }
    }
  }

  return false;
}

bool _hashmap_pop(hashmap_t *hashmap, void *key, void *out_target) {
  size_t hash = hashmap->hash_fn(key, hashmap->key_size);

  for (size_t i = 0; i < hashmap->capacity; i++) {
    size_t idx = (hash + i) & (hashmap->capacity - 1);
    char state = hashmap->states[idx];
    if (state == 0)
      return false;

    if (state == 1) {
      void *test_key = (char *)hashmap->keys + idx * hashmap->key_size;
      if (hashmap->cmp_fn(test_key, key, hashmap->key_size) == true) {
        void *value = (char *)hashmap->values + idx * hashmap->value_size;
        memcpy(out_target, value, hashmap->value_size);
        hashmap->states[idx] = 2;
        hashmap->size--;
        return true;
      }
    }
  }

  return false;
}

void _hashmap_assert_type(hashmap_t *hashmap, const char *key_name, const char *val_name, const char *action) {
  if (strcmp(key_name, hashmap->key_name) != 0 || strcmp(val_name, hashmap->val_name) != 0) {
    if (action) {
      eds_error("Type mismatch. Trying to use %s with type (%s, %s) on hashmap of type (%s, %s).",
                action,
                key_name, val_name,
                hashmap->key_name, hashmap->val_name);
    } else {
      eds_error("Type mismatch. hashmap of type (%s, %s) does not match (%s, %s).",
                hashmap->key_name, hashmap->val_name,
                key_name, val_name);
    }
  }
}

void _strmap_assert_type(hashmap_t *hashmap, const char *val_name, const char *action) {
  if (hashmap->cmp_fn != _eds_cmp_str || strcmp(val_name, hashmap->val_name) != 0) {
    if (action) {
      eds_error("Type mismatch. Trying to use %s with type (strmap, %s) on strmap of type (strmap, %s).",
                action,
                val_name,
                hashmap->val_name);
    } else {
      eds_error("Type mismatch. strmap of type (strmap, %s) does not match (strmap, %s).",
                hashmap->val_name,
                val_name);
    }
  }
}
#endif

#endif // EDS_IMPLEMENTATION
