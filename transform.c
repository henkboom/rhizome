#include "transform.h"

static void handle_move(void *data, const char *name, const void *content)
{
    transform_s *transform = data;
    const vect_s *delta = content;

    transform->pos = vect_add(transform->pos, *delta);
}

static void handle_set_pos(void *data, const char *name, const void *content)
{
    transform_s *transform = data;
    const vect_s *new_pos = content;

    transform->pos = *new_pos;
}

const transform_s * add_transform_component(game_s *game, entity_s *entity)
{
    transform_s *transform = malloc(sizeof(transform_s));

    transform->component = game_add_component(game, entity, transform);
    transform->pos = make_vect(0, 0, 0);

    game_subscribe(game, transform->component, "move", handle_move);
    game_subscribe(game, transform->component, "set_pos", handle_set_pos);

    return game_add_buffer(
        game, transform->component, transform, sizeof(transform_s));
}
