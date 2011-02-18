#include "dummy_scene.h"

#include "camera.h"
#include "group.h"
#include "input_handler.h"
#include "player_input.h"
#include "renderer.h"
#include "sprite.h"
#include "transform.h"
#include "vect.h"

typedef struct
{
    component_h group;
    transform_h transform;
    player_input_h player_input;
} dummy_scene_s;

component_h add_dummy_scene_component(
    game_context_s *context,
    component_h parent)
{
    context = game_add_component(context, parent, release_component);

    component_subscribe(context, tick);

    dummy_scene_s *dummy_scene = malloc(sizeof(dummy_scene_s));
    game_set_component_data(context, dummy_scene);

    component_h self = game_get_self(context);

    // general
    add_input_handler_component(context, self);
    add_renderer_component(context, self);

    // camera
    transform_h camera_transform = add_transform_component(context, self,
        vect_zero,
        make_look_quaternion(make_vect(0, 0, -1), make_vect(0, 1, 0)));
    camera_h camera = add_camera_component(context, self, camera_transform);
    component_h camera_component = handle_get(camera)->component;
    send_camera_set_orthographic(context, camera_component, 1);
    send_camera_set_height(context, camera_component, 2);
    send_camera_set_near(context, camera_component, -1);
    send_camera_set_far(context, camera_component, 1);

    // the spinning axes
    dummy_scene->group = add_group_component(context, self);
    dummy_scene->transform =
        add_transform_component(context, dummy_scene->group,
            vect_zero,
            quaternion_identity);
    dummy_scene->player_input =
        add_player_input_component(context, dummy_scene->group);

    add_sprite_component(
        context,
        dummy_scene->group,
        dummy_scene->transform);

    return self;
}

static void release_component(void *data)
{
    free(data);
}

static void handle_tick(
    game_context_s *context,
    void *data,
    const nothing_s *n)
{
    dummy_scene_s *dummy_scene = data;
    const transform_s *transform = handle_get(dummy_scene->transform);
    const player_input_s *player_input = handle_get(dummy_scene->player_input);
    if(transform && player_input)
    {
        send_transform_move(context, transform->component,
            vect_div(player_input->direction, 100));
        send_transform_rotate(context, transform->component,
            make_quaternion_rotation(
                vect_normalize(make_vect(1, 1, 1)),
                0.01));

        if(transform->pos.x > 100)
            game_remove_component(context, dummy_scene->group);
    }
}
