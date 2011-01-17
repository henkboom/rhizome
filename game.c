#include "game.h"

#include <assert.h>
#include <string.h>

#include "array.h"

//// Predefined Stuff /////////////////////////////////////////////////////////
// (damn one-pass compiler...)

typedef struct _message_s message_s;
typedef struct _subscription_s subscription_s;
typedef struct _buffer_record_s buffer_record_s;;

struct _game_s
{
    entity_id_t next_entity_id;
    array_of(entity_h) entities;
    array_of(entity_h) entities_to_remove;

    component_id_t next_component_id;
    array_of(component_h) components;

    array_of(buffer_record_s *) buffer_records;

    array_of(message_s *) messages;
    array_of(subscription_s *) subscriptions;
};

static void game_handle_removals(game_s *game);

//// Entity ///////////////////////////////////////////////////////////////////

struct _entity_s
{
    entity_id_t id;
};

static entity_s * entity_new(entity_id_t id)
{
    entity_s *entity = malloc(sizeof(entity));
    entity->id = id;

    return entity;
}

static void entity_release(entity_s *entity)
{
    free(entity);
}

//// Component Data ///////////////////////////////////////////////////////////

struct _component_s
{
    entity_h entity;
    component_id_t id;
    component_release_f release_func;
    void *data;
};

static component_s * component_new(
    game_s *game,
    entity_h entity,
    component_release_f release_func)
{
    assert(game);

    component_s *component = malloc(sizeof(component_s));
    component->entity = entity;
    component->id = game->next_component_id++;
    component->release_func = release_func;
    component->data = NULL;

    return component;
}

static void component_release(component_s *component)
{
    free(component);
}

//// Message //////////////////////////////////////////////////////////////////

struct _message_s
{
    component_h to;
    int is_broadcast;
    char *name;
    void *content;
};

static message_s * message_new(
    component_h to,
    int is_broadcast,
    const char *name,
    size_t len)
{
    assert(name);

    message_s *message = malloc(sizeof(message_s));

    message->to = to;
    message->is_broadcast = is_broadcast;
    message->name = malloc(strlen(name) + 1);
    strcpy(message->name, name);
    if(len == 0)
    {
        message->content = NULL;
    }
    else
    {
        message->content = malloc(len);
    }

    return message;
}

static void message_release(message_s *message)
{
    assert(message);

    free((void *)message->name);
    free(message->content);
    free(message);
}

//// Subscription /////////////////////////////////////////////////////////////

struct _subscription_s
{
    const char *name;
    component_h subscriber;
    message_handler_f handler;
};

static subscription_s * subscription_new(
    const char *name,
    component_h subscriber,
    message_handler_f handler)
{
    assert(name);
    assert(handler);

    subscription_s *subscription = malloc(sizeof(subscription_s));

    char *name_copy = malloc(strlen(name) + 1);
    strcpy(name_copy, name);
    subscription->name = name_copy;
    subscription->subscriber = subscriber;
    subscription->handler = handler;

    return subscription;
}

static void subscription_release(subscription_s *subscription)
{
    assert(subscription);

    free((void *)subscription->name);
    free(subscription);
}

//// Buffer Record ////////////////////////////////////////////////////////////

struct _buffer_record_s
{
    component_h owner;
    void *source;
    size_t size;
    buffer_updater_f update_function;
    void_h buffer_handle
};

static buffer_record_s * buffer_record_new(
    component_h owner,
    void *source,
    size_t size,
    buffer_updater_f update_function)
{
    buffer_record_s *buffer_record = malloc(sizeof(buffer_record_s));

    buffer_record->owner = owner;
    buffer_record->source = source;
    buffer_record->size = size;
    buffer_record->update_function = update_function;
    handle_new(&buffer_record->buffer_handle, malloc(size));

    return buffer_record;
}

