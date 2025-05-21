#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <cglm/struct.h>
#include "stb_image.h"
#include "game.h"
#include "shader.h"

vec3s camPos = {0.0f, 0.0f, 3.0f};
vec3s camFront = {0.0f, 0.0f, -1.0f};
vec3s camUp = {0.0f, 1.0f, 0.0f};

float dt = 0.0f;
float lastframe = 0.0f;

int main(void) {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    // Setup global opengl state
    glEnable(GL_DEPTH_TEST);

    GLuint program = create_shader("../src/shaders/triangle.shader");
    use_shader(program);

    GLuint vertex_buffer, vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glGenBuffers(1, &vertex_buffer);

    glBindVertexArray(vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // uv attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create and load texture
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    char *data = stbi_load("../assets/debugempty.png", &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    use_shader(program);
    set_int(program, "ourTexture", 0);

    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        float current_frame = (float)glfwGetTime();
        dt = current_frame - lastframe;
        lastframe = current_frame;

        // input
        process_input(window);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        const float ratio = (float) width / (float) height;

        glViewport(0, 0, width, height);
        glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        use_shader(program);

        // retrieve uniform location
        GLint model_loc = glGetUniformLocation(program, "model");
        GLint view_loc = glGetUniformLocation(program, "view");
        GLint proj_loc = glGetUniformLocation(program, "projection");

        mat4s m = glms_mat4_identity();
        mat4s v = glms_mat4_identity();
        mat4s p;

        p = glms_perspective(glm_rad(45.0f), ratio, 0.1f, 100.0f);
        m = glms_rotate(m, glfwGetTime(), (vec3s){0.5f, 1.0f, 0.0f});

        vec3s center = glms_vec3_add(camPos, camFront);
        mat4s view = glms_lookat(camPos, center, camUp);
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, (GLfloat *)view.raw);

        // pass them back to the shader
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, (GLfloat *)m.raw);
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, (GLfloat *)v.raw);
        glUniformMatrix4fv(proj_loc, 1, GL_FALSE, (GLfloat *)p.raw);

        v = glms_lookat(camPos, camFront, camUp);
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, (GLfloat *)v.raw);

        // render box
        glBindVertexArray(vertex_array);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void process_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cam_speed = (float)(2.5f * dt);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camPos.z -= cam_speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camPos.z += cam_speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camPos.x -= cam_speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camPos.x += cam_speed;
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