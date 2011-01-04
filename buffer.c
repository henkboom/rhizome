#include "buffer.h"

#include <assert.h>

struct _buffer_s
{
    size_t capacity;
    size_t length;
    void **data;
};

buffer_s *buffer_new()
{
    buffer_s *buffer = (buffer_s*)malloc(sizeof(buffer_s));
    buffer->capacity = 0;
    buffer->length = 0;
    buffer->data = NULL;
    return buffer;
}

void buffer_release(buffer_s *buffer)
{
    assert(buffer);
    buffer_set_capacity(buffer, 0);
    free(buffer);
}

size_t buffer_length(buffer_s *buffer)
{
    return buffer->length;
}

void buffer_set_length(buffer_s *buffer, size_t new_length)
{
    assert(buffer);

    buffer_ensure_capacity(buffer, new_length);
    buffer->length = new_length;
}

void buffer_ensure_capacity(buffer_s *buffer, size_t min_capacity)
{
    assert(buffer);

    if(buffer->capacity < min_capacity)
    {
        size_t new_capacity = buffer->capacity;

        if(new_capacity == 0)
        {
            new_capacity = 1;
        }

        while(new_capacity < min_capacity)
        {
            new_capacity *= 2;
        }

        buffer_set_capacity(buffer, new_capacity);
    }
}

void buffer_set_capacity(buffer_s *buffer, size_t new_capacity)
{
    assert(buffer);

    buffer->capacity = new_capacity;

    if(buffer->length > new_capacity)
    {
        buffer->length = new_capacity;
    }

    if(new_capacity != 0 || buffer->data != NULL)
    {
        buffer->data = realloc(buffer->data, new_capacity * sizeof(void *));
    }
}

void buffer_add(buffer_s *buffer, void *new_element)
{
    assert(buffer);

    buffer_set_length(buffer, buffer->length + 1);
    buffer->data[buffer->length-1] = new_element;
}

void * buffer_get(buffer_s *buffer, size_t index)
{
    assert(buffer);
    assert(index < buffer->length);

    return buffer->data[index];
}

void buffer_set(buffer_s *buffer, size_t index, void *value)
{
    assert(buffer);
    assert(index < buffer->length);

    buffer->data[index] = value;
}

void ** buffer_get_ptr(buffer_s *buffer)
{
    assert(buffer);

    return buffer->data;
}
