#include "input_handler.h"

#include <assert.h>
#include <stdio.h>

#include <GL/glfw.h>

// the glfw callbacks don't support giving them closure data, gotta use a
// global variable instead
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

static int GLFWCALL window_close_callback()
{
    assert(target_context != NULL);

    broadcast_input_handler_close_event(target_context, nothing);

    return GL_FALSE;
}

component_h add_input_handler_component(
    game_context_s *context,
    component_h parent)
{
    context = game_add_component(context, parent, release_component);

    component_subscribe(context, tick);

    // only get input events when we explicitly ask with glfwPollEvents()
    glfwDisable(GLFW_AUTO_POLL_EVENTS);

    // set the callbacks. actually the callbacks might be called on setting
    // right now, particularly the window size one.
    target_context = context;
    glfwSetKeyCallback(key_callback);
    glfwSetWindowSizeCallback(window_size_callback);
    glfwSetWindowCloseCallback(window_close_callback);
    target_context = NULL;

    return game_get_self(context);
}

static void release_component(void *data)
{
    // do nothing
}

static void handle_tick(game_context_s *context, void *data, const nothing_s *n)
{
    target_context = context;
    glfwPollEvents();
    target_context = NULL;
}
