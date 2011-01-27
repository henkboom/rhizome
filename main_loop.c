#include "main_loop.h"

#include <stdio.h>
#include <GL/glfw.h>

#define WIDTH 640
#define HEIGHT 480

static void wait_until(double time)
{
    // leave this much left after the OS sleep, in case it's innacurate
    const double sleep_allowance = 0.002;
    const double min_sleep_time = 0.002;
    double time_to_sleep = time - glfwGetTime() - sleep_allowance;
    if(time_to_sleep >= min_sleep_time)
    {
        glfwSleep(time_to_sleep);
    }

    // busy wait the rest
    while(glfwGetTime() < time)
    {
        // do nothing
    }
}

static void main_loop(game_s *game)
{
    const double update_interval = 1.0/60;
    double next_update_time = glfwGetTime();

    while(1)
    {
        next_update_time += update_interval;
        wait_until(next_update_time);
        game_tick(game);
    }
}

void enter_main_loop(initial_component_f initial_component)
{
    if(!glfwInit())
        return;
    glfwOpenWindow(WIDTH, HEIGHT, 8, 8, 8, 8, 24, 0, GLFW_WINDOW);
    glViewport(0, 0, WIDTH, HEIGHT);

    game_s *game = game_new(initial_component);

    main_loop(game);

    game_release(game);
}

