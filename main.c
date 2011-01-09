#include <stdio.h>
#include <GL/glfw.h>

#include "game.h"
#include "renderer.h"
#include "transform.h"

game_s *game;

static void tick_function(
    void *data,
    const char *name,
    const char *content)
{
    const transform_s *transform = handle_get(*(transform_h*)data);
    vect_s *move_by =
        game_send_message(game, transform->component, "move", sizeof(vect_s));
    *move_by = make_vect(1, 1, 0);

    move_by = game_send_message(game, transform->component, "move", sizeof(vect_s));
    *move_by = make_vect(0, 1, 0);

    static int done = 0;
    if(!done)
    {
        done = 1;
        transform_h * content =
            game_broadcast_message(game, "add_sprite", sizeof(transform_h));
        *content = *(transform_h*)data;
    }
}

void init_game(game_s *game)
{
    add_renderer_component(game);

    entity_h entity = game_add_entity(game);
    transform_h *data = malloc(sizeof(transform_h));
    *data = add_transform_component(game, entity);
    component_h c = game_add_component(game, entity, data);
    game_subscribe(game, c, "tick", tick_function);
}

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

#define WIDTH 640
#define HEIGHT 480

int main(void)
{
    if(!glfwInit())
        return -1;
    glfwOpenWindow(WIDTH, HEIGHT, 8, 8, 8, 8, 24, 0, GLFW_WINDOW);
    glViewport(0, 0, WIDTH, HEIGHT);

    game = game_new();
    init_game(game);

    main_loop(game);

    game_release(game);
    return 0;
}
