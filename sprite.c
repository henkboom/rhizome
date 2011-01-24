#include "sprite.h"

#include <GL/gl.h>
#include "renderer.h"

begin_component(sprite);
    component_subscribe(sprite_track_transform);
end_component();

typedef struct {
    render_job_s render_job;
    transform_h transform;
} sprite_s;

static void render(const render_job_s *data);

static component_h init(game_context_s *context)
{
    sprite_s *sprite = malloc(sizeof(sprite_s));
    game_set_component_data(context, sprite);

    sprite->render_job.render = render;
    sprite->transform = null_handle(transform_h);

    render_job_h handle;
    game_add_buffer(context, sprite, sizeof(sprite_s), (void_h*)&handle);

    broadcast_renderer_add_job(context, handle);

    return game_get_self(context);
}

static void release(void *data)
{
    free(data);
}

static void handle_sprite_track_transform(
    game_context_s *context,
    void *data,
    const transform_h *transform)
{
    sprite_s *sprite = data;
    sprite->transform = *transform;
}

static void draw_vect(vect_s v)
{
    glVertex3d(v.x, v.y, v.z);
}

static void render(const render_job_s *data)
{
    sprite_s *sprite = (sprite_s *)data;
    const transform_s *transform = handle_get(sprite->transform);

    if(transform)
    {
        vect_s i = quaternion_rotate_i(transform->orientation);
        vect_s j = quaternion_rotate_j(transform->orientation);
        vect_s k = quaternion_rotate_k(transform->orientation);

        glBegin(GL_LINES);
            glColor3d(1, 0, 0);
            draw_vect(transform->pos);
            draw_vect(vect_add(transform->pos, vect_mul(i, 40)));
            glColor3d(0, 1, 0);
            draw_vect(transform->pos);
            draw_vect(vect_add(transform->pos, vect_mul(j, 40)));
            glColor3d(0, 0, 1);
            draw_vect(transform->pos);
            draw_vect(vect_add(transform->pos, vect_mul(k, 40)));
        glEnd();

        glColor3d(1, 1, 1);
    }
}
