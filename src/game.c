#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cglm/struct.h>
#include "game.h"

#include <stdio.h>
#include <stdlib.h>

#include "shader.h"

#include "stb_image.h"

typedef struct Vertex {
    vec3s pos;
    vec3s col;
    vec3s uv;
} Vertex;

static const Vertex vertices[] = {{{0.5f, 0.5f, 0.f }, {1.f, 0.f, 0.f}, {1.f, 1.f}},
                                  {{0.5f, -0.5f, 0.f }, {0.f, 1.f, 0.f}, {1.f, 0.f}},
                                  {{-0.5f, -0.5f, 0.f }, {0.f, 0.f, 1.f}, {0.f, 0.f}},
                                  {{-0.5f, 0.5f, 0.f }, {0.f, 0.f, 1.f}, {0.f, 1.f}}};

static int indices[] = {0, 1, 3, 1, 2, 3};

static void error_callback(int error, const char *description) { fprintf(stderr, "Error: %s\n", description); }

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void) {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);

    GLFWwindow *window = glfwCreateWindow(640, 480, "starkgfx", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    GLuint program = create_shader("../src/shaders/triangle.shader");
    use_shader(program);

    const GLint mvp_location = glGetUniformLocation(program, "MVP");
    const GLint vpos_location = glGetAttribLocation(program, "aPos");
    const GLint vcol_location = glGetAttribLocation(program, "aColor");
    const GLint vuv_location = glGetAttribLocation(program, "aTexCoord");

    GLuint vertex_buffer, vertex_array, element_object;
    glGenVertexArrays(1, &vertex_array);
    glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &element_object);

    glBindVertexArray(vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, pos));
    glEnableVertexAttribArray(vpos_location);

    // color attribute
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, col));
    glEnableVertexAttribArray(vcol_location);

    // uv attribute
    glVertexAttribPointer(vuv_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, uv));
    glEnableVertexAttribArray(vuv_location);

    // Create and load texture
    int width, height, nrChannels;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    char *data = stbi_load("../assets/container.jpg", &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    use_shader(program);
    set_int(program, "ourTexture", 0);

    while (!glfwWindowShouldClose(window)) {

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        const float ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        mat4s m = glms_mat4_identity();
        mat4s p = glms_mat4_identity();
        mat4s mvp = glms_mat4_identity();

        m = glms_rotate_y(m, (float) glfwGetTime() * 1.f);
        p = glms_ortho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mvp = glms_mat4_mul(p, m);

        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *) &mvp);

        use_shader(program);
        glBindVertexArray(vertex_array);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
