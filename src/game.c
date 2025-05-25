#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <cglm/struct.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "stb_image.h"
#include "shader.h"

game g;
// camera* cam;

// vec3s camPos = {0.0f, 0.0f, 3.0f};
// vec3s camFront = {0.0f, 0.0f, -1.0f};
// vec3s camUp = {0.0f, 1.0f, 0.0f};

bool first_mouse = true;
// float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
// float pitch =  0.0f;
float lastx =  640.0f / 2.0;
float lasty =  480.0 / 2.0;

float dt = 0.0f;
float lastframe = 0.0f;

int main(void) {
    g.cam = camera_new();
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

    // Setup callbacks
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //Disables mouse
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // Create and load texture
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    char *data = stbi_load("../assets/debugempty.png", &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    use_shader(program);
    set_int(program, "ourTexture", 0);

    // MAIN LOOP
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        float current_frame = (float) glfwGetTime();
        dt = current_frame - lastframe;
        lastframe = current_frame;

        // input
        process_input(window);

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

        mat4s model = glms_mat4_identity();
        model = glms_rotate(model, glfwGetTime(), (vec3s) {0.5f, 1.0f, 0.0f});
        mat4s project = glms_perspective(glm_rad(45.0f), ratio, 0.1f, 100.0f);

        // camera/view transformation
        mat4s view = camera_view(g.cam);

        // pass them back to the shader
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, (GLfloat *) view.raw);
        glUniformMatrix4fv(proj_loc, 1, GL_FALSE, (GLfloat *) project.raw);

        // render boxes
        // glBindVertexArray(vertex_array);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(vertex_array);
        for (GLuint i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            mat4s model = glms_mat4_identity(); // make sure to initialize matrix to identity matrix first
            model = glms_translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glms_rotate(model, glm_rad(angle) * glfwGetTime(), (vec3s){1.0f, 0.3f, 0.5f} );
            set_mat4s(program, "model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

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