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
        SDL_Init(SDL_INIT_EVENTS | SDL_INIT_AUDIO | SDL_INIT_VIDEO);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#ifdef __EMSCRIPTEN__
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#endif
        initialized = true;
    }

    window = SDL_CreateWindow(name.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            w, h, SDL_WINDOW_OPENGL);

    if (!window) {
        printf("Error creating Window: %s\n", SDL_GetError());
    }

    context = SDL_GL_CreateContext(window);

    if (!context) {
        printf("Error creating GL Context: %s\n", SDL_GetError());
    }

    printf ("GL Version: %s\n", glGetString (GL_VERSION));
    printf ("GLSL Version: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
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