static void buffer_record_release(buffer_record_s *buffer_record)
{
    assert(buffer_record);

    free(handle_get(buffer_record->buffer_handle));
    handle_release(buffer_record->buffer_handle);
    free(buffer_record);
}

//// Game /////////////////////////////////////////////////////////////////////

game_s * game_new()
{
    game_s *game = malloc(sizeof(game_s));

    // 0 is the null entity
    game->next_entity_id = 1;
    game->entities = array_new();
    game->entities_to_remove = array_new();

    // 0 is the null component
    game->next_component_id = 1;
    game->components = array_new();

    game->buffer_records = array_new();

    game->messages = array_new();
    game->subscriptions = array_new();

    return game;
}

void game_release(game_s *game)
{
    assert(game);

    int i;
    for(i = 0; i < array_length(game->entities); i++)
    {
        game_remove_entity(game, array_get(game->entities, i));
    }
    game_handle_removals(game);

    assert(array_length(game->entities) == 0);
    array_release(game->entities);

    array_release(game->entities_to_remove);

    assert(array_length(game->components) == 0);
    array_release(game->components);

    assert(array_length(game->buffer_records) == 0);
    array_release(game->buffer_records);

    // if there are messages in the queue something is very wrong
    assert(array_length(game->messages) == 0);
    array_release(game->messages);

    assert(array_length(game->subscriptions) == 0);
    array_release(game->subscriptions);

    free(game);
}

entity_h game_add_entity(game_s *game)
{
    assert(game);

    entity_h handle;
    handle_new(&handle, entity_new(game->next_entity_id++));
    array_add(game->entities, handle);

    return handle;
}

void game_remove_entity(game_s *game, entity_h entity)
{
    assert(handle_get(entity));
    array_add(game->entities_to_remove, entity);
}

component_h game_add_component(game_s *game, entity_h entity, void *data)
{
    assert(game);
    assert(handle_get(entity) != NULL);

    component_h handle;
    handle_new(&handle, component_new(game, entity, data));
    array_add(game->components, handle);

    return handle;
}

void component_set_data(component_h component, void *data)
{
    component_s *c = handle_get(component);
    assert(c != NULL);
    assert(c->data == NULL);
    c->data = data;
}

static void default_buffer_updater(
    void *component_data,
    void *buffer,
    void *source,
    size_t array_size)
{
    memcpy(buffer, source, array_size);
}

void game_add_buffer(
    game_s *game,
    component_h owner,
    void *source,
    size_t size,
    void_h *out)
{
    game_add_buffer_with_updater(
        game, owner, source, size, default_buffer_updater, out);
}

void game_add_buffer_with_updater(
    game_s *game,
    component_h owner,
    void *source,
    size_t size,
    buffer_updater_f update_function,
    void_h *out)
{
    buffer_record_s *buffer_record =
        buffer_record_new(owner, source, size, update_function);

    array_add(game->buffer_records, buffer_record);
    update_function(
        handle_get(owner)->data,
        handle_get(buffer_record->buffer_handle),
        buffer_record->source,
        size);

    *out = buffer_record->buffer_handle;
}

void game_subscribe(
    game_s *game,
    component_h subscriber,
    const char *name,
    void (*handler)())
{
    assert(game);
    assert(name);
    assert(handler);

    subscription_s *subscription = subscription_new(name, subscriber, handler);
    array_add(game->subscriptions, subscription);
}

void *game_broadcast_message(
    game_s *game,
    const char *name,
    size_t len)
{
    assert(game);
    assert(name);

    message_s *message = message_new(null_handle(component_h), 1, name, len);

    array_add(game->messages, message);

    return message->content;
}

void *game_send_message(
    game_s *game,
    component_h to,
    const char *name,
    size_t len)
{
    assert(game);
    assert(name);

    message_s *message = message_new(to, 0, name, len);

    array_add(game->messages, message);

    return message->content;
}

//// Message Dispatch ////

