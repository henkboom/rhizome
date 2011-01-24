#include "group.h"

begin_component(group);
end_component();

component_h init(game_context_s *context)
{
    return game_get_self(context);
}

void release(void *data)
{
    // do nothing
}
