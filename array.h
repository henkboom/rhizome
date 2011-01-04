#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>

typedef struct _array_s array_s;

array_s *array_new();
void array_release(array_s *array);

size_t array_length(array_s *array);
void array_set_length(array_s *array, size_t new_length);
void array_ensure_capacity(array_s *array, size_t min_capacity);
void array_set_capacity(array_s *array, size_t new_capacity);
void array_add(array_s *array, void *new_element);
void * array_get(array_s *array, size_t index);
void array_set(array_s *array, size_t index, void *value);
void ** array_get_ptr(array_s *array);

#endif
