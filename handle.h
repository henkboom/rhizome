#ifndef RHIZOME__HANDLE_H
#define RHIZOME__HANDLE_H

typedef long long unsigned int handle_id_t;
typedef long long unsigned int handle_index_t;

typedef struct
{
    handle_id_t id;
    handle_index_t index;
} untyped_handle_s;

#define define_handle_type(handle_type, value_type) \
    typedef struct \
    { \
        untyped_handle_s _handle; \
        value_type *_type[0]; \
    } handle_type

define_handle_type(void_h, void);

#define handle_new(dest, value) \
    (1 ? ((dest)->_handle = handle_new_untyped((void *)(value)), (void)0) \
       : ((dest)->_type[0] = (value), (void)0))
untyped_handle_s handle_new_untyped(void *value);

extern untyped_handle_s null_untyped_handle;
#define handle_reset(dest) \
    ((dest)->_handle = null_untyped_handle)

#define null_handle(type) (*(type*)(&null_untyped_handle))

#define handle_release(handle) \
    (handle_release_untyped((handle)._handle))
void handle_release_untyped(untyped_handle_s handle);

#define handle_get(handle) \
    ( (__typeof((handle)._type[0])) handle_get_untyped((handle)._handle) )
void * handle_get_untyped(untyped_handle_s handle);

#define handle_live(handle) \
    (handle_get(handle) != NULL)

#endif
