#ifndef APP_HPP
#define APP_HPP

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include "Camera.hpp"

struct App {
    int mScreenWidth = 640;
    int mScreenHeight = 480;
    SDL_Window* mGraphicsApplicationWindow = nullptr;
    SDL_GLContext mOpenGLContext = nullptr;
    bool mQuit = false;
    // Program Object (for our shaders)
    GLuint mGraphicsPipelineShaderProgram = 0;
    Camera mCamera;
};

void InitializeProgram(App* app);
void CleanUp(App& app);

#endif