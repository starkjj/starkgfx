#pragma once

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "camera.h"
#include "starkgfx.h"

#include "stb_image.h"
#include "shader.h"

typedef struct {
    GLFWwindow* window;
    int width, height;
    float ratio;
    camera* cam;
} graphics;

graphics* graphics_new();

// setup window and initialize graphics
void create_window(graphics* gfx, int width, int height);
void render(graphics* gfx, float dt);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void error_callback(int error, const char* description);

void draw_cubes_example(graphics* gfx);

vec3s cubePositions[] = {
    (vec3s){ 0.0f,  0.0f,  0.0f},
    (vec3s){ 2.0f,  5.0f, -15.0f},
    (vec3s){-1.5f, -2.2f, -2.5f},
    (vec3s){-3.8f, -2.0f, -12.3f},
    (vec3s){ 2.4f, -0.4f, -3.5f},
    (vec3s){-1.7f,  3.0f, -7.5f},
    (vec3s){ 1.3f, -2.0f, -2.5f},
    (vec3s){ 1.5f,  2.0f, -2.5f},
    (vec3s){ 1.5f,  0.2f, -1.5f},
    (vec3s){-1.3f,  1.0f, -1.5f}
};