#ifndef RENDERER_H
#define RENDERER_H

#include "game.h"
#include "transform.h"
#include "vect.h"

typedef struct
{
    transform_s transform;
} sprite_s;

const component_h add_renderer_component(game_s *game);

#endif
