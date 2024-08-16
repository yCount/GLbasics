#include <iostream>

#include <SDL.h>
// g++ main.cpp -o prog -I/usr/include/SDL2 `pkg-config --libs sdl2`

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not be initialized " << SDL_GetError();
    } else {
        std::cout << "SDL video system is ready to go \n";
    }

    return 0;
}