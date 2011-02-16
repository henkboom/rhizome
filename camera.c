#include "camera.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include "renderer.h"

// internal render job state, may be modified by the render itself
typedef struct {
    render_job_s render_job;
    camera_h camera;
} camera_render_job_s;

static void render(const render_context_s *context, const render_job_s *data);

camera_h add_camera_component(
    game_context_s *context,
    component_h parent,
    transform_h transform)
{
    context = game_add_component(context, parent, release_component);

    camera_s *camera = malloc(sizeof(camera_s));
    game_set_component_data(context, camera);
    camera->transform = transform;

    camera_h camera_handle;
    game_add_buffer(context, camera, sizeof(camera_s),
            (void_h *)&camera_handle);

    // render job
    camera_render_job_s *camera_render_job =
        malloc(sizeof(camera_render_job_s));
    camera_render_job->render_job.render = render;
    camera_render_job->camera = camera_handle;

    render_job_h render_job_handle;
    game_add_buffer(context, camera_render_job, sizeof(camera_render_job_s),
        (void_h *)&render_job_handle);
    broadcast_renderer_add_job(context, render_job_handle);

    return camera_handle;
}

static void release_component(void *data)
{
    free(data);
}

static void render(const render_context_s *context, const render_job_s *data)
{
    const camera_render_job_s *render_job = (const camera_render_job_s *)data;
    const camera_s *camera = handle_get(render_job->camera);
    const transform_s *transform = handle_get(camera->transform);

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, context->width, context->height);
    glEnable(GL_DEPTH_TEST);
    glColor3d(1, 1, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65, (double)context->width/context->height, 0.1, 100);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    quaternion_s rot_quaternion = quaternion_inverse(transform->orientation);
    double rot[16];
    vect_s rot_i = quaternion_rotate_i(rot_quaternion);
    vect_s rot_j = quaternion_rotate_j(rot_quaternion);
    vect_s rot_k = quaternion_rotate_k(rot_quaternion);
    rot[ 0] = rot_i.x; rot[ 1] = rot_i.y; rot[ 2] = rot_i.z; rot[ 3] = 0;
    rot[ 4] = rot_j.x; rot[ 5] = rot_j.y; rot[ 6] = rot_j.z; rot[ 7] = 0;
    rot[ 8] = rot_k.x; rot[ 9] = rot_k.y; rot[10] = rot_k.z; rot[11] = 0;
    rot[12] = 0;       rot[13] = 0;       rot[14] = 0;       rot[15] = 1;
    glMultMatrixd(rot);

    glTranslated(-transform->pos.x, -transform->pos.y, -transform->pos.z);

}
