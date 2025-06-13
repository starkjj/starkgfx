#include "game.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <gl.h>
#include <iostream>
#include "glm/ext/matrix_clip_space.hpp"
#include "primitive.hpp"
#include "stb_image.h"
#include "texture.hpp"

auto game = Game();

bool first_mouse = true;
float lastx = 1280.f/2.f;
float lasty = 720.f/2.f;

float dt = 0.0f;
float lastframe = 0.0f;

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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    game.window = SDL_CreateWindow("StarkGFX", game.window_width, game.window_height, SDL_WINDOW_OPENGL);
    if (game.window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_SetWindowRelativeMouseMode(game.window, true);

    // Create OpenGL context
    game.gl_context = SDL_GL_CreateContext(game.window);
    SDL_GL_MakeCurrent(game.window, game.gl_context);
    SDL_GL_SetSwapInterval(1); // enable vsync
    if (game.gl_context == nullptr) {
       std:: cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
       SDL_Quit();
       return -1;
    }

    // Check our OpenGL version
    int version = gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress);
    printf("GL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    glEnable(GL_DEPTH_TEST);

    return 0;
}

auto Game::update() -> void {
    Shader cube_shader("../../src/shaders/cube/color_cube.vert", "../../src/shaders/cube/color_cube.frag");
    Shader lamp_shader("../../src/shaders/cube/lamp_cube.vert", "../../src/shaders/cube/lamp_cube.frag");

    glm::vec3 light_pos(1.2f, 1.0f, 2.0f);

    // Setup VBO
    unsigned int VBO, cubeVAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &cubeVAO);

    // bind vertex data to VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(game.vertices), game.vertices, GL_STATIC_DRAW);

    // Send our cube data to gpu
    glBindVertexArray(cubeVAO);
    // Send cube's position data to gpu
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Send cube's normal data to gpu
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Send cube's uv data to gpu
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //configure the light_cube VAO (they have the same VBO b/c the vertices are the same)
    unsigned int lampVAO;
    glGenVertexArrays(1, &lampVAO);
    glBindVertexArray(lampVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int diffuseMap = Texture::loadTexture("../../assets/default.png");
    unsigned int specularMap = Texture::loadTexture("../../assets/default.png");

    cube_shader.use();
    cube_shader.set_int("material.diffuse", 0);
    cube_shader.set_int("material.specular", 1);

    while (!game.quit) {
        float currentframe = SDL_GetTicks();
        dt = currentframe - lastframe;
        lastframe = currentframe;

        // Input updates
        SDL_Event e;
        game.process_input(e);

        glClearColor(0.4, 0.75, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Setup lighting shader
        cube_shader.use();
        cube_shader.set_vec3("viewPos", camera.position);
        cube_shader.set_float("material.roughness", 64.0f);

         // directional light
        cube_shader.set_vec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        cube_shader.set_vec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        cube_shader.set_vec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        cube_shader.set_vec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        cube_shader.set_vec3("pointLights[0].position", game.pointLightPositions[0]);
        cube_shader.set_vec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        cube_shader.set_vec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        cube_shader.set_vec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        cube_shader.set_float("pointLights[0].constant", 1.0f);
        cube_shader.set_float("pointLights[0].linear", 0.09f);
        cube_shader.set_float("pointLights[0].quadratic", 0.032f);
        // point light 2
        cube_shader.set_vec3("pointLights[1].position", game.pointLightPositions[1]);
        cube_shader.set_vec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        cube_shader.set_vec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        cube_shader.set_vec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        cube_shader.set_float("pointLights[1].constant", 1.0f);
        cube_shader.set_float("pointLights[1].linear", 0.09f);
        cube_shader.set_float("pointLights[1].quadratic", 0.032f);
        // point light 3
        cube_shader.set_vec3("pointLights[2].position", game.pointLightPositions[2]);
        cube_shader.set_vec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        cube_shader.set_vec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        cube_shader.set_vec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        cube_shader.set_float("pointLights[2].constant", 1.0f);
        cube_shader.set_float("pointLights[2].linear", 0.09f);
        cube_shader.set_float("pointLights[2].quadratic", 0.032f);
        // point light 4
        cube_shader.set_vec3("pointLights[3].position", game.pointLightPositions[3]);
        cube_shader.set_vec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        cube_shader.set_vec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        cube_shader.set_vec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        cube_shader.set_float("pointLights[3].constant", 1.0f);
        cube_shader.set_float("pointLights[3].linear", 0.09f);
        cube_shader.set_float("pointLights[3].quadratic", 0.032f);
        // spotLight
        cube_shader.set_vec3("spotLight.position", camera.position);
        cube_shader.set_vec3("spotLight.direction", camera.front);
        cube_shader.set_vec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        cube_shader.set_vec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        cube_shader.set_vec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        cube_shader.set_float("spotLight.constant", 1.0f);
        cube_shader.set_float("spotLight.linear", 0.09f);
        cube_shader.set_float("spotLight.quadratic", 0.032f);
        cube_shader.set_float("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        cube_shader.set_float("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));  

        // Setup view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(game.camera.fov), game.aspect_ratio, 0.1f, 100.f);
        glm::mat4 view = game.camera.get_view_mat();
        cube_shader.set_mat4("projection", projection);
        cube_shader.set_mat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        cube_shader.set_mat4("model", model);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // render cube
        // render containers
        glBindVertexArray(cubeVAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            cube_shader.set_mat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // render the lamp
        lamp_shader.use();
        lamp_shader.set_mat4("projection", projection);
        lamp_shader.set_mat4("view", view);

        glBindVertexArray(lampVAO);
        for (unsigned int i = 0; i < 4; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, game.pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lamp_shader.set_mat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        SDL_GL_SwapWindow(game.window);
    }
}


auto Game::process_input(SDL_Event e) -> void {
    camera.ProcessKeyboard(dt);

    // Event-based input
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT)
            quit = true;
        if (e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && e.window.windowID == SDL_GetWindowID(window))
            quit = true;
        if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_ESCAPE) {
            quit = true;
        }

        if (e.type == SDL_EVENT_MOUSE_MOTION) {
            camera.ProcessMouseMovement(e.motion.xrel, -e.motion.yrel);
        }
    }
}

auto Game::destroy() const -> void {
    SDL_GL_DestroyContext(game.gl_context);
    SDL_DestroyWindow(game.window);
    SDL_Quit();
}
