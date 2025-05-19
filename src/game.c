#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cglm/struct.h>
#include "game.h"

#include <stdlib.h>
#include <stdio.h>

#include "shader.h"

typedef struct Vertex
{
    vec2s pos;
    vec3s col;
} Vertex;

static const Vertex vertices[] =
{
    { { 0.5f, 0.5f }, { 1.f, 0.f, 0.f } },
    { {  0.5f, -0.5f }, { 0.f, 1.f, 0.f } },
    { {   -0.f,  -0.5f }, { 0.f, 0.f, 1.f } },
{ {   -0.f,  0.5f }, { 0.f, 0.f, 1.f } }
};

static int indices[] = {
    0, 1, 3,
    1, 2, 3
};

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void)
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint( GLFW_SAMPLES, 8 );

    GLFWwindow* window = glfwCreateWindow(640, 480, "starkgfx", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    // NOTE: OpenGL error checks have been omitted for brevity

    GLuint program = create_shader("../src/shaders/triangle.shader");

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    const GLint mvp_location = glGetUniformLocation(program, "MVP");
    const GLint vpos_location = glGetAttribLocation(program, "vPos");
    const GLint vcol_location = glGetAttribLocation(program, "vCol");

    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, pos));
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, col));

    GLuint element_object;
    glGenBuffers(3, &element_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    double prev_time = glfwGetTime();
    double title_countdown_s = 0.1;
    while (!glfwWindowShouldClose(window))
    {
        double current_time = glfwGetTime();
        double delta_time = current_time - prev_time;
        prev_time = current_time;
        // print FPS
        title_countdown_s -= delta_time;
        if (title_countdown_s <= 0.0 && delta_time > 0.0) {
            double fps = 1.0 / delta_time;

            // Create string and put FPS in window title;
            char tmp[256];
            sprintf(tmp, "FPS: %.2lf", fps);
            glfwSetWindowTitle(window, tmp);
            title_countdown_s = 0.1;
        }

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        const float ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        mat4s m = glms_mat4_identity();
        mat4s p = glms_mat4_identity();
        mat4s mvp = glms_mat4_identity();

        m = glms_rotate_y(m, (float) glfwGetTime() * 1.f);
        p = glms_ortho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mvp = glms_mat4_mul(p, m);

        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp);

        glUseProgram(program);
        glBindVertexArray(vertex_array);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}