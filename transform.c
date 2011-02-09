#include "transform.h"

transform_h add_transform_component(
    game_context_s *context,
    component_h parent,
    vect_s pos,
    quaternion_s orientation)
{
    context = game_add_component(context, parent, release_component);

    component_subscribe(context, transform_move);
    component_subscribe(context, transform_set_pos);
    component_subscribe(context, transform_rotate);
    component_subscribe(context, transform_set_orientation);

    transform_s *transform = malloc(sizeof(transform_s));
    transform->component = game_get_self(context);
    transform->pos = make_vect(0, 0, 0);
    transform->orientation = make_quaternion(1, 0, 0, 0);

    transform_h handle;
    game_add_buffer(
        context, transform, sizeof(transform_s), (void_h *)&handle);

    game_set_component_data(context, transform);
    return handle;
}

static void release_component(void *data)
{
    free(data);
}

static void handle_transform_move(
    game_context_s *context,
    void *data,
    const vect_s *delta)
{
    transform_s *transform = data;

    transform->pos = vect_add(transform->pos, *delta);
}

static void handle_transform_set_pos(
    game_context_s *context,
    void *data,
    const vect_s *new_pos)
{
    transform_s *transform = data;

    transform->pos = *new_pos;
}

static void handle_transform_rotate(
    game_context_s *context,
    void *data,
    const quaternion_s *rotation)
{
    transform_s *transform = data;

    transform->orientation = quaternion_normalize(
        quaternion_mul(transform->orientation, *rotation));
}

static void handle_transform_set_orientation(
    game_context_s *context,
    void *data,
    const quaternion_s *orientation)
{
    transform_s *transform = data;

    transform->orientation = *orientation;
}
