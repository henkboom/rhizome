#include "input_handler.h"

#include <assert.h>
#include <stdio.h>

#include <GL/glfw.h>

#define MAX_JOYSTICKS (GLFW_JOYSTICK_LAST+1)

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

typedef struct {
    joystick_event_s joysticks[MAX_JOYSTICKS];
} input_handler_s;

component_h add_input_handler_component(
    game_context_s *context,
    component_h parent)
{
    context = game_add_component(context, parent, release_component);

    component_subscribe(context, tick);

    input_handler_s *input_handler = malloc(sizeof(input_handler_s));
    game_set_component_data(context, input_handler);

    for(int i = 0; i < MAX_JOYSTICKS; i++)
    {
        input_handler->joysticks[i].joystick = i + 1;
        input_handler->joysticks[i].axes = array_new();
        input_handler->joysticks[i].buttons = array_new();
    }

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
    input_handler_s *input_handler = data;
    for(int i = 0; i < MAX_JOYSTICKS; i++)
    {
        array_release(input_handler->joysticks[i].axes);
        array_release(input_handler->joysticks[i].buttons);
    }
    free(input_handler);
}

static void handle_tick(game_context_s *context, void *data, const nothing_s *n)
{
    input_handler_s *input_handler = data;

    // process the callback-based events
    target_context = context;
    glfwPollEvents();
    target_context = NULL;

    // process joysticks
    for(int i = 0; i < MAX_JOYSTICKS; i++)
    {
        if(glfwGetJoystickParam(i, GLFW_PRESENT) == GL_TRUE)
        {
            joystick_event_s *joystick = &input_handler->joysticks[i];

            int axis_count = glfwGetJoystickParam(i, GLFW_AXES);
            array_set_length(joystick->axes, axis_count);
            glfwGetJoystickPos(i, array_get_ptr(joystick->axes), axis_count);

            int button_count = glfwGetJoystickParam(i, GLFW_BUTTONS);
            array_set_length(joystick->buttons, button_count);
            glfwGetJoystickButtons(i, array_get_ptr(joystick->buttons),
                button_count);

            broadcast_input_handler_joystick_event(context, *joystick);
        }
    }
}
