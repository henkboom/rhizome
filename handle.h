#ifndef HANDLE_H
#define HANDLE_H

typedef long long unsigned int handle_id_t;
typedef long long unsigned int handle_index_t;

typedef struct
{
    handle_id id;
    handle_index_t index;
} untyped_handle_s;

#define define_handle_type(handle_type, value_type) \
    typedef struct \
    { \
        untyped_handle_s _handle; \
        value_type _type[0]; \
    } handle_type

// check that this works, and throws an error if the type is wrong
#define handle_new(dest, value) \
    (1 ? (*(untyped_handle_s *)(dest) = handle_new_untyped(value)) \
       : (dest._type[0] = *(value)))
untyped_handle_s handle_new_untyped(void *value);


#define handle_release_untyped(handle) \
    (handle_release_untyped(handle._handle))
void handle_release_untyped(untyped_handle_s handle);

#define handle_get(handle) \
    ( (typeof((handle)._type[0])*) handle_get_untyped((handle)._handle) )
void * handle_get_untyped(untyped_handle_s handle);

#endif
