#ifndef RHIZOME__PLAYER_INPUT_H
#define RHIZOME__PLAYER_INPUT_H

#include "game.h"
#include "vect.h"

typedef struct {
    vect_s direction;
} player_input_s;
define_handle_type(player_input_h, const player_input_s);

player_input_h add_player_input_component(
    game_context_s *context,
    component_h parent);

#endif
