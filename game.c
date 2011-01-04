#include "game.h"

#include <assert.h>
#include <string.h>

#include "buffer.h"

//// Predefined Types /////////////////////////////////////////////////////////
// (damn one-pass compiler...)

struct _game_s
{
    entity_id_t next_entity_id;
    buffer_s *entities;

    component_id_t next_component_id;
    buffer_s *components; // holds component_data_s pointers

    buffer_s *buffer_records;

    buffer_s *messages;
    buffer_s *subscriptions;
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

typedef struct
{
    entity_s *entity;
    component_id_t id;
    void *data;
} component_data_s;

static component_data_s * component_data_new(
    game_s *game,
    entity_s *entity,
    void *data)
{
    assert(game);

    component_data_s *component_data = malloc(sizeof(component_data_s));
    component_data->entity = entity;
    component_data->id = game->next_component_id++;
    component_data->data = data;

    return component_data;
}

static void component_data_release(component_data_s *component_data)
{
    free(component_data);
}

static component_data_s * get_component_data(
    game_s *game,
    component_s component)
{
    assert(component.index <= buffer_length(game->components));
    component_data_s *component_data =
        buffer_get(game->components, component.index);

    if(component_data->id == component.id)
    {
        return component_data;
    }
    else
    {
        return NULL;
    }
}

//// Message //////////////////////////////////////////////////////////////////

typedef struct
{
    component_s to;
    char *name;
    void *content;
} message_s;

static message_s * message_new(component_s to, const char *name, size_t len)
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
    component_s null_component;
    null_component.id = 0;
    null_component.index = 0;
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
    component_s subscriber;
    message_handler_f handler;
} subscription_s;

static subscription_s * subscription_new(
    const char *name,
    component_s subscriber,
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
    component_s owner;
    void *source;
    size_t size;
    buffer_updater_f update_function;
    void *buffer;
} buffer_record_s;

static buffer_record_s * buffer_record_new(
    component_s owner,
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

    game->next_entity_id = 1;
    game->entities = buffer_new();

    game->next_component_id = 1;
    game->components = buffer_new();

    game->buffer_records = buffer_new();

    game->messages = buffer_new();
    game->subscriptions = buffer_new();

    return game;
}

void game_release(game_s *game)
{
    assert(game);

    int i;

    for(i = 0; i < buffer_length(game->components); i++)
    {
        component_data_release(buffer_get(game->components, i));
    }
    buffer_release(game->components);

    for(i = 0; i < buffer_length(game->entities); i++)
    {
        entity_release(buffer_get(game->entities, i));
    }
    buffer_release(game->entities);

    for(i = 0; i < buffer_length(game->buffer_records); i++)
    {
        buffer_record_release(buffer_get(game->buffer_records, i));
    }
    buffer_release(game->buffer_records);


    // if there are messages in the queue something is very wrong
    assert(buffer_length(game->messages) == 0);
    buffer_release(game->messages);

    for(i = 0; i < buffer_length(game->subscriptions); i++)
    {
        subscription_release(buffer_get(game->subscriptions, i));
    }
    buffer_release(game->subscriptions);
    free(game);
}

entity_s * game_add_entity(game_s *game)
{
    assert(game);

    entity_s *entity = entity_new(game->next_entity_id++);

    buffer_add(game->entities, entity);

    return entity;
}

component_s game_add_component(game_s *game, entity_s *entity, void *data)
{
    assert(game);

    component_data_s *component_data = component_data_new(game, entity, data);

    // get an empty component slot
    // do this faster please
    int i;
    for(i = 0; i < buffer_length(game->components); i++)
    {
        if(buffer_get(game->components, i) == NULL)
            break;
    }
    if(i == buffer_length(game->components))
    {
        buffer_add(game->components, NULL);
    }

    // assign the component slot
    buffer_set(game->components, i, component_data);

    component_s component;
    component.id = component_data->id;
    component.index = i;
    return component;
}

static void default_buffer_updater(
    void *component_data,
    void *buffer,
    void *source,
    size_t buffer_size)
{
    memcpy(buffer, source, buffer_size);
}

const void *game_add_buffer(
    game_s *game,
    component_s owner,
    void *source,
    size_t size)
{
    return game_add_buffer_with_updater(
        game, owner, source, size, default_buffer_updater);
}

const void *game_add_buffer_with_updater(
    game_s *game,
    component_s owner,
    void *source,
    size_t size,
    buffer_updater_f update_function)
{
    buffer_record_s *buffer_record =
        buffer_record_new(owner, source, size, update_function);

    buffer_add(game->buffer_records, buffer_record);
    update_function(
        get_component_data(game, owner)->data,
        buffer_record->buffer,
        buffer_record->source,
        size);

    return buffer_record->buffer;
}