#define CMP(a, b) ((a) < (b) ? -1 : ((a) == (b) ? 0 : 1))

static int component_cmp(const component_s *a, const component_s *b)
{
    if(a == NULL || b == NULL)
        return CMP(a != NULL, b != NULL);
    else
        return CMP(a->id, b->id);
}

static int message_cmp(const void *a, const void *b)
{
    const message_s *m_a = *(const message_s **)a;
    const message_s *m_b = *(const message_s **)b;

    int retval = strcmp(m_a->name, m_b->name);
    if(retval == 0)
        retval = component_cmp(handle_get(m_a->to), handle_get(m_b->to));

    return retval;
}

static int subscription_cmp(const void *a, const void *b)
{
    const subscription_s *s_a = *(const subscription_s **)a;
    const subscription_s *s_b = *(const subscription_s **)b;

    int retval = strcmp(s_a->name, s_b->name);
    if(retval == 0)
        retval = component_cmp(
            handle_get(s_a->subscriber), handle_get(s_b->subscriber));

    return retval;
}

static void qsort_array(
    array_of(void *) array,
    int (*cmp)(const void *, const void *))
{
    assert(array);
    assert(cmp);

    if(array_length(array) > 0)
    {
        qsort(
            array_get_ptr(array),
            array_length(array),
            sizeof(void *), 
            cmp);
    }
}

static void game_dispatch_messages(game_s *game)
{
    assert(game);
    int subscription_count = array_length(game->subscriptions);
    int message_count = array_length(game->messages);
    int message_index = 0;

    qsort_array((void *)game->messages, message_cmp);
    qsort_array((void *)game->subscriptions, subscription_cmp);

    while(message_index < message_count)
    {
        message_s *message = array_get(game->messages, message_index);

        int subscription_index = 0;
        subscription_s *subscription =
            array_get(game->subscriptions, subscription_index);
        int cmp = 1;

        // skip subscriptions which have names less than the current message
        while(subscription_index < subscription_count &&
              (cmp = strcmp(subscription->name, message->name)) < 0)
        {
            subscription_index++;
            subscription = array_get(game->subscriptions, subscription_index);
        }

        // if we've found a match then handle relevant subscriptions
        if(cmp == 0)
        {
            if(message->is_broadcast) // broadcast message
            {
                // leave subscription_index alone since there may be more
                // broadcast messages of the same type coming up
                int broadcast_to = subscription_index;
                subscription = array_get(game->subscriptions, broadcast_to);
                while(broadcast_to < subscription_count &&
                      strcmp(subscription->name, message->name) == 0)
                {
                    subscription->handler(
                        handle_get(subscription->subscriber)->data,
                        message->content);

                    broadcast_to++;
                    if(broadcast_to < array_length(game->subscriptions))
                    {
                        subscription =
                            array_get(game->subscriptions, broadcast_to);
                    }
                }
            }
            else // directed message
            {
                // skip subscribers with ids too low for the current
                // message
                cmp = 1;
                while(subscription_index < subscription_count &&
                      (cmp = strcmp(subscription->name, message->name)) == 0 &&
                      component_cmp(
                          handle_get(subscription->subscriber),
                          handle_get(message->to)))
                {
                    subscription_index++;
                    subscription = array_get(
                        game->subscriptions, subscription_index);
                }

                if(cmp == 0 && handle_get(subscription->subscriber) ==
                   handle_get(message->to))
                {
                    subscription->handler(
                        handle_get(subscription->subscriber)->data,
                        message->content);
                }
            }
        }

        message_release(message);
        message_index++;
    }

    int new_message_count = array_length(game->messages) - message_count;
    if(new_message_count > 0)
    {
        memmove(
            array_get_ptr(game->messages),
            array_get_ptr(game->messages) + message_count,
            new_message_count * sizeof(void *));
    }
    array_set_length(game->messages, new_message_count);
}

//// Entity Removal Processing ////

