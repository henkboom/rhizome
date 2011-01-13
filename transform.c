#include "transform.h"

begin_component(transform);
    component_subscribe(transform_move);
    component_subscribe(transform_set_pos);
end_component();

static transform_h init(game_s *game, component_h component)
{
    transform_s *transform = malloc(sizeof(transform_s));
    transform->component = component;
    transform->pos = make_vect(0, 0, 0);

    transform_h handle;
    game_add_buffer(game, transform->component, transform, sizeof(transform_s),
                    (void_h *)&handle);

    component_set_data(component, transform);
    return handle;
}

static void release(void *data)
{
    free(data);
}

static void handle_transform_move(void *data, const vect_s *content)
{
    transform_s *transform = data;
    const vect_s *delta = content;

    transform->pos = vect_add(transform->pos, *delta);
}

static void handle_transform_set_pos(void *data, const vect_s *content)
{
    transform_s *transform = data;
    const vect_s *new_pos = content;

    transform->pos = *new_pos;
}
