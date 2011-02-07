#include "game.h"

#include <assert.h>

#include "array.h"
#include "string.h"

//// Predefined Stuff /////////////////////////////////////////////////////////
// (damn one-pass compiler...)

typedef struct _message_s message_s;
typedef struct _subscription_s subscription_s;
typedef struct _buffer_record_s buffer_record_s;

typedef long long unsigned int component_id_t;

struct _game_s
{

    component_id_t next_component_id;
    array_of(component_h) components;
    array_of(component_h) components_to_remove;

    array_of(buffer_record_s *) buffer_records;

    array_of(message_s *) messages;
    array_of(subscription_s *) subscriptions;
};

static void internal_game_remove_component(game_s *game, component_h component);
static void game_handle_removals(game_s *game);

//// Component Data ///////////////////////////////////////////////////////////

struct _component_s
{
    component_h parent;
    component_id_t id;
    component_release_f release_func;
    void *data;
};

static component_s * component_new(
    game_s *game,
    component_h parent,
    component_release_f release_func)
{
    assert(game);

    component_s *component = malloc(sizeof(component_s));
    component->parent = parent;
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

game_s * game_new(initial_component_f init)
{
    game_s *game = malloc(sizeof(game_s));


    game->next_component_id = 0;
    game->components = array_new();
    game->components_to_remove = array_new();

    game->buffer_records = array_new();

    game->messages = array_new();
    game->subscriptions = array_new();

    game_context_s generic_context;
    generic_context.game = game;
    generic_context.component = null_handle(component_h);

    init(&generic_context, null_handle(component_h));

    return game;
}

void game_release(game_s *game)
{
    assert(game);

    int i;
    for(i = 0; i < array_length(game->components); i++)
    {
        internal_game_remove_component(game, array_get(game->components, i));
    }
    game_handle_removals(game);

    assert(array_length(game->components) == 0);
    array_release(game->components);

    array_release(game->components_to_remove);

    assert(array_length(game->buffer_records) == 0);
    array_release(game->buffer_records);

    // if there are messages in the queue something is very wrong
    assert(array_length(game->messages) == 0);
    array_release(game->messages);

    assert(array_length(game->subscriptions) == 0);
    array_release(game->subscriptions);

    free(game);
}

int game_done(game_s *game)
{
    return array_length(game->components) == 0;
}

static void internal_game_remove_component(game_s *game, component_h component)
{
    assert(game);
    assert(handle_live(component));

    array_add(game->components_to_remove, component);
}

void game_remove_component(game_context_s *context, component_h component)
{
    assert(context);
    assert(handle_live(component));

    internal_game_remove_component(context->game, component);
}

game_context_s game_add_component(
    game_context_s *context,
    component_h parent,
    component_release_f release_func)
{
    assert(context);

    component_h handle;
    handle_new(&handle, component_new(context->game, parent, release_func));
    array_add(context->game->components, handle);

    game_context_s new_context = *context;
    new_context.component = handle;
    return new_context;
}

component_h game_get_self(game_context_s *context)
{
    return context->component;
}

void game_set_component_data(game_context_s *context, void *data)
{
    component_s *c = handle_get(context->component);
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
    game_context_s *context,
    void *source,
    size_t size,
    void_h *out)
{
    game_add_buffer_with_updater(
        context, source, size, default_buffer_updater, out);
}

void game_add_buffer_with_updater(
    game_context_s *context,
    void *source,
    size_t size,
    buffer_updater_f update_function,
    void_h *out)
{
    buffer_record_s *buffer_record =
        buffer_record_new(context->component, source, size, update_function);

    array_add(context->game->buffer_records, buffer_record);
    update_function(
        handle_get(context->component)->data,
        handle_get(buffer_record->buffer_handle),
        buffer_record->source,
        size);

    *out = buffer_record->buffer_handle;
}

void game_subscribe(
    game_context_s *context,
    const char *name,
    void (*handler)())
{
    assert(context);
    assert(name);
    assert(handler);

    subscription_s *subscription =
        subscription_new(name, context->component, handler);
    array_add(context->game->subscriptions, subscription);
}

void *game_broadcast_message(
    game_context_s *context,
    const char *name,
    size_t len)
{
    assert(context);
    assert(name);

    message_s *message = message_new(null_handle(component_h), 1, name, len);

    array_add(context->game->messages, message);

    return message->content;
}

void *game_send_message(
    game_context_s *context,
    component_h to,
    const char *name,
    size_t len)
{
    assert(context);
    assert(name);

    message_s *message = message_new(to, 0, name, len);

    array_add(context->game->messages, message);

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

static void game_dispatch_messages(game_s *game)
{
    assert(game);
    int subscription_count = array_length(game->subscriptions);
    int message_count = array_length(game->messages);
    int message_index = 0;

    array_qsort(game->messages, message_cmp);
    array_qsort(game->subscriptions, subscription_cmp);

    game_context_s context;
    context.game = game;

    while(message_index < message_count)
    {
        message_s *message = array_get(game->messages, message_index);

        int subscription_index = 0;
        subscription_s *subscription = NULL;
        if(subscription_count > 0)
            subscription = array_get(game->subscriptions, subscription_index);
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
                    context.component = subscription->subscriber;
                    subscription->handler(
                        &context,
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
                    context.component = subscription->subscriber;
                    subscription->handler(
                        &context,
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

static void game_handle_removals(game_s *game)
{
    int i;

    while(array_length(game->components_to_remove) > 0)
    {
        // remove
        for(i = 0; i < array_length(game->components_to_remove); i++)
        {
            component_h handle = array_get(game->components_to_remove, i);
            component_s *component = handle_get(handle);
            if(component != NULL)
            {
                component_release(component);
                handle_release(handle);
            }
        }
        array_set_length(game->components_to_remove, 0);
        // remove children
        //
        // This code relies on the fact that children always come after their
        // parents in the components list. TODO: assert this
        //
        // Skip the first element, since it is the root.
        for(i = 1; i < array_length(game->components); i++)
        {
            component_h handle = array_get(game->components, i);
            component_s *component = handle_get(handle);
            if(component && handle_get(component->parent) == NULL)
            {
                if(component->release_func != NULL)
                {
                    component->release_func(component->data);
                }
                component_release(component);
                handle_release(handle);
            }
        }
        array_filter(game->components, is_not_null_handle);

        for(i = 0; i < array_length(game->buffer_records); i++)
        {
            buffer_record_s *record = array_get(game->buffer_records, i);
            if(handle_get(record->owner) == NULL)
            {
                buffer_record_release(record);
                array_set(game->buffer_records, i, NULL);
            }
        }
        array_filter(game->buffer_records, is_not_null_pointer);

        for(i = 0; i < array_length(game->subscriptions); i++)
        {
            subscription_s *subscription = array_get(game->subscriptions, i);
            if(handle_get(subscription->subscriber) == NULL)
            {
                subscription_release(subscription);
                array_set(game->subscriptions, i, NULL);
            }
        }
        array_filter(game->subscriptions, is_not_null_pointer);
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

//// Tick ////

void game_tick(game_s *game)
{
    assert(game);

    game_context_s generic_context;
    generic_context.game = game;
    generic_context.component = null_handle(component_h);

    broadcast_tick(&generic_context, nothing);

    while(array_length(game->messages) != 0)
    {
        game_dispatch_messages(game);
        game_handle_removals(game);
        game_update_buffers(game);
    }
}
