#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <cglm/struct.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"

#include "graphics.h"
#include "primitive.h"

game g;

bool first_mouse = true;
float lastx =  640.0f / 2.0;
float lasty =  480.0 / 2.0;

float dt = 0.0f;
float lastframe = 0.0f;

int main(void) {
    g.cam = camera_new();
    g.gfx = graphics_new();

    create_window(g.gfx, 640, 480);

    // Setup callbacks
    glfwSetCursorPosCallback(g.gfx->window, mouse_callback);
    glfwSetKeyCallback(g.gfx->window, key_callback);

    // Make primitive test
    create_pcube(
        {0.f, 3.f, 0.f},
        {1.f, 1.f, 1.f}
    );

    // MAIN LOOP
    while (!glfwWindowShouldClose(g.gfx->window)) {
        // per-frame time logic
        float current_frame = (float) glfwGetTime();
        dt = current_frame - lastframe;
        lastframe = current_frame;

        // input
        process_input(g.gfx->window);

        // render graphics
        render(g.gfx, dt);

        glfwPollEvents();
    }


    glfwDestroyWindow(g.gfx->window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void process_input(GLFWwindow *window)
{
    if (glfwGetKey(g.gfx->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        g.cam->pos = glms_vec3_muladds(g.cam->dir, g.cam->movement_speed * dt, g.cam->pos);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        g.cam->pos = glms_vec3_mulsubs(g.cam->dir, g.cam->movement_speed * dt, g.cam->pos);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        g.cam->pos = glms_vec3_mulsubs(glms_normalize(glms_cross(g.cam->dir, g.cam->up)), g.cam->movement_speed * dt, g.cam->pos);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        g.cam->pos = glms_vec3_muladds(glms_normalize(glms_cross(g.cam->dir, g.cam->up)), g.cam->movement_speed * dt, g.cam->pos);

    // locks player to xy plane
    // camPos.y = 0.0f;
}

void mouse_callback(GLFWwindow *window, double xposin, double yposin) {
    float xpos = (float)(xposin);
    float ypos = (float)(yposin);

    if (first_mouse)
    {
        lastx = xpos;
        lasty = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - lastx;
    float yoffset = lasty - ypos; // reversed since y-coordinates go from bottom to top
    lastx = xpos;
    lasty = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    g.cam->yaw += xoffset;
    g.cam->pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (g.cam->pitch > 89.0f)
        g.cam->pitch = 89.0f;
    if (g.cam->pitch < -89.0f)
        g.cam->pitch = -89.0f;

    update_camera_vectors(g.cam);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

static void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}