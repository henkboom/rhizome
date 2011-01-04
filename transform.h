#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "game.h"
#include "vect.h"

typedef struct {
    component_s component;
    vect_s pos;
} transform_s;

const transform_s * add_transform_component(game_s *game, entity_s *entity);

#endif
