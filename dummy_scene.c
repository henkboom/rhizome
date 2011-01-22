#include "dummy_scene.h"

#include "input_handler.h"
#include "renderer.h"
#include "transform.h"

begin_component(dummy_scene);
    component_subscribe(tick);
end_component();

typedef struct
{
    entity_h entity;
    transform_h transform;
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

    broadcast_renderer_add_sprite(context, dummy_scene->transform);

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
    if(transform)
    {
        send_transform_move(context, transform->component, make_vect(1, 1, 0));
        send_transform_move(context, transform->component, make_vect(0, 1, 0));

        if(transform->pos.x > 100)
            game_remove_entity(context, dummy_scene->entity);
    }
}
