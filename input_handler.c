#include "input_handler.h"

#include <assert.h>
#include <stdio.h>

#include <GL/glfw.h>

begin_component(input_handler);
    component_subscribe(tick);
end_component();

typedef struct {
    game_s *game;
} input_handler_s;

static game_s *target_game = NULL;

static void GLFWCALL key_callback(int key, int action)
{
    assert(target_game != NULL);

    key_event_s e;
    e.key = key;
    e.is_down = action == GLFW_PRESS;
    broadcast_input_handler_key_event(target_game, e);
    printf("sent key callback %d %d\n", e.key, e.is_down);
}

static component_h init(game_s *game, component_h component)
{
    input_handler_s *input_handler = malloc(sizeof(input_handler_s));
    component_set_data(component, input_handler);
    input_handler->game = game;

    glfwDisable(GLFW_AUTO_POLL_EVENTS);
    glfwSetKeyCallback(key_callback);

    return component;
}

static void release(void *data)
{
    free(data);
}

static void handle_tick(void *data, const void **dummy)
{
    input_handler_s *input_handler = data;
    target_game = input_handler->game;
    glfwPollEvents();
    target_game = NULL;
}
