#include "sprite.h"

#include <GL/gl.h>
#include "renderer.h"

typedef struct {
    render_job_s render_job;

    // accesed by render job, do not mutate
    transform_h transform;
    mesh_s *mesh;
} sprite_s;

static void render(const render_context_s *context, void *data);
static mesh_s *make_placeholder_mesh();

component_h add_sprite_component(
    game_context_s *context,
    component_h parent,
    transform_h transform,
    mesh_s *mesh)
{
    context = game_add_component(context, parent, release_component);

    sprite_s *sprite = malloc(sizeof(sprite_s));
    game_set_component_data(context, sprite);

    sprite->render_job.priority = 1;
    sprite->render_job.render = render;
    sprite->render_job.data = sprite;
    sprite->transform = transform;
    sprite->mesh = mesh;

    render_job_h handle;
    game_add_buffer(context, sprite, sizeof(sprite_s), (void_h*)&handle);

    broadcast_renderer_add_job(context, handle);

    return game_get_self(context);
}

static void release_component(void *data)
{
    free(data);
}

static void draw_vect(vect_s v)
{
    glVertex3d(v.x, v.y, v.z);
}

static void render(const render_context_s *context, void *data)
{
    sprite_s *sprite = data;

    const transform_s *transform = handle_get(sprite->transform);

    if(sprite->mesh == NULL)
        sprite->mesh = make_placeholder_mesh();

    if(transform)
    {
        glPushMatrix();
        glTranslated(transform->pos.x, transform->pos.y, transform->pos.z);

        float rot[16];
        quaternion_to_mat4(transform->orientation, rot);
        glMultMatrixf(rot);

        mesh_render(sprite->mesh);
        glPopMatrix();
    }
}

// TODO: this stuff should probably be somewhere else once I have some sort of
// resource library
static char *axes_vertex_shader =
"#version 150 compatibility\n"
"in vec3 position;\n"
"in vec3 color;\n"
"out vec3 frag_color;\n"
"void main()\n"
"{\n"
"   gl_Position = gl_ModelViewProjectionMatrix * vec4(position, 1.0);\n"
"   frag_color = color;\n"
"}";

static char *axes_fragment_shader =
"#version 150 compatibility\n"
"in vec3 frag_color;\n"
"void main()\n"
"{\n"
"   gl_FragColor = vec4(frag_color, 1);\n"
"}";

static float axes_position_data[] =
{
    0, 0, 0,
    0.05, 0.05, 0.05,
    1, 0, 0,

    0, 0, 0,
    0.05, 0.05, 0.05,
    0, 1, 0,

    0, 0, 0,
    0.05, 0.05, 0.05,
    0, 0, 1
};

static float axes_color_data[] =
{
    1, 0, 0,
    1, 0, 0,
    1, 0, 0,

    0, 1, 0,
    0, 1, 0,
    0, 1, 0,

    0, 0, 1,
    0, 0, 1,
    0, 0, 1
};

static GLuint axes_element_data[] =
{
    0, 1, 2,
    0, 2, 1,
    3, 4, 5,
    3, 5, 4,
    6, 7, 8,
    6, 8, 7
};

static mesh_s *placeholder_mesh;

static mesh_s *make_placeholder_mesh()
{
    if(placeholder_mesh != NULL)
        return placeholder_mesh;

    placeholder_mesh = mesh_new();

    GLuint shaders[2];
    shaders[0] = graphics_create_shader(
        GL_FRAGMENT_SHADER, "axes_fragment_shader", axes_fragment_shader);
    shaders[1] = graphics_create_shader(
        GL_VERTEX_SHADER, "axes_vertex_shader", axes_vertex_shader);
    GLuint program = graphics_create_program(2, shaders);

    mesh_set_program(placeholder_mesh, program);
    mesh_set_attribute(placeholder_mesh, "position", 3,
        sizeof(axes_position_data), axes_position_data);
    mesh_set_attribute(placeholder_mesh, "color", 3, sizeof(axes_color_data),
        axes_color_data);
    mesh_set_elements(placeholder_mesh, sizeof(axes_element_data),
        axes_element_data);

    return placeholder_mesh;
}
