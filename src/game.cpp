#include "game.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <gl.h>
#include <iostream>

#include "glm/ext/matrix_clip_space.hpp"
#include "stb_image.h"

bool first_mouse = true;
float lastx = 1280.f/2.f;
float lasty = 720.f/2.f;

float dt = 0.0f;
float lastframe = 0.0f;

auto game = Game();

int main() {
    game.init();
    game.update();
    game.destroy();

    return 0;
}

auto Game::init() -> int {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        std::cerr << "SDL_Init failed" << std::endl;
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

    game.window = SDL_CreateWindow("StarkGFX", 1280, 720, SDL_WINDOW_OPENGL);
    if (game.window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Create OpenGL context
    game.gl_context = SDL_GL_CreateContext(game.window);
    // glEnable(GL_DEPTH_TEST);

    // Check our OpenGL version
    int version = gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress);
    printf("GL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    SDL_SetWindowRelativeMouseMode(game.window, true);

    return 0;
}

auto Game::update() -> void {
    Shader shader("../../src/shaders/triangle/triangle.vs", "../../src/shaders/triangle/triangle.fs");

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(game.vertices), game.vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
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
    unsigned char *data = stbi_load("../../assets/debugempty.png", &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    shader.use();
    shader.set_int("ourTexture", 0);

    while (!game.quit) {
        float currentframe = SDL_GetTicks();
        dt = currentframe - lastframe;
        lastframe = currentframe;

        // Input updates
        SDL_Event e;
        game.process_input(e);


        glClearColor(0.4, 0.75, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        shader.use();

        // pass projection matrix to shader
        glm::mat4 projection = glm::perspective(glm::radians(game.camera.Zoom), 1280.f / 720.f, 0.1f, 100.f);
        shader.set_mat4("projection", projection);

        //camera/view transformation
        glm::mat4 view = game.camera.GetViewMatrix();
        shader.set_mat4("view", view);

        // render boxes
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++) {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, game.cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.set_mat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        SDL_GL_SwapWindow(game.window);
    }
}


auto Game::process_input(SDL_Event e) -> void {
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT)
            quit = true;
        if (e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && e.window.windowID == SDL_GetWindowID(window))
            quit = true;
        if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_ESCAPE) {
            quit = true;
        }

        if (e.type == SDL_EVENT_MOUSE_MOTION) {
            float xoffset = lastx - e.motion.x;
            float yoffset = lasty - e.motion.y;

            lastx = e.motion.x;
            lasty = e.motion.y;

            camera.ProcessMouseMovement(-xoffset, yoffset);
        }

        if (e.key.scancode == SDL_SCANCODE_W) {
            camera.ProcessKeyboard(FORWARD, dt);
        }
        if (e.key.scancode == SDL_SCANCODE_A) {
            camera.ProcessKeyboard(LEFT, dt);
        }
        if (e.key.scancode == SDL_SCANCODE_S) {
            camera.ProcessKeyboard(BACKWARD, dt);
        }
        if (e.key.scancode == SDL_SCANCODE_D) {
            camera.ProcessKeyboard(RIGHT, dt);
        }
    }
}

auto Game::destroy() const -> void {
    SDL_GL_DestroyContext(game.gl_context);
    SDL_DestroyWindow(game.window);
    SDL_Quit();
}
