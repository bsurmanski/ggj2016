#ifndef _DRAW_SDLWINDOW_HPP
#define _DRAW_SDLWINDOW_HPP

#include <SDL2/SDL.h>
#include "common/string.hpp"

class SDLWindow {
    SDL_Window *window;
    SDL_GLContext context;

    public:
    SDLWindow(int w, int h, String name);
    ~SDLWindow();

    void SwapBuffers();
    void Clear();
};

#endif
