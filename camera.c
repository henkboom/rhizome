#include "camera.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include "renderer.h"

typedef struct _camera_internal_s camera_internal_s;

struct _camera_internal_s {
    camera_s public;
    render_job_s render_job;
};

static void render(const render_context_s *context, void *data);

camera_h add_camera_component(
    game_context_s *context,
    component_h parent,
    transform_h transform)
{
    context = game_add_component(context, parent, release_component);

    component_subscribe(context, camera_set_orthographic);
    component_subscribe(context, camera_set_height);
    component_subscribe(context, camera_set_fov_y);
    component_subscribe(context, camera_set_near);
    component_subscribe(context, camera_set_far);

    camera_internal_s *camera = malloc(sizeof(camera_internal_s));
    game_set_component_data(context, camera);
    camera->public.component = game_get_self(context);
    camera->public.transform = transform;
    camera->public.is_orthographic = 0;
    camera->public.height = 2;
    camera->public.fov_y = 65;
    camera->public.near = 0.1;
    camera->public.far = 1000;

    camera_h camera_handle;
    game_add_buffer(context, &camera->public, sizeof(camera_s),
            (void_h *)&camera_handle);

    // render job
    camera->render_job.priority = 0;
    camera->render_job.render = render;
    // the renderer is never called after this buffer is gone
    camera->render_job.data = (void *)handle_get(camera_handle);

    render_job_h render_job_handle;
    handle_new(&render_job_handle, &camera->render_job);
    broadcast_renderer_add_job(context, render_job_handle);

    return camera_handle;
}

static void release_component(void *data)
{
    free(data);
}

static void handle_camera_set_orthographic(game_context_s *context, void *data,
    const int *is_orthographic)
{
    camera_internal_s *camera = data;
    camera->public.is_orthographic = *is_orthographic;
}

static void handle_camera_set_height(game_context_s *context, void *data,
    const double *height)
{
    camera_internal_s *camera = data;
    camera->public.height = *height;
}

static void handle_camera_set_fov_y(game_context_s *context, void *data,
    const double *fov_y)
{
    camera_internal_s *camera = data;
    camera->public.fov_y = *fov_y;
}

static void handle_camera_set_near(game_context_s *context, void *data,
    const double *near)
{
    camera_internal_s *camera = data;
    camera->public.near = *near;
}

static void handle_camera_set_far(game_context_s *context, void *data,
    const double *far)
{
    camera_internal_s *camera = data;
    camera->public.far = *far;
}

#define SQRT2 1.41421356237309504880

static void render(const render_context_s *context, void *data)
{
    const camera_s *camera = data;
    const transform_s *transform = handle_get(camera->transform);

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, context->width, context->height);
    glEnable(GL_DEPTH_TEST);
    glColor3d(1, 1, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double ratio = (double)context->width/context->height;
    if(camera->is_orthographic)
    {
        glOrtho(
            -ratio*camera->height/2, ratio*camera->height/2,
            -camera->height/2, camera->height/2,
            -camera->near, -camera->far);
    }
    else
    {
        gluPerspective(camera->fov_y, ratio, camera->near, camera->far);
    }
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // all this is inverted, since a camera transform is an inverse object
    // transform

    quaternion_s rot_quaternion = quaternion_conjugate(transform->orientation);

    // I need to rotate the quaternion first because I want the camera to point
    // in the direction of its local x axis, whereas the opengl coordinate
    // space points towards the negative z axis.

    quaternion_s screen_rotation = make_quaternion(SQRT2/2, 0, SQRT2/2, 0);
                              // = make_quaternion_rotation(vect_j, 3.14159/2);
    rot_quaternion = quaternion_mul(screen_rotation, rot_quaternion);

    // put it all into a matrix
    float rot[16];
    quaternion_to_mat4(rot_quaternion, rot);
    glMultMatrixf(rot);

    glTranslated(-transform->pos.x, -transform->pos.y, -transform->pos.z);
}
