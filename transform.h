#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "game.h"
#include "handle.h"
#include "vect.h"

typedef struct {
    component_h component;
    vect_s pos;
} transform_s;

define_handle_type(transform_h, const transform_s);

declare_component(transform, transform_h);

define_message(transform_move, vect_s);
define_message(transform_set_pos, vect_s);

#endif
