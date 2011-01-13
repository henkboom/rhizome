#ifndef RENDERER_H
#define RENDERER_H

#include "game.h"
#include "transform.h"
#include "vect.h"

declare_component(renderer, component_h);

define_message(renderer_add_sprite, transform_h);

#endif
