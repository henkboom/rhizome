#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "game.h"
#include "handle.h"
#include "vect.h"

typedef struct {
    component_s component;
    vect_s pos;
} transform_s;
define_handle_type(transform_h, const transform_s);

transform_h add_transform_component(game_s *game, entity_s *entity);

#endif
