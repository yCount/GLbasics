#include "Input.hpp"
#include <SDL2/SDL.h>
#include <iostream>

void Input(App* app) {
        SDL_Event e;

    static int mouseX = app->mScreenWidth/2;
    static int mouseY = app->mScreenHeight/2;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            std::cout << "Goodbye!" << std::endl;
            app->mQuit = true;
        } else if (e.type == SDL_MOUSEMOTION) {
            mouseX += e.motion.xrel;
            mouseY += e.motion.yrel;
            app->mCamera.MouseLook(mouseX, mouseY);
        }
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_LCTRL] && state[SDL_SCANCODE_E]) {
        app->mQuit = true;
    }

    if (state[SDL_SCANCODE_UP]) {
        app->mCamera.MoveForward(0.05f);
    }
    if (state[SDL_SCANCODE_DOWN]) {
        app->mCamera.MoveBackward(0.05f);
    }
    
    if (state[SDL_SCANCODE_LEFT]) {
        app->mCamera.MoveLeft(0.01f);
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        app->mCamera.MoveRight(0.01f);
    }
}
