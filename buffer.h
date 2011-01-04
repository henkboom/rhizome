#ifndef BUFFER_H
#define BUFFER_H

#include <stdlib.h>

typedef struct _buffer_s buffer_s;

buffer_s *buffer_new();
void buffer_release(buffer_s *buffer);

size_t buffer_length(buffer_s *buffer);
void buffer_set_length(buffer_s *buffer, size_t new_length);
void buffer_ensure_capacity(buffer_s *buffer, size_t min_capacity);
void buffer_set_capacity(buffer_s *buffer, size_t new_capacity);
void buffer_add(buffer_s *buffer, void *new_element);
void * buffer_get(buffer_s *buffer, size_t index);
void buffer_set(buffer_s *buffer, size_t index, void *value);
void ** buffer_get_ptr(buffer_s *buffer);

#endif
