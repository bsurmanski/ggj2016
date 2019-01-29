#include "sdlWindow.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <SDL2/SDL_opengles2.h>
#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#endif

static bool initialized = false;

SDLWindow::SDLWindow(int w, int h, String name) {
    if(!initialized) {
        SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        initialized = true;
    }

    window = SDL_CreateWindow(name.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            w, h, SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(window);
}

SDLWindow::~SDLWindow() {
}

void SDLWindow::SwapBuffers() {
    SDL_GL_SwapWindow(window);
}

void SDLWindow::Clear() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    //SDL_FillRect(screen, NULL, 0);
}
