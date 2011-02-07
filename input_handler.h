#ifndef RHIZOME__INPUT_HANDLER_H
#define RHIZOME__INPUT_HANDLER_H

#include "game.h"

declare_component(input_handler, component_h);

// key event
typedef struct {
    int key;
    int is_down;
} key_event_s;

define_broadcast(input_handler_key_event, key_event_s);

// window resize event
typedef struct {
    int width;
    int height;
} resize_event_s;

define_broadcast(input_handler_resize_event, resize_event_s);

// window close event
define_broadcast(input_handler_close_event, nothing_s);

#endif
