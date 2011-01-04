#include "handle.h"

#include <assert.h>
#include <stdlib.h>

#define SLOT_COUNT 10000

untyped_handle_s null_untyped_handle = {0, 0};

typedef struct
{
    handle_id_t id;
    void *value;
} handle_slot_s;

static handle_slot_s slots[SLOT_COUNT];

static handle_index_t get_free_index()
{
    static int initted = 0;
    int i;

    if(!initted)
    {
        initted = 1;
        for(i = 0; i < SLOT_COUNT; i++)
        {
            slots[i].id = 0;
            slots[i].value = NULL;
        }
    }

    for(i = 0; i < SLOT_COUNT; i++)
    {
        if(slots[i].value == NULL)
            return i;
    }
    assert(0 && "ran out of handle slots");
}

untyped_handle_s handle_new_untyped(void *value)
{
    handle_index_t index = get_free_index();
    slots[index].id++;
    slots[index].value = value;

    untyped_handle_s handle;
    handle.id = slots[index].id;
    handle.index = index;

    return handle;
}

void handle_release_untyped(untyped_handle_s handle)
{
    slots[handle.index].value = NULL;
}

void * handle_get_untyped(untyped_handle_s handle)
{
    if(slots[handle.index].id == handle.id)
        return slots[handle.index].value;
    else
        return NULL;
}

