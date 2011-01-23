#ifndef RENDERER_H
#define RENDERER_H

#include "game.h"
#include "transform.h"
#include "vect.h"

declare_component(renderer, component_h);

typedef struct {
    int priority;
    void (*render)(void *data);
    void *data;
} render_job_s;
define_handle_type(render_job_h, render_job_s);

define_broadcast(renderer_add_job, render_job_h);

#endif
