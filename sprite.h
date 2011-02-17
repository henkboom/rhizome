#ifndef RHIZOME__SPRITE_H
#define RHIZOME__SPRITE_H

#include "game.h"
#include "transform.h"

component_h add_sprite_component(
    game_context_s *context,
    component_h parent,
    transform_h transform);

#endif
