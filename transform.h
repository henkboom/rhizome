#ifndef RHIZOME__TRANSFORM_H
#define RHIZOME__TRANSFORM_H

#include "game.h"
#include "handle.h"
#include "quaternion.h"
#include "vect.h"

typedef struct {
    component_h component;
    vect_s pos;
    quaternion_s orientation;
} transform_s;
define_handle_type(transform_h, const transform_s);

transform_h add_transform_component(
    game_context_s *context,
    component_h parent,
    vect_s pos,
    quaternion_s orientation);

define_message(transform_move, vect_s);
define_message(transform_set_pos, vect_s);
define_message(transform_rotate, quaternion_s);
define_message(transform_set_orientation, quaternion_s);

#endif