static int is_not_null_pointer(void *value)
{
    return *(void **)value != NULL;
}

static int is_not_null_handle(void *value)
{
    return handle_get(*(void_h*)value) != NULL;
}

static int filter(void *array, int count, int item_size, int (*keep)(void *))
{
    void *src = array;
    void *dst = array;
    void *end = array + count * item_size;

    while(src != end && keep(src))
    {
        src += item_size;
        dst += item_size;
    }
    while(src != end)
    {
        if(keep(src))
        {
            memcpy(dst, src, item_size);
            dst += item_size;
        }
        src += item_size;
    }

    return (dst - array) / item_size;
}

#define FILTER_ARRAY(array, keep) \
    array_set_length(array, \
        filter(array_get_ptr(array), \
               array_length(array), \
               sizeof(array_get(array, 0)), \
               keep))

static void game_handle_removals(game_s *game)
{
    int i;

    //while(array_length(game->entities_to_remove) > 0)
    {
        // remove dead entities
        for(i = 0; i < array_length(game->entities_to_remove); i++)
        {
            entity_h handle = array_get(game->entities_to_remove, i);
            entity_s *entity = handle_get(handle);
            if(entity != NULL)
            {
                entity_release(entity);
                handle_release(handle);
                //printf("removed entity\n");
            }
        }
        array_set_length(game->entities_to_remove, 0);
        //printf("before: %lu entities\n", array_length(game->entities));
        FILTER_ARRAY(game->entities, is_not_null_handle);
        //printf("after: %lu entities\n", array_length(game->entities));

        // remove dead components
        for(i = 0; i < array_length(game->components); i++)
        {
            component_h handle = array_get(game->components, i);
            component_s *component = handle_get(handle);
            if(handle_get(component->entity) == NULL)
            {
                if(component->release_func != NULL)
                {
                    component->release_func(component->data);
                }
                component_release(component);
                handle_release(handle);
                //printf("removed component\n");
            }
        }
        //printf("before: %lu components\n", array_length(game->components));
        FILTER_ARRAY(game->components, is_not_null_handle);
        //printf("after: %lu components\n", array_length(game->components));

        for(i = 0; i < array_length(game->buffer_records); i++)
        {
            buffer_record_s *record = array_get(game->buffer_records, i);
            if(handle_get(record->owner) == NULL)
            {
                buffer_record_release(record);
                array_set(game->buffer_records, i, NULL);
                //printf("removed buffer\n");
            }
        }
        //printf("before: %lu buffers\n", array_length(game->buffer_records));
        FILTER_ARRAY(game->buffer_records, is_not_null_pointer);
        //printf("after: %lu buffers\n", array_length(game->buffer_records));

        for(i = 0; i < array_length(game->subscriptions); i++)
        {
            subscription_s *subscription = array_get(game->subscriptions, i);
            if(handle_get(subscription->subscriber) == NULL)
            {
                subscription_release(subscription);
                array_set(game->subscriptions, i, NULL);
                //printf("removed subscription\n");
            }
        }
        //printf("before: %lu subs\n", array_length(game->subscriptions));
        FILTER_ARRAY(game->subscriptions, is_not_null_pointer);
        //printf("after: %lu subs\n", array_length(game->subscriptions));
    }
}

//// Buffer Updates ////

static void game_update_buffers(game_s *game)
{
    int i;
    int record_count = array_length(game->buffer_records);
    for(i = 0; i < record_count; i++)
    {
        buffer_record_s *record = array_get(game->buffer_records, i);
        record->update_function(
            handle_get(record->owner)->data,
            handle_get(record->buffer_handle),
            record->source,
            record->size);
    }
}

void game_tick(game_s *game)
{
    assert(game);

    broadcast_tick(game, 0);

    while(array_length(game->messages) != 0)
    {
        game_dispatch_messages(game);
        game_handle_removals(game);
        game_update_buffers(game);
    }
}
