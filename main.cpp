#include <iostream>
#include <GL/glew.h>
#include <SDL2/SDL.h>

const std::string strVertexShader(
    "#version 330\n"
    "layout(location = 0) in vec4 position;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = position;\n"
    "}\n"
);

const std::string strFragmentShader(
    "#version 330\n"
    "out vec4 outputColor;\n"
    "void main()\n"
    "{\n"
    "   outputColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
    "}\n"
);

int PrintSDLError(const std::string &prepend) {
        std::cerr << prepend << "Error: " << SDL_GetError() << std::endl;
        return 0;
}

GLuint CreateShader(GLenum eShaderType, const std::string &strShaderFile)
{
    GLuint shader = glCreateShader(eShaderType);
    const char *strFileData = strShaderFile.c_str();
    glShaderSource(shader, 1, &strFileData, NULL);

    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

        const char *strShaderType = NULL;
        switch(eShaderType)
        {
        case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
        case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
        case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
        }

        fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
        delete[] strInfoLog;
    }

    return shader;
}

int InitGL() {
    GLuint vertex_shader = CreateShader(GL_VERTEX_SHADER, strVertexShader);
    GLuint fragment_shader = CreateShader(GL_FRAGMENT_SHADER,
                                          strFragmentShader);
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glLinkProgram(program);

    GLint status;
    glGetProgramiv (program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
        std :: cerr << "Linker failure: " << strInfoLog << std :: endl;
        delete[] strInfoLog;
    }

    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);

    return 0;
}

int Render() {
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
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
    //Request OpenGL 3.3 context.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
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
