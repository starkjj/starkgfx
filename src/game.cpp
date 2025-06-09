#include <iostream>
#include "game.hpp"
#include <gl.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

int main() {
    auto game = Game();
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

    window = SDL_CreateWindow("StarCad", 1280, 720, SDL_WINDOW_OPENGL);
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Create OpenGL context
    gl_context = SDL_GL_CreateContext(window);

    // Check our OpenGL version
    int version = gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress);
    printf("GL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    return 0;
}

auto Game::update() -> void {
    while (!quit) {
        SDL_Event e;

        // Input updates
        while (SDL_PollEvent(&e)) {
            process_input(e);
        }

        glClearColor(0.4, 0.75, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(window);
        SDL_Delay(1);
    }
}


auto Game::process_input(SDL_Event e) -> void {
    if (e.type == SDL_EVENT_QUIT)
        quit = true;
    if (e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && e.window.windowID == SDL_GetWindowID(window))
        quit = true;
    if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_ESCAPE) {
        quit = true;
    }
}

auto Game::destroy() const -> void {
    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
