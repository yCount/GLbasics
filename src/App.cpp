#include "App.hpp"
#include <iostream>
#include <cstdlib>

void InitializeProgram(App* app) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL2 could not be initialized video subsystem" << std::endl;
        exit(1);
    }

    // Setting OpenGL version 4.1, it should work on Windows, Linux, and Mac, but beyond 4.1 Mac does not support OpenGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    // Setting OpenGl core profile, so obsolite functions of OpenGL are disabled (this setting is advised)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);


    app->mGraphicsApplicationWindow = SDL_CreateWindow("OpenGL Window",
                              0, 0, app->mScreenWidth,
                              app->mScreenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (app->mGraphicsApplicationWindow==nullptr) {
        std::cout << "SDL_Window was not able to be created" << std::endl;
        exit(1);
    }

    app->mOpenGLContext = SDL_GL_CreateContext(app->mGraphicsApplicationWindow);

    if (app->mOpenGLContext==nullptr) {
        std::cout << "OpenGl context not avaliable" << std::endl;
        exit(1);
    }

    // intialize the Glad library
    if(!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::cout << "glad was not initialized" << std::endl;
        exit(1);
    }
}

void CleanUp(App& app) {
    SDL_DestroyWindow(app.mGraphicsApplicationWindow);
    SDL_Quit();
}
