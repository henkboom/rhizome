#ifndef GAME_H
#define GAME_H

#include <stdlib.h>

// internal types
typedef long long unsigned int entity_id_t;
typedef long long unsigned int component_id_t;

// basic types
typedef struct _entity_s entity_s;
typedef struct _game_s game_s;

typedef struct 
{
    component_id_t id;
    size_t index;
} component_s;

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
entity_s * game_add_entity(game_s *game);

// game component
component_s game_add_component(game_s *game, entity_s *entity, void *data);

// game buffer
const void * game_add_buffer(
    game_s *game,
    component_s owner,
    void *source,
    size_t size);

const void * game_add_buffer_with_updater(
    game_s *game,
    component_s owner,
    void *source,
    size_t size,
    buffer_updater_f update_function);

// game messages
void game_subscribe(
    game_s *game,
    component_s subscriber,
    const char *name,
    void (*handler)());

void *game_broadcast_message(
    game_s *game,
    const char *name,
    size_t len);

void *game_send_message(
    game_s *game,
    component_s to,
    const char *name,
    size_t len);

void game_tick(game_s *game);

#endif
