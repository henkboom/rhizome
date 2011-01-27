#ifndef RHIZOME__PLAYER_INPUT_H
#define RHIZOME__PLAYER_INPUT_H

#include "game.h"
#include "vect.h"

typedef struct {
    vect_s direction;
} player_input_s;
define_handle_type(player_input_h, const player_input_s);


declare_component(player_input, player_input_h);

#endif
