#pragma once

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE 1024
#define MAX_SIZE 8192

typedef enum { NONE = -1, VERTEX = 0, FRAGMENT = 1 } shader_type;

typedef struct {
    GLuint id;
    char *vertex;
    char *fragment;
} shader_data;

static GLuint create_shader(const char *filepath) {
    // Validate and open shader file
    assert(filepath);
    FILE *file = fopen(filepath, "r");

    shader_type type = NONE;
    char vertex[MAX_SIZE] = {0};
    char fragment[MAX_SIZE] = {0};

    // Parse shader file
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "#shader vertex")) {
            type = VERTEX;
            continue;
        }

        if (strstr(line, "#shader fragment")) {
            type = FRAGMENT;
            continue;
        }

        if (type == VERTEX) {
            strncat(vertex, line, MAX_LINE);
        } else if (type == FRAGMENT) {
            strncat(fragment, line, MAX_LINE);
        }
    }
    fclose(file);

    GLuint shader_program = glCreateProgram();
    GLuint vertex_shader_handle = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_handle = glCreateShader(GL_FRAGMENT_SHADER);

    // Store our shader data
    shader_data data = (shader_data) {shader_program, vertex, fragment};

    // Compile vertex shader
    char vertlog[512];
    glShaderSource(vertex_shader_handle, 1, &data.vertex, NULL);
    glCompileShader(vertex_shader_handle);
    int vparams = -1;
    glGetShaderiv(vertex_shader_handle, GL_COMPILE_STATUS, &vparams);
    if (GL_TRUE != vparams) {
        glGetShaderInfoLog(vertex_shader_handle, 512, NULL, vertlog);
        fprintf(stderr, "Error compiling vertex shader: %s \n", vertlog);
        glDeleteShader(vertex_shader_handle);
        glDeleteShader(fragment_shader_handle);
        glDeleteProgram(shader_program);
        return 0;
    }

    // Compile fragment shader
    char fraglog[512];
    glShaderSource(fragment_shader_handle, 1, &data.fragment, NULL);
    glCompileShader(fragment_shader_handle);
    int fparams = -1;
    glGetShaderiv(fragment_shader_handle, GL_COMPILE_STATUS, &fparams);
    if (GL_TRUE != fparams) {
        glGetShaderInfoLog(fragment_shader_handle, 512, NULL, fraglog);
        fprintf(stderr, "Error compiling fragment shader: %s \n", fraglog);
        glDeleteShader(fragment_shader_handle);
        glDeleteShader(vertex_shader_handle);
        glDeleteProgram(shader_program);
        return 0;
    }

    // Attach shader
    glAttachShader(shader_program, vertex_shader_handle);
    glAttachShader(shader_program, fragment_shader_handle);

    // Link shader program
    glLinkProgram(shader_program);
    glDeleteShader(vertex_shader_handle);
    glDeleteShader(fragment_shader_handle);
    int sparams = -1;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &sparams);
    if (GL_TRUE != sparams) {
        fprintf(stderr, "Error linking shader program at index %u \n", shader_program);
        glDeleteProgram(shader_program);
        return 0;
    }

    return shader_program;
}

static void use_shader(const GLuint id) {
    glUseProgram(id);
}

static void set_bool(const GLuint id, const char* name, bool value) {
    glUniform1i(glGetUniformLocation(id, name), (int)value);
}

static void set_int(const GLuint id, const char* name, int value) {
    glUniform1i(glGetUniformLocation(id, name), value);
}

static void set_float(const GLuint id, const char* name, float value) {
    glUniform1f(glGetUniformLocation(id, name), value);
}

static void set_mat4s(const GLuint id, const char* name, const mat4s value) {
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, (GLfloat *) value.raw);
}
