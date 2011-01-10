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
typedef void (*buffer_updater_f)(
    void *component_data,
    void *buffer,
    void *source,
    size_t buffer_size);

typedef void (*message_handler_f)(
    void *component_data,
    const char *name,
    const void *content);

// game
game_s * game_new();
void game_release(game_s *game);

// game entity
entity_h game_add_entity(game_s *game);
void game_remove_entity(game_s *game, entity_h entity);

// game component
component_h game_add_component(game_s *game, entity_h entity, void *data);

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
    void (*handler)());

// TODO: messages sent to dead components should not become broadcasts!
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

#endif
