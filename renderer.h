#ifndef RHIZOME__RENDERER_H
#define RHIZOME__RENDERER_H

#include "game.h"
#include "transform.h"
#include "vect.h"

declare_component(renderer, component_h);

struct _render_job_s {
    void (*render)(const struct _render_job_s *data);
};
typedef struct _render_job_s render_job_s;
define_handle_type(render_job_h, const render_job_s);

define_broadcast(renderer_add_job, render_job_h);

#endif
