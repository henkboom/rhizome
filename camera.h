#ifndef RHIZOME__CAMERA_H
#define RHIZOME__CAMERA_H

#include "game.h"
#include "transform.h"

typedef struct {
    component_h component;
    transform_h transform;
    int is_orthographic;
    double height; // ortho only
    double fov_y; // perspective only
    double near;
    double far;
} camera_s;
define_handle_type(camera_h, const camera_s);

camera_h add_camera_component(
    game_context_s *context,
    component_h parent,
    transform_h transform);

define_message(camera_set_orthographic, int);
define_message(camera_set_height, double);
define_message(camera_set_fov_y, double);
define_message(camera_set_near, double);
define_message(camera_set_far, double);

#endif
