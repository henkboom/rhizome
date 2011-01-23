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

static void render(const render_job_s *data)
{
    sprite_s *sprite = (sprite_s *)data;
    const transform_s *transform = handle_get(sprite->transform);

    if(transform)
    {
        glBegin(GL_POINTS);
        glVertex3d(transform->pos.x, transform->pos.y, 0);
        glEnd();
    }
}
