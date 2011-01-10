#include "renderer.h"
#include "GL/glfw.h"

#include "array.h"
#include "transform.h"

typedef struct
{
    component_h component;
    array_of(transform_h) sprites;
} renderer_s;

static void handle_tick(void *data, const char *name, const void *content)
{
    renderer_s *renderer = data;

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
        // TODO: actually remove dead transforms from the list
        if(transform)
        {
            //printf("%f, %f\n", transform->pos.x, transform->pos.y);
            glVertex3d(transform->pos.x, transform->pos.y, 0);
        }
    }
    glEnd();

    glfwSwapBuffers();
}

static void handle_add_sprite(void *data, const char *name, const void *content)
{
    renderer_s *renderer = data;
    array_add(renderer->sprites, *(const transform_h *)content);
}

const component_h add_renderer_component(game_s *game)
{
    renderer_s *renderer = malloc(sizeof(renderer_s));
    renderer->component =
        game_add_component(game, game_add_entity(game), renderer);
    renderer->sprites = array_new();

    game_subscribe(game, renderer->component, "tick", handle_tick);
    game_subscribe(game, renderer->component, "add_sprite", handle_add_sprite);

    return renderer->component;
}
