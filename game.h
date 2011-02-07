#ifndef RHIZOME__GAME_H
#define RHIZOME__GAME_H

#include <stdlib.h>

#include "handle.h"

//// Types ////////////////////////////////////////////////////////////////////

// basic types
typedef struct _game_s game_s;
typedef struct _game_context_s game_context_s;
typedef struct _component_s component_s;

define_handle_type(component_h, component_s);

// TODO this shouldn't be here
struct _game_context_s
{
    game_s *game;
    component_h component;
};

// callback types
typedef void (*component_release_f)(
    void *component_data);

typedef void (*buffer_updater_f)(
    void *component_data,
    void *buffer,
    void *source,
    size_t buffer_size);

typedef void (*message_handler_f)(
    game_context_s *context,
    void *component_data,
    const void *content);

typedef component_h (*initial_component_f)(
    game_context_s *context,
    component_h parent);

typedef struct {} nothing_s;
nothing_s nothing;

//// External Interface ///////////////////////////////////////////////////////

game_s * game_new(initial_component_f init);
void game_release(game_s *game);
int game_done(game_s *game);
void game_tick(game_s *game);

//// Internal (Component) Interface ///////////////////////////////////////////

// game component
game_context_s game_add_component(
    game_context_s *context,
    component_h parent,
    component_release_f release_func);

void game_remove_component(game_context_s *context, component_h component);

component_h game_get_self(game_context_s *context);

void game_set_component_data(game_context_s *context, void *data);

// game buffer
void game_add_buffer(
    game_context_s *context,
    void *source,
    size_t size,
    void_h *out);

void game_add_buffer_with_updater(
    game_context_s *context,
    void *source,
    size_t size,
    buffer_updater_f update_function,
    void_h *out);

// game messages
void game_subscribe(
    game_context_s *context,
    const char *name,
    message_handler_f handler);

void *game_broadcast_message(
    game_context_s *context,
    const char *name,
    size_t len);

void *game_send_message(
    game_context_s *context,
    component_h to,
    const char *name,
    size_t len);

#define declare_component(name, return_type) \
    return_type (add_##name##_component)( \
        game_context_s *context, component_h parent); \
    typedef return_type _component_return_type_##name;

//TODO: figure out how to remove the last line of these next two define* macros
#define define_message(message_name, content_type) \
    typedef content_type _message_content_type_##message_name; \
    static inline void send_##message_name( \
        game_context_s *context, \
        component_h to, \
        content_type content) \
    { \
        content_type *data = game_send_message( \
            context, to, #message_name, sizeof(content_type)); \
        *data = content; \
    } \
    typedef void (_message_handler_type_##message_name) \
        (game_context_s *context, void *data, const content_type *); \
    static _message_handler_type_##message_name handle_##message_name;

#define define_broadcast(message_name, content_type) \
    typedef content_type _message_content_type_##message_name; \
    static inline void broadcast_##message_name( \
        game_context_s *context, \
        content_type content) \
    { \
        content_type *data = game_broadcast_message( \
            context, #message_name, sizeof(content_type)); \
        *data = content; \
    } \
    typedef void (_message_handler_type_##message_name) \
        (game_context_s *context, void *data, const content_type *); \
    static _message_handler_type_##message_name handle_##message_name;

#define begin_component(name) \
    static _component_return_type_##name init( \
        game_context_s *); \
    static void release(void *); \
    _component_return_type_##name add_##name##_component( \
        game_context_s *context, component_h parent) \
    { \
        game_context_s _new_context = \
            game_add_component(context, parent, release); \
        _component_return_type_##name _ret = init(&_new_context); \

#define component_subscribe(message_name) \
        _message_handler_type_##message_name *_handler_##message_name = \
            handle_##message_name; \
        game_subscribe(&_new_context, #message_name, \
            (message_handler_f)_handler_##message_name);

#define end_component() \
        return _ret; \
    }

define_broadcast(tick, nothing_s);

#endif
