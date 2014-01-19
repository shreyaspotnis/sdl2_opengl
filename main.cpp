#include <iostream>
#include <GL/glew.h>
#include <SDL2/SDL.h>

int PrintSDLError(const std::string &prepend) {
        std::cerr << prepend << "Error: " << SDL_GetError() << std::endl;
        return 0;
}

int InitGL() {
    return 0;

}

int Render() {
    return 0;
}

int main(int argc, char **argv){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL2_GL", // window title
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

    // Initialize OpenGL
    if(InitGL() != 0) {
        // destroy window and quit if initializing openGL failed
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    };

    //Our event structure
    SDL_Event e;
    bool quit = false;
    while (!quit) {
        // handle all pending events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = true;
            if (e.type == SDL_KEYDOWN) {
                switch(e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                }
            }
        }
        Render();
        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
