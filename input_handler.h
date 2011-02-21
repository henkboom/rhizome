#ifndef RHIZOME__INPUT_HANDLER_H
#define RHIZOME__INPUT_HANDLER_H

#include "array.h"
#include "game.h"

component_h add_input_handler_component(
    game_context_s *context,
    component_h parent);

// key event
typedef struct {
    int key;
    int is_down;
} key_event_s;

define_broadcast(input_handler_key_event, key_event_s);

// joystick event

typedef struct {
    int joystick;
    array_of(float) axes;
    array_of(unsigned char) buttons;
} joystick_event_s;
define_handle_type(joystick_event_h, const joystick_event_s);

define_broadcast(input_handler_joystick_event, joystick_event_s);

// window resize event
typedef struct {
    int width;
    int height;
} resize_event_s;

define_broadcast(input_handler_resize_event, resize_event_s);

// window close event
define_broadcast(input_handler_close_event, nothing_s);

#endif
