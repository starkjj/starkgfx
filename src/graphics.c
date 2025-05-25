#include "graphics.h"
#include "camera.h"

game g;

graphics * graphics_new() {
    graphics* g = malloc(sizeof(graphics));

    g->width = 640;
    g->height = 480;
    g->window = NULL;
    g->ratio = (float)g->width / (float)g->height;
}

void create_window(graphics* gfx, int width, int height) {
    gfx->width = width;
    gfx->height = height;

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

    gfx->window = glfwCreateWindow(gfx->width, gfx->height, "starkgfx", NULL, NULL);
    if (!gfx->window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetInputMode(gfx->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //Disables mouse
    glfwMakeContextCurrent(gfx->window);
    glfwSetFramebufferSizeCallback(gfx->window, framebuffer_size_callback);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    // Setup global opengl state
    glEnable(GL_DEPTH_TEST);
}

void render(graphics* gfx, float dt) {
    glfwGetFramebufferSize(gfx->window, &gfx->width, &gfx->height);

    glViewport(0, 0, gfx->width, gfx->height);
    glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glfwSwapBuffers(gfx->window);
}

void draw_cubes_example(graphics* gfx) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    // use_shader(program);

    // retrieve uniform location
    // GLint model_loc = glGetUniformLocation(program, "model");
    GLint view_loc = glGetUniformLocation(program, "view");
    GLint proj_loc = glGetUniformLocation(program, "projection");

    // mat4s model = glms_mat4_identity();
    // model = glms_rotate(model, glfwGetTime(), (vec3s) {0.5f, 1.0f, 0.0f});
    mat4s project = glms_perspective(glm_rad(45.0f), gfx->ratio, 0.1f, 100.0f);

    // camera/view transformation
    mat4s view = camera_view();

    // pass them back to the shader
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (GLfloat *) view.raw);
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, (GLfloat *) project.raw);

    // render boxes
    // glBindVertexArray(vertex_array);
    // glDrawArrays(GL_TRIANGLES, 0, 36);

    // glBindVertexArray(vertex_array);
    // for (GLuint i = 0; i < 10; i++)
    // {
    //     // calculate the model matrix for each object and pass it to shader before drawing
    //     mat4s model = glms_mat4_identity(); // make sure to initialize matrix to identity matrix first
    //     model = glms_translate(model, cubePositions[i]);
    //     float angle = 20.0f * i;
    //     model = glms_rotate(model, glm_rad(angle) * glfwGetTime(), (vec3s){1.0f, 0.3f, 0.5f} );
    //     set_mat4s(program, "model", model);
    //
    //     glDrawArrays(GL_TRIANGLES, 0, 36);
    // }
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