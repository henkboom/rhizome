#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <GL/glew.h>
#include <GL/glfw.h>

GLuint graphics_create_shader(
    GLenum shader_type,
    const char *source_name,
    const char *code);

GLuint graphics_create_shader_from_file(
    GLenum shader_type,
    const char *filename);

GLuint graphics_create_program(int count, GLuint *shaders);

GLuint graphics_make_buffer(
    GLenum target,
    GLsizei size,
    const void *data,
    GLenum usage);

typedef struct _mesh_s mesh_s;

mesh_s *mesh_new();
void mesh_release(mesh_s *mesh);
void mesh_set_program(mesh_s *mesh, GLuint program);
void mesh_set_elements(mesh_s *mesh, GLsizei size, GLuint *elements);
void mesh_set_attribute(
    mesh_s *mesh,
    const char *name,
    GLint components,
    GLsizei size,
    GLfloat *attribute_data);
void mesh_render(mesh_s *mesh);

#endif
