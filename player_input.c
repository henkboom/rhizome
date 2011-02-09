#include "player_input.h"

#include <GL/glfw.h>

#include "input_handler.h"

typedef struct {
    player_input_s out;
    int left;
    int right;
    int up;
    int down;
} player_input_data_s;

player_input_h add_player_input_component(
    game_context_s *context,
    component_h parent)
{
    context = game_add_component(context, parent, release_component);

    component_subscribe(context, input_handler_key_event);

    player_input_data_s *input = malloc(sizeof(player_input_data_s));
    game_set_component_data(context, input);

    input->out.direction = make_vect(0, 0, 0);
    input->left = 0;
    input->right = 0;
    input->up = 0;
    input->down = 0;

    player_input_h handle;
    game_add_buffer(context, &input->out, sizeof(player_input_s),
                    (void_h*)&handle);
    return handle;
}

static void release_component(void *data)
{
    free(data);
}

static void handle_input_handler_key_event(
    game_context_s *context,
    void *data,
    const key_event_s *event)
{
    player_input_data_s *input = data;

    if(event->key == GLFW_KEY_LEFT)
        input->left = event->is_down;
    else if(event->key == GLFW_KEY_RIGHT)
        input->right = event->is_down;
    else if(event->key == GLFW_KEY_UP)
        input->up = event->is_down;
    else if(event->key == GLFW_KEY_DOWN)
        input->down = event->is_down;
    else
        return;

    input->out.direction = make_vect(
        (input->right ? 1 : 0) - (input->left ? 1 : 0),
        (input->up    ? 1 : 0) - (input->down ? 1 : 0),
        0);

    if(vect_sqrmag(input->out.direction) > 1)
        input->out.direction = vect_normalize(input->out.direction);
}
