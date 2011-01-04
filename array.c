#include "array.h"

#include <assert.h>

struct _array_s
{
    size_t capacity;
    size_t length;
    void **data;
};

array_s *array_new()
{
    array_s *array = (array_s*)malloc(sizeof(array_s));
    array->capacity = 0;
    array->length = 0;
    array->data = NULL;
    return array;
}

void array_release(array_s *array)
{
    assert(array);
    array_set_capacity(array, 0);
    free(array);
}

size_t array_length(array_s *array)
{
    return array->length;
}

void array_set_length(array_s *array, size_t new_length)
{
    assert(array);

    array_ensure_capacity(array, new_length);
    array->length = new_length;
}

void array_ensure_capacity(array_s *array, size_t min_capacity)
{
    assert(array);

    if(array->capacity < min_capacity)
    {
        size_t new_capacity = array->capacity;

        if(new_capacity == 0)
        {
            new_capacity = 1;
        }

        while(new_capacity < min_capacity)
        {
            new_capacity *= 2;
        }

        array_set_capacity(array, new_capacity);
    }
}

void array_set_capacity(array_s *array, size_t new_capacity)
{
    assert(array);

    array->capacity = new_capacity;

    if(array->length > new_capacity)
    {
        array->length = new_capacity;
    }

    if(new_capacity != 0 || array->data != NULL)
    {
        array->data = realloc(array->data, new_capacity * sizeof(void *));
    }
}

void array_add(array_s *array, void *new_element)
{
    assert(array);

    array_set_length(array, array->length + 1);
    array->data[array->length-1] = new_element;
}

void * array_get(array_s *array, size_t index)
{
    assert(array);
    assert(index < array->length);

    return array->data[index];
}

void array_set(array_s *array, size_t index, void *value)
{
    assert(array);
    assert(index < array->length);

    array->data[index] = value;
}

void ** array_get_ptr(array_s *array)
{
    assert(array);

    return array->data;
}
