#ifndef RHIZOME__CAMERA_H
#define RHIZOME__CAMERA_H

#include "game.h"
#include "transform.h"

typedef struct {
    transform_h transform;
} camera_s;
define_handle_type(camera_h, const camera_s);

camera_h add_camera_component(
    game_context_s *context,
    component_h parent,
    transform_h transform);

#endif
