#include <stdio.h>
#include <GL/glfw.h>

#include "input_handler.h"
#include "game.h"
#include "renderer.h"
#include "transform.h"

typedef struct
{
    game_s *game;
    entity_h entity;
    transform_h transform;
} dummy_data_s;

static void tick_function(
    void *data,
    const void *content)
{
    dummy_data_s *dummy_data = data;
    game_s *game = dummy_data->game;
    const transform_s *transform = handle_get(dummy_data->transform);
    send_transform_move(game, transform->component, make_vect(1, 1, 0));
    send_transform_move(game, transform->component, make_vect(0, 1, 0));

    static int done = 0;
    if(!done)
    {
        done = 1;
        transform_h * content = game_broadcast_message(
            game, "renderer_add_sprite", sizeof(transform_h));
        *content = dummy_data->transform;
    }

    if(transform->pos.x > 100)
        game_remove_entity(game, dummy_data->entity);
}

void init_game(game_s *game)
{
    add_input_handler_component(game, game_add_entity(game));
    add_renderer_component(game, game_add_entity(game));

    entity_h entity = game_add_entity(game);
    dummy_data_s *data = malloc(sizeof(dummy_data_s));
    data->game = game;
    data->entity = entity;
    data->transform = add_transform_component(game, entity);
    component_h c = game_add_component(game, entity, free);
    component_set_data(c, data);
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

    game_s *game = game_new();
    init_game(game);

    main_loop(game);

    game_release(game);
    return 0;
}
