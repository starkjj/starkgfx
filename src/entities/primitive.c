#include "primitive.h"


pcube* create_pcube(vec3s position, vec3s scale) {
    pcube* cube = malloc(sizeof(pcube));

    cube->program = create_shader("../src/shaders/triangle.shader");
    use_shader(cube->program);

    glGenVertexArrays(1, &cube->va);
    glGenBuffers(1, &cube->vb);

    glBindVertexArray(cube->va);

    glBindBuffer(GL_ARRAY_BUFFER, cube->vb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // uv attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenTextures(1, &cube->texture);
    glBindTexture(GL_TEXTURE_2D, cube->texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // Create and load texture
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    char *data = stbi_load(debug_texture, &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    use_shader(cube->program);
    set_int(cube->program, "ourTexture", 0);

    return cube;
}

void render_pcube(pcube* cube) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cube->texture);

    use_shader(cube->program);

    // retrieve uniform location
    // GLint model_loc = glGetUniformLocation(cube->program, "model");

    mat4s model = glms_mat4_identity();
    model = glms_rotate(model, glfwGetTime(), (vec3s) {0.5f, 1.0f, 0.0f});

    set_mat4s(cube->program, "model", model);

    glBindVertexArray(cube->va);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // render boxes
    // glBindVertexArray(vertex_array);
    // glDrawArrays(GL_TRIANGLES, 0, 36);
}