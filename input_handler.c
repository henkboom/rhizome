#include "input_handler.h"

#include <assert.h>
#include <stdio.h>

#include <GL/glfw.h>

begin_component(input_handler);
    component_subscribe(tick);
end_component();

typedef struct {
} input_handler_s;

static game_context_s *target_context = NULL;

static void GLFWCALL key_callback(int key, int action)
{
    assert(target_context != NULL);

    key_event_s event;
    event.key = key;
    event.is_down = action == GLFW_PRESS;
    broadcast_input_handler_key_event(target_context, event);
}

static void GLFWCALL window_size_callback(int width, int height)
{
    assert(target_context != NULL);

    resize_event_s event;
    event.width = (width < 1) ? 1 : width;
    event.height = (height < 1) ? 1 : height;
    broadcast_input_handler_resize_event(target_context, event);
}

static component_h init(game_context_s *context)
{
    input_handler_s *input_handler = malloc(sizeof(input_handler_s));
    game_set_component_data(context, input_handler);

    // only get input events when we explicitly ask with glfwPollEvents()
    glfwDisable(GLFW_AUTO_POLL_EVENTS);

    // set the callbacks. actually the callbacks might be called on setting
    // right now, particularly the window size one.
    target_context = context;
    glfwSetKeyCallback(key_callback);
    glfwSetWindowSizeCallback(window_size_callback);
    target_context = NULL;

    return game_get_self(context);
}

static void release(void *data)
{
    free(data);
}

static void handle_tick(game_context_s *context, void *data, const void **dummy)
{
    target_context = context;
    glfwPollEvents();
    target_context = NULL;
}
