#include "handle.h"

typedef struct
{
    handle_id_t id;
    void *value;
} handle_slot_s;

static handle_slot_s *slots;



untyped_handle_s handle_new_untyped(void *value)
{
    // to do
}

void handle_release_untyped(untyped_handle_s handle)
{
    // to do
}

void * handle_get_untyped(untyped_handle_s handle)
{
    // to do
}

