#ifndef GAME_H
#define GAME_H

#include <stdlib.h>

#include "handle.h"

// internal types
typedef long long unsigned int entity_id_t;
typedef long long unsigned int component_id_t;

// basic types
typedef struct _entity_s entity_s;
typedef struct _game_s game_s;

typedef struct _component_s component_s;

define_handle_type(component_h, component_s);
define_handle_type(entity_h, entity_s);

// callback types
typedef void (*component_release_f)(
    void *component_data);

typedef void (*buffer_updater_f)(
    void *component_data,
    void *buffer,
    void *source,
    size_t buffer_size);

typedef void (*message_handler_f)(
    void *component_data,
    const void *content);

// game
game_s * game_new();
void game_release(game_s *game);

// game entity
entity_h game_add_entity(game_s *game);
void game_remove_entity(game_s *game, entity_h entity);

// game component
component_h game_add_component(game_s *game, entity_h entity, void *data);

void component_set_data(component_h component, void *data);

// game buffer
void game_add_buffer(
    game_s *game,
    component_h owner,
    void *source,
    size_t size,
    void_h *out);

void game_add_buffer_with_updater(
    game_s *game,
    component_h owner,
    void *source,
    size_t size,
    buffer_updater_f update_function,
    void_h *out);

// game messages
void game_subscribe(
    game_s *game,
    component_h subscriber,
    const char *name,
    message_handler_f handler);

void *game_broadcast_message(
    game_s *game,
    const char *name,
    size_t len);

void *game_send_message(
    game_s *game,
    component_h to,
    const char *name,
    size_t len);

void game_tick(game_s *game);

#define declare_component(component_name, return_type) \
    return_type (add_##component_name##_component)( \
        game_s *game, entity_h entity); \
    typedef return_type _component_return_type_##component_name;

//TODO: figure out how to remove the last line of these next two macros
#define define_message(message_name, content_type) \
    typedef content_type _message_content_type_##message_name; \
    static inline void send_##message_name( \
        game_s *game, \
        component_h to, \
        content_type content) \
    { \
        content_type *data = game_send_message( \
            game, to, #message_name, sizeof(content_type)); \
        *data = content; \
    } \
    typedef void (_message_handler_type_##message_name) \
        (void *data, const content_type *); \
    static _message_handler_type_##message_name handle_##message_name;

#define define_broadcast(message_name, content_type) \
    typedef content_type _message_content_type_##message_name; \
    static inline void broadcast_##message_name( \
        game_s *game, \
        content_type content) \
    { \
        content_type *data = game_broadcast_message( \
            game, #message_name, sizeof(content_type)); \
        *data = content; \
    } \
    typedef void (_message_handler_type_##message_name) \
        (void *data, const content_type *); \
    static _message_handler_type_##message_name handle_##message_name;

#define begin_component(component_name) \
    static _component_return_type_##component_name init( \
        game_s *, component_h); \
    static void release(void *); \
    _component_return_type_##component_name add_##component_name##_component( \
        game_s *game, entity_h entity) \
    { \
        component_h _component = game_add_component(game, entity, release); \
        _component_return_type_##component_name _ret = init(game, _component); \

#define component_subscribe(message_name) \
        _message_handler_type_##message_name *_handler_##message_name = \
            handle_##message_name; \
        game_subscribe( game, _component, #message_name, \
            (message_handler_f)_handler_##message_name);

#define end_component() \
        return _ret; \
    }

// TODO: void *?
define_broadcast(tick, void *);

#endif
