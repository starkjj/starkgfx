#pragma once
#include "starkgfx.h"
#include "camera.h"
#include "graphics.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow *window);
static void error_callback(int error, const char *description);
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);

typedef struct game {
    camera* cam;
    graphics* gfx;
    bool quit;
    bool force_mouse_free;
    bool allow_control_input;
    bool allow_editor_picking;
} game;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// global name
extern game g;