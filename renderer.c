#include "renderer.h"
#include "GL/glfw.h"

#include "array.h"
#include "transform.h"

begin_component(renderer);
    component_subscribe(tick);
    component_subscribe(renderer_add_sprite);
end_component();

typedef struct
{
    component_h component;
    array_of(transform_h) sprites;
} renderer_s;

static component_h init(game_s *game, component_h component)
{
    renderer_s *renderer = malloc(sizeof(renderer_s));

    renderer->component = component;
    renderer->sprites = array_new();

    component_set_data(component, renderer);
    return component;
}

static void release(void *data)
{
    renderer_s *renderer = data;

    array_release(renderer->sprites);
    free(renderer);
}

static int is_not_null_handle(void *value)
{
    return handle_get(*(void_h*)value) != NULL;
}


static void handle_tick(void *data, const void** dummy)
{
    renderer_s *renderer = data;

    array_filter(renderer->sprites, is_not_null_handle);

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
    glBegin(GL_POINTS);
    int i;
    for(i = 0; i < array_length(renderer->sprites); i++)
    {
        const transform_s *transform =
            handle_get(array_get(renderer->sprites, i));
        if(transform)
        {
            //printf("%f, %f\n", transform->pos.x, transform->pos.y);
            glVertex3d(transform->pos.x, transform->pos.y, 0);
        }
    }
    glEnd();

    glfwSwapBuffers();
}

static void handle_renderer_add_sprite(void *data, const transform_h *sprite)
{
    renderer_s *renderer = data;
    array_add(renderer->sprites, *sprite);
}

