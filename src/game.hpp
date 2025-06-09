#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

struct Game {
    auto init() -> int;
    auto update() -> void;
    auto process_input(SDL_Event e) -> void;
    auto destroy() const -> void;

    SDL_Window* window{};
    SDL_GLContext gl_context{};
    bool quit = false;
};