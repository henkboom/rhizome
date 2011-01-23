#include "dummy_scene.h"

#include "input_handler.h"
#include "player_input.h"
#include "renderer.h"
#include "sprite.h"
#include "transform.h"
#include "vect.h"

begin_component(dummy_scene);
    component_subscribe(tick);
end_component();

typedef struct
{
    entity_h entity;
    transform_h transform;
    player_input_h player_input;
} dummy_scene_s;

static component_h init(game_context_s *context)
{
    dummy_scene_s *dummy_scene = malloc(sizeof(dummy_scene_s));
    game_set_component_data(context, dummy_scene);

    add_input_handler_component(context, game_add_entity(context));
    add_renderer_component(context, game_add_entity(context));

    entity_h entity = game_add_entity(context);
    dummy_scene->entity = entity;
    dummy_scene->transform = add_transform_component(context, entity);
    send_transform_set_pos(
        context,
        handle_get(dummy_scene->transform)->component,
        make_vect(50, 50, 0));
    dummy_scene->player_input = add_player_input_component(context, entity);

    component_h sprite = add_sprite_component(context, entity);
    send_sprite_track_transform(context, sprite, dummy_scene->transform);

    return game_get_self(context);
}

static void release(void *data)
{
    free(data);
}

static void handle_tick(game_context_s *context, void *data, const void **dummy)
{
    dummy_scene_s *dummy_scene = data;
    const transform_s *transform = handle_get(dummy_scene->transform);
    const player_input_s *player_input = handle_get(dummy_scene->player_input);
    if(transform && player_input)
    {
        send_transform_move(context, transform->component, player_input->direction);

        if(transform->pos.x > 100)
            game_remove_entity(context, dummy_scene->entity);
    }
}
