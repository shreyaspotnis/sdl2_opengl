#include <iostream>
#include <GL/glew.h>
#include <SDL2/SDL.h>

int PrintSDLError(const std::string &prepend) {
        std::cerr << prepend << "Error: " << SDL_GetError() << std::endl;
        return 0;
}

int main(int argc, char **argv){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Game", // window title
                           100, // x position of window
                           100, // y position of window
                           640,
                           480,
                           SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if (window == NULL) {
        PrintSDLError("SDL_CreateWindow");
        return 1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    if(gl_context == NULL) {
            PrintSDLError("SDL_GL_CreateContext");
            return 1;
    }

    // Initialize glew
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        std :: cerr << "Error: " << glewGetErrorString(glewError)
                    << std :: endl;
    }

    SDL_Delay(2000);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
