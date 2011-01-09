#include "game.h"

#include <assert.h>
#include <string.h>

#include "array.h"

//// Predefined Types /////////////////////////////////////////////////////////
// (damn one-pass compiler...)

struct _game_s
{
    entity_id_t next_entity_id;
    array_s *entities;

    component_id_t next_component_id;
    array_s *components;

    array_s *buffer_records;

    array_s *messages;
    array_s *subscriptions;
};

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
    void *data;
};

static component_s * component_new(
    game_s *game,
    entity_h entity,
    void *data)
{
    assert(game);

    component_s *component = malloc(sizeof(component_s));
    component->entity = entity;
    component->id = game->next_component_id++;
    component->data = data;

    return component;
}

static void component_release(component_s *component)
{
    free(component);
}

//// Message //////////////////////////////////////////////////////////////////

typedef struct
{
    component_h to;
    char *name;
    void *content;
} message_s;

static message_s * message_new(component_h to, const char *name, size_t len)
{
    assert(name);

    message_s *message = malloc(sizeof(message_s));

    message->to = to;
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

static message_s *message_new_broadcast(const char *name, size_t len)
{
    component_h null_component;
    handle_reset(&null_component);
    return message_new(null_component, name, len);
}

static void message_release(message_s *message)
{
    assert(message);

    free((void *)message->name);
    free(message->content);
    free(message);
}

//// Subscription /////////////////////////////////////////////////////////////

typedef struct
{
    const char *name;
    component_h subscriber;
    message_handler_f handler;
} subscription_s;

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

typedef struct
{
    component_h owner;
    void *source;
    size_t size;
    buffer_updater_f update_function;
    void *buffer;
} buffer_record_s;

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
    buffer_record->buffer = malloc(size);

    return buffer_record;
}

static void buffer_record_release(buffer_record_s *buffer_record)
{
    assert(buffer_record);

    free(buffer_record->buffer);
    free(buffer_record);
}

//// Game /////////////////////////////////////////////////////////////////////

game_s * game_new()
{
    game_s *game = malloc(sizeof(game_s));

    // 0 is the null entity
    game->next_entity_id = 1;
    game->entities = array_new();

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

    for(i = 0; i < array_length(game->components); i++)
    {
        component_release(array_get(game->components, i));
    }
    array_release(game->components);

    for(i = 0; i < array_length(game->entities); i++)
    {
        entity_release(array_get(game->entities, i));
    }
    array_release(game->entities);

    for(i = 0; i < array_length(game->buffer_records); i++)
    {
        buffer_record_release(array_get(game->buffer_records, i));
    }
    array_release(game->buffer_records);


    // if there are messages in the queue something is very wrong
    assert(array_length(game->messages) == 0);
    array_release(game->messages);

    for(i = 0; i < array_length(game->subscriptions); i++)
    {
        subscription_release(array_get(game->subscriptions, i));
    }
    array_release(game->subscriptions);
    free(game);
}

entity_h game_add_entity(game_s *game)
{
    assert(game);

    entity_s *entity = entity_new(game->next_entity_id++);

    array_add(game->entities, entity);


    entity_h handle;
    handle_new(&handle, entity);
    return handle;
}

void game_remove_entity(game_s *game, entity_h entity)
{
    assert(0 && "unimplemented");
}

component_h game_add_component(game_s *game, entity_h entity, void *data)
{
    assert(game);

    component_s *component = component_new(game, entity, data);

    array_add(game->components, component);

    component_h handle;
    handle_new(&handle, component);
    return handle;
}

static void default_buffer_updater(
    void *component_data,
    void *buffer,
    void *source,
    size_t array_size)
{
    memcpy(buffer, source, array_size);
}

const void *game_add_buffer(
    game_s *game,
    component_h owner,
    void *source,
    size_t size)
{
    return game_add_buffer_with_updater(
        game, owner, source, size, default_buffer_updater);
}

const void *game_add_buffer_with_updater(
    game_s *game,
    component_h owner,
    void *source,
    size_t size,
    buffer_updater_f update_function)
{
    buffer_record_s *buffer_record =
        buffer_record_new(owner, source, size, update_function);

    array_add(game->buffer_records, buffer_record);
    update_function(
        handle_get(owner)->data,
        buffer_record->buffer,
        buffer_record->source,
        size);

    return buffer_record->buffer;
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

    message_s *message = message_new_broadcast(name, len);

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

    message_s *message = message_new(to, name, len);

    array_add(game->messages, message);

    return message->content;
}

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
    array_s *array,
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

    qsort_array(game->messages, message_cmp);
    qsort_array(game->subscriptions, subscription_cmp);

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
            if(handle_get(message->to) == NULL) // broadcast message
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
                        message->name,
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
                        message->name,
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

static void game_update_buffers(game_s *game)
{
    int i;
    int record_count = array_length(game->buffer_records);
    for(i = 0; i < record_count; i++)
    {
        buffer_record_s *record = array_get(game->buffer_records, i);
        record->update_function(
            handle_get(record->owner)->data,
            record->buffer,
            record->source,
            record->size);
    }
}

void game_tick(game_s *game)
{
    assert(game);

    game_broadcast_message(game, "tick", 0);

    while(array_length(game->messages) != 0)
    {
        game_dispatch_messages(game);
        game_update_buffers(game);
    }
}
