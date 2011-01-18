#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "game.h"

declare_component(input_handler, component_h);

typedef struct {
    int key;
    int is_down;
} key_event_s;

define_broadcast(input_handler_key_event, key_event_s);

#endif
