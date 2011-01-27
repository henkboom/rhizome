#ifndef RHIZOME__ARRAY_H
#define RHIZOME__ARRAY_H

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
untyped_array_s *untyped_array_new();

#define array_release(array) \
    untyped_array_release(&(array)->_untyped)
void untyped_array_release(untyped_array_s *array);

#define array_element_size(array) \
    sizeof(*(array)->_typed_data)

#define array_length(array) \
    ((array)->_untyped.length / array_element_size(array))

#define array_set_length(array, new_length) \
    untyped_array_set_length( \
        &(array)->_untyped, \
        (new_length) * array_element_size(array))
void untyped_array_set_length(untyped_array_s *array, size_t new_length);

#define array_ensure_capacity(array, min_capacity) \
    untyped_array_ensure_capacity( \
        &(array)->_untyped, \
        (min_capacity) * array_element_size(array))
void untyped_array_ensure_capacity(untyped_array_s *array, size_t min_capacity);

#define array_set_capacity(array, new_capacity) \
    untyped_array_set_length( \
        &(array)->_untyped, \
        (new_capacity) * array_element_size)
void untyped_array_set_capacity(untyped_array_s *array, size_t new_capacity);

#define array_add(array, new_value) \
    (array_set_length(array, array_length(array) + 1), \
     array_set(array, array_length(array)-1, new_value))

#define array_get(array, index) \
    ((array)->_typed_data[index])

#define array_set(array, index, value) \
    ((array)->_typed_data[index] = (value))

#define array_get_ptr(array) \
    ((array)->_typed_data)

#define array_qsort(array, cmp) \
    untyped_array_qsort(&(array)->_untyped, array_element_size(array), cmp)
void untyped_array_qsort(
    untyped_array_s *array,
    size_t element_size,
    int (*cmp)(const void *, const void *));

#define array_filter(array, keep) \
    untyped_array_filter(&(array)->_untyped, array_element_size(array), keep)
void untyped_array_filter(
    untyped_array_s *array,
    size_t element_size,
    int (*keep)(void *));

#endif
