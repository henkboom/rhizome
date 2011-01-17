#include "array.h"

#include <assert.h>
#include <string.h>

struct _untyped_array_s
{
    size_t capacity;
    size_t length;
    void **data;
};

untyped_array_s *untyped_array_new()
{
    untyped_array_s *array = (untyped_array_s*)malloc(sizeof(untyped_array_s));
    array->capacity = 0;
    array->length = 0;
    array->data = NULL;
    return array;
}

void untyped_array_release(untyped_array_s *array)
{
    assert(array);
    untyped_array_set_capacity(array, 0);
    free(array);
}

void untyped_array_set_length(untyped_array_s *array, size_t new_length)
{
    assert(array);

    untyped_array_ensure_capacity(array, new_length);
    array->length = new_length;
}

void untyped_array_ensure_capacity(untyped_array_s *array, size_t min_capacity)
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

        untyped_array_set_capacity(array, new_capacity);
    }
}

void untyped_array_set_capacity(untyped_array_s *array, size_t new_capacity)
{
    assert(array);

    array->capacity = new_capacity;

    if(array->length > new_capacity)
    {
        array->length = new_capacity;
    }

    if(new_capacity != 0 || array->data != NULL)
    {
        array->data = realloc(array->data, new_capacity);
    }
}

void untyped_array_qsort(
    untyped_array_s *array,
    size_t element_size,
    int (*cmp)(const void *, const void *))
{
    assert(array);
    assert(cmp);
    assert(array->length % element_size == 0);

    if(array->length > 0)
    {
        qsort(array->data, array->length / element_size, element_size, cmp);
    }
}

void untyped_array_filter(
    untyped_array_s *array,
    size_t element_size,
    int (*keep)(void *))
{
    assert(array->length % element_size == 0);

    void *src = array->data;
    void *dst = array->data;
    void *end = array->data + array->length;

    while(src != end && keep(src))
    {
        src += element_size;
        dst += element_size;
    }
    while(src != end)
    {
        if(keep(src))
        {
            memcpy(dst, src, element_size);
            dst += element_size;
        }
        src += element_size;
    }

    untyped_array_set_length(array, dst - array->data);
}
