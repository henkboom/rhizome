#include "group.h"


component_h add_group_component(game_context_s *context, component_h parent)
{
    context = game_add_component(context, parent, release_component);
    return game_get_self(context);
}

void release_component(void *data)
{
    // do nothing
}
