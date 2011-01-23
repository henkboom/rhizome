#ifndef SPRITE_H
#define SPRITE_H

#include "game.h"
#include "transform.h"

declare_component(sprite, component_h);

define_message(sprite_track_transform, transform_h);

#endif
