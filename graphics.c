#include "graphics.h"

#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"

#define error_on(expr, ...) \
do \
{ \
    if(expr) error_at_line(-1, errno, __FILE__, __LINE__, __VA_ARGS__); \
} while(0)

GLuint graphics_create_shader(
    GLenum shader_type,
    const char *source_name,
    const char *code)
{
    GLuint shader = glCreateShader(shader_type);
    GLint size = strlen(code);
    glShaderSource(shader, 1, (const char **)&code, &size);
    glCompileShader(shader);

    GLint log_length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
    if(log_length > 1)
    {
        GLchar *log = malloc((log_length+1) * sizeof(GLchar));
        glGetShaderInfoLog(shader, log_length, NULL, log);
        log[log_length] = 0;
        
        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if(status == GL_FALSE)
            error(-1, 0, "%s: %s", source_name, log);
        else
            printf("%s: %s\n", source_name, log);
        free(log);
    }

    return shader;
}

GLuint graphics_create_shader_from_file(
    GLenum shader_type,
    const char *filename)
{
    FILE *file = fopen(filename, "rb");
    error_on(!file, "create_shader '%s'", filename);
    error_on(fseek(file, 0, SEEK_END) < 0, "create_shader '%s'", filename);
    GLint size = ftell(file);
    error_on(size < 0, "create_shader '%s'", filename);
    rewind(file);

    char *data = malloc(size);
    error_on(fread(data, size, 1, file) < 1, "create_shader '%s'", filename);
    
    return graphics_create_shader(shader_type, filename, data);
}

GLuint graphics_create_program(int count, GLuint *shaders)
{
    GLuint program = glCreateProgram();
    
    for(int i = 0; i < count; i++)
        glAttachShader(program, shaders[i]);
    
    glLinkProgram(program);
    
    GLint log_length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
    if(log_length > 1)
    {
        GLchar *log = malloc((log_length+1) * sizeof(GLchar));
        glGetProgramInfoLog(program, log_length, NULL, log);
        log[log_length] = 0;
        
        GLint status;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if(status == GL_FALSE)
            error(-1, 0, "Link error: %s", log);
        else
            printf("Link message: %s\n", log);
        free(log);
    }
    
    return program;
}

GLuint graphics_make_buffer(
    GLenum target,
    GLsizei size,
    const void *data,
    GLenum usage)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, size, data, usage);
    glBindBuffer(target, 0);
    return buffer;
}

typedef struct {
    const char *name;
    GLint components;
    GLuint buffer;
} mesh_attribute_s;

typedef struct {
    int count;
    GLuint buffer;
} mesh_elements_s;

struct _mesh_s {
    GLuint program;
    mesh_elements_s elements;
    array_of(mesh_attribute_s) attributes;
};

mesh_s *mesh_new()
{
    mesh_s *mesh = malloc(sizeof(mesh_s));
    mesh->elements.count = 0;
    mesh->elements.buffer = 0;
    mesh->attributes = array_new();
    return mesh;
}

void mesh_release(mesh_s *mesh)
{
    if(mesh->elements.buffer != 0)
        glDeleteBuffers(1, &mesh->elements.buffer);

    for(int i = 0; i < array_length(mesh->attributes); i++)
    {
        mesh_attribute_s *attribute = array_get_ptr(mesh->attributes) + i;
        free((char *)attribute->name);
        glDeleteBuffers(1, &attribute->buffer);
    }
    array_release(mesh->attributes);

    free(mesh);
}

void mesh_set_program(mesh_s *mesh, GLuint program)
{
    mesh->program = program;
}

void mesh_set_elements(mesh_s *mesh, GLsizei size, GLuint *elements)
{
    if(mesh->elements.buffer != 0)
        glDeleteBuffers(1, &mesh->elements.buffer);

    mesh->elements.buffer = graphics_make_buffer(
        GL_ELEMENT_ARRAY_BUFFER, size, elements, GL_STATIC_DRAW);

    mesh->elements.count = size / sizeof(*elements);
}

void mesh_set_attribute(
    mesh_s *mesh,
    const char *name,
    GLint components,
    GLsizei size,
    GLfloat *attribute_data)
{
    mesh_attribute_s *target_attribute = NULL;
    for(int i = 0; i < array_length(mesh->attributes); i++)
    {
        if(strcmp(array_get(mesh->attributes, i).name, name) == 0)
        {
            target_attribute = array_get_ptr(mesh->attributes) + i;
        }
    }
    if(target_attribute == NULL)
    {
        mesh_attribute_s new_attribute = {NULL, 0};
        array_add(mesh->attributes, new_attribute);
        target_attribute = array_get_ptr(mesh->attributes)
                         + array_length(mesh->attributes) - 1;
    }

    if(target_attribute->name == NULL)
    {
        char *name_buf = malloc(strlen(name) + 1);
        strcpy(name_buf, name);
        target_attribute->name = name_buf;
    }

    if(target_attribute->buffer != 0)
        glDeleteBuffers(1, &target_attribute->buffer);

    target_attribute->buffer = graphics_make_buffer(
        GL_ARRAY_BUFFER, size, attribute_data, GL_STATIC_DRAW);

    target_attribute->components = components;
}

void mesh_render(mesh_s *mesh)
{
    for(int i = 0 ; i < array_length(mesh->attributes); i++)
    {
        mesh_attribute_s *attribute = array_get_ptr(mesh->attributes) + i;
        GLint location = glGetAttribLocation(mesh->program, attribute->name);
        if(location >= 0)
        {
            printf("binding %d to %s/%d\n", attribute->buffer,
                   attribute->name, location);
            glBindBuffer(GL_ARRAY_BUFFER, attribute->buffer);
            glVertexAttribPointer(
                location, attribute->components, GL_FLOAT, GL_FALSE,
                sizeof(GLfloat) * attribute->components, (void *)0);
            glEnableVertexAttribArray(location);
        }
    }

    printf("%d\n", mesh->program);
    glUseProgram(mesh->program);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->elements.buffer);
    glDrawElements(
        GL_TRIANGLES, mesh->elements.count, GL_UNSIGNED_INT, (void *)0);
    glUseProgram(0);

    for(int i = 0 ; i < array_length(mesh->attributes); i++)
    {
        mesh_attribute_s *attribute = array_get_ptr(mesh->attributes) + i;
        GLint location = glGetAttribLocation(mesh->program, attribute->name);
        if(location >= 0)
        {
            glDisableVertexAttribArray(location);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
