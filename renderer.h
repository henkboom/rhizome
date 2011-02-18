#ifndef RHIZOME__RENDERER_H
#define RHIZOME__RENDERER_H

#include "game.h"
#include "transform.h"
#include "vect.h"

component_h add_renderer_component(game_context_s *context, component_h parent);

typedef struct {
    int width;
    int height;
} render_context_s;

struct _render_job_s {
    int priority;
    void (*render)(
        const render_context_s *context,
        void *data);
    void *data;
};
typedef struct _render_job_s render_job_s;
define_handle_type(render_job_h, const render_job_s);

define_broadcast(renderer_add_job, render_job_h);

#endif
