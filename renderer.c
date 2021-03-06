#include "renderer.h"

#include <GL/glfw.h>

#include "array.h"
#include "input_handler.h"
#include "transform.h"

typedef struct
{
    array_of(render_job_h) render_jobs;
    render_context_s render_context;
} renderer_s;

component_h add_renderer_component(game_context_s *context, component_h parent)
{
    context = game_add_component(context, parent, release_component);

    component_subscribe(context, tick);
    component_subscribe(context, renderer_add_job);
    component_subscribe(context, input_handler_resize_event);

    renderer_s *renderer = malloc(sizeof(renderer_s));

    renderer->render_jobs = array_new();
    // these will be overridden soon, but give them sensible initial values
    renderer->render_context.width = 640;
    renderer->render_context.height = 480;

    game_set_component_data(context, renderer);
    return game_get_self(context);
}

static void release_component(void *data)
{
    renderer_s *renderer = data;

    array_release(renderer->render_jobs);
    free(renderer);
}

static int is_not_null_handle(void *value)
{
    return handle_get(*(void_h*)value) != NULL;
}

static int render_job_cmp(const void *a, const void *b)
{
    return handle_get(*(render_job_h *)a)->priority -
           handle_get(*(render_job_h *)b)->priority;
}

static void handle_tick(game_context_s *context, void *data, const nothing_s *n)
{
    renderer_s *renderer = data;

    array_filter(renderer->render_jobs, is_not_null_handle);
    array_qsort(renderer->render_jobs, render_job_cmp);

    for(int i = 0; i < array_length(renderer->render_jobs); i++)
    {
        const render_job_s *render_job =
            handle_get(array_get(renderer->render_jobs, i));
        render_job->render(&renderer->render_context, render_job->data);
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

static void handle_input_handler_resize_event(
    game_context_s *context,
    void *data,
    const resize_event_s *event)
{
    renderer_s *renderer = data;
    renderer->render_context.width = event->width;
    renderer->render_context.height = event->height;
}
