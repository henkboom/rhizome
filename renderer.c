#include "renderer.h"

#include <GL/glfw.h>

#include "array.h"
#include "transform.h"

begin_component(renderer);
    component_subscribe(tick);
    component_subscribe(renderer_add_job);
end_component();

typedef struct
{
    array_of(render_job_h) render_jobs;
} renderer_s;

static component_h init(game_context_s *context)
{
    renderer_s *renderer = malloc(sizeof(renderer_s));

    renderer->render_jobs = array_new();

    game_set_component_data(context, renderer);
    return game_get_self(context);
}

static void release(void *data)
{
    renderer_s *renderer = data;

    array_release(renderer->render_jobs);
    free(renderer);
}

static int is_not_null_handle(void *value)
{
    return handle_get(*(void_h*)value) != NULL;
}


static void handle_tick(game_context_s *context, void *data, const void** dummy)
{
    renderer_s *renderer = data;

    array_filter(renderer->render_jobs, is_not_null_handle);

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 640, 0, 480, 1, -1);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3d(1, 1, 1);

    for(int i = 0; i < array_length(renderer->render_jobs); i++)
    {
        const render_job_s *render_job =
            handle_get(array_get(renderer->render_jobs, i));
        render_job->render(render_job);
    }

    glfwSwapBuffers();
}

static void handle_renderer_add_job(
    game_context_s *context,
    void *data,
    const render_job_h *render_job)
{
    renderer_s *renderer = data;
    array_add(renderer->render_jobs, *render_job);
}

