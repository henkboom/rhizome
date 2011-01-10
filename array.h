#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>

typedef struct {
    void *data;
    size_t capacity;
    size_t length;
} untyped_array_s;

#define array_of(value_type) \
    union \
    { \
        untyped_array_s _untyped; \
        value_type *_typed_data; \
    } *

#define array_new() \
    ((void *)untyped_array_new())

#define array_release(array) \
    untyped_array_release(&(array)->_untyped)

#define array_length(array) \
    ((array)->_untyped.length / sizeof(*(array)->_typed_data))

#define array_set_length(array, new_length) \
    untyped_array_set_length( \
        &(array)->_untyped, \
        (new_length) * sizeof(*(array)->_typed_data))

#define array_ensure_capacity(array, min_capacity) \
    untyped_array_ensure_capacity( \
        &(array)->_untyped, \
        (min_capacity) * sizeof(*(array)->_typed_data))

#define array_set_capacity(array, new_capacity) \
    untyped_array_set_length( \
        &(array)->_untyped, \
        (new_capacity) * sizeof(*(array)->_typed_data))

#define array_add(array, new_value) \
    (array_set_length(array, array_length(array) + 1), \
     array_set(array, array_length(array)-1, new_value))

#define array_get(array, index) \
    ((array)->_typed_data[index])

#define array_set(array, index, value) \
    ((array)->_typed_data[index] = (value))

#define array_get_ptr(array) \
    ((array)->_typed_data)

untyped_array_s *untyped_array_new();
void untyped_array_release(untyped_array_s *array);

void untyped_array_set_length(untyped_array_s *array, size_t new_length);
void untyped_array_ensure_capacity(untyped_array_s *array, size_t min_capacity);
void untyped_array_set_capacity(untyped_array_s *array, size_t new_capacity);

#endif
