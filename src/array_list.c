#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ARRAY_CAPACITY 512
#define array(T, a) array_init(sizeof(T), ARRAY_CAPACITY, a)
#define array_header(array) (((Array_Header *)array) - 1)
#define array_length(array) ((array_header(array))->length)
#define array_capacity(array) ((Array_Header *)(array)->capacity)

#define array_append(a, v)                                                     \
  ((a) = array_ensure_capacity(a, 1, sizeof(*a)),                              \
   (a)[array_header(a)->length] = (v), &(a)[array_header(a)->length++])

#define array_remove(array, idx)                                               \
  do {                                                                         \
    Array_Header *header = array_header(array);                                \
    int last_idx = header->length - 1;                                         \
    if (last_idx == idx) {                                                     \
      header->length--;                                                        \
    } else if (header->length > 1) {                                           \
      void *free_space = &array[idx];                                          \
      void *last = &array[last_idx];                                           \
      memcpy(free_space, last, sizeof(*array));                                \
      header->length--;                                                        \
    }                                                                          \
  } while (0);

#define pop_back(array) ((array)[array_header(array)->length])

typedef struct {
  void *(*alloc)(size_t size);
  void (*free)(void *ptr, size_t size, void *context);
  void *context;
} Allocator;

void *my_alloc(size_t size) { return malloc(size); }

void my_free(void *ptr, size_t size, void *context) {
  (void)ptr;
  (void)size;
  (void)context;
  free(ptr);
}

typedef struct {
  size_t length;
  size_t capacity;
  size_t padding;
  Allocator *alloc;
} Array_Header;

// la manera en la que esta array list va a funcionar es utilizando headers,
// de la misma manera que cuando implementamos malloc, aritmetica de punteros
// etc

void *array_init(size_t item_size, size_t capacity, Allocator *a) {
  void *ptr = 0;
  size_t size = sizeof(Array_Header) + item_size * capacity;
  Array_Header *header = a->alloc(size);
  if (!header)
    return NULL;
  header->capacity = capacity;
  header->length = 0;
  header->alloc = a;
  ptr = header + 1;
  return ptr;
}

void *array_ensure_capacity(void *array, size_t item_count, size_t item_size) {
  Array_Header *header = array_header(array);

  if (!header)
    return NULL;
  if (!header->alloc)
    return NULL;
  if (!header->alloc->alloc)
    return NULL;
  if (!header->alloc->free)
    return NULL;

  size_t desired_capacity = header->length + item_count;

  if (header->capacity > desired_capacity) {
    return header + 1;
  }

  size_t new_capacity = header->capacity * 2;
  while (new_capacity < header->capacity)
    new_capacity = header->capacity * 2;
  size_t old_size = sizeof(*header) + header->length * item_size;
  size_t new_size = old_size + item_count * item_size;

  Array_Header *reallocated_header = header->alloc->alloc(new_size);

  if (!reallocated_header)
    return NULL;

  memcpy(reallocated_header, header, old_size);
  header->alloc->free(header, header->length, header->alloc->context);
  return reallocated_header + 1;
}