void game_subscribe(
    game_s *game,
    component_s subscriber,
    const char *name,
    void (*handler)())
{
    assert(game);
    assert(name);
    assert(handler);

    subscription_s *subscription = subscription_new(name, subscriber, handler);
    buffer_add(game->subscriptions, subscription);
}

void *game_broadcast_message(
    game_s *game,
    const char *name,
    size_t len)
{
    assert(game);
    assert(name);

    message_s *message = message_new_broadcast(name, len);

    buffer_add(game->messages, message);

    return message->content;
}

void *game_send_message(
    game_s *game,
    component_s to,
    const char *name,
    size_t len)
{
    assert(game);
    assert(name);

    message_s *message = message_new(to, name, len);

    buffer_add(game->messages, message);

    return message->content;
}

static int message_cmp(const void *a, const void *b)
{
    const message_s *m_a = *(const message_s **)a;
    const message_s *m_b = *(const message_s **)b;

    int retval = strcmp(m_a->name, m_b->name);
    if(retval == 0)
    {
        retval = m_a->to.id < m_b->to.id ? -1 :
                 m_a->to.id == m_b->to.id ? 0 : 1;
    }

    return retval;
}

static int subscription_cmp(const void *a, const void *b)
{
    const subscription_s *s_a = *(const subscription_s **)a;
    const subscription_s *s_b = *(const subscription_s **)b;

    int retval = strcmp(s_a->name, s_b->name);
    if(retval == 0)
    {
        retval = s_a->subscriber.id <  s_b->subscriber.id ? -1 :
                 s_a->subscriber.id == s_b->subscriber.id ? 0 : 1;
    }

    return retval;
}

static void qsort_buffer(
    buffer_s *buffer,
    int (*cmp)(const void *, const void *))
{
    assert(buffer);
    assert(cmp);

    if(buffer_length(buffer) > 0)
    {
        qsort(
            buffer_get_ptr(buffer),
            buffer_length(buffer),
            sizeof(void *), 
            cmp);
    }
}

static void game_dispatch_messages(game_s *game)
{
    assert(game);
    int subscription_count = buffer_length(game->subscriptions);
    int message_count = buffer_length(game->messages);
    int message_index = 0;

    qsort_buffer(game->messages, message_cmp);
    qsort_buffer(game->subscriptions, subscription_cmp);

    while(message_index < message_count)
    {
        message_s *message = buffer_get(game->messages, message_index);

        int subscription_index = 0;
        subscription_s *subscription =
            buffer_get(game->subscriptions, subscription_index);
        int cmp = 1;

        // skip subscriptions which have names less than the current message
        while(subscription_index < subscription_count &&
              (cmp = strcmp(subscription->name, message->name)) < 0)
        {
            subscription_index++;
            subscription = buffer_get(game->subscriptions, subscription_index);
        }

        // if we've found a match then handle relevant subscriptions
        if(cmp == 0)
        {
            if(message->to.id == 0) // broadcast message
            {
                // leave subscription_index alone since there may be more
                // broadcast messages of the same type coming up
                int broadcast_to = subscription_index;
                subscription = buffer_get(game->subscriptions, broadcast_to);
                while(broadcast_to < subscription_count &&
                      strcmp(subscription->name, message->name) == 0)
                {
                    subscription->handler(
                        get_component_data(game, subscription->subscriber)
                            ->data,
                        message->name,
                        message->content);

                    broadcast_to++;
                    if(broadcast_to < buffer_length(game->subscriptions))
                    {
                        subscription =
                            buffer_get(game->subscriptions, broadcast_to);
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
                      subscription->subscriber.id < message->to.id)
                {
                    subscription_index++;
                    subscription = buffer_get(
                        game->subscriptions, subscription_index);
                }

                if(cmp == 0 && subscription->subscriber.id == message->to.id)
                {
                    subscription->handler(
                        get_component_data(game, subscription->subscriber)
                            ->data,
                        message->name,
                        message->content);
                }
            }
        }

        message_release(message);
        message_index++;
    }

    int new_message_count = buffer_length(game->messages) - message_count;
    if(new_message_count > 0)
    {
        memmove(
            buffer_get_ptr(game->messages),
            buffer_get_ptr(game->messages) + message_count,
            new_message_count * sizeof(void *));
    }
    buffer_set_length(game->messages, new_message_count);
}

static void game_update_buffers(game_s *game)
{
    int i;
    int record_count = buffer_length(game->buffer_records);
    for(i = 0; i < record_count; i++)
    {
        buffer_record_s *record = buffer_get(game->buffer_records, i);
        record->update_function(
            get_component_data(game, record->owner)->data,
            record->buffer,
            record->source,
            record->size);
    }
}

void game_tick(game_s *game)
{
    assert(game);

    game_broadcast_message(game, "tick", 0);

    while(buffer_length(game->messages) != 0)
    {
        game_dispatch_messages(game);
        game_update_buffers(game);
    }
}
