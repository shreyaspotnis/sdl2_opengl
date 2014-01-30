// A simple application using SDL2, OpenGL and GLSL to display a red background
// with a green triangle drawn on top. Based heavily (lots of code directly
// copied) on tutorials on http://www.arcsynthesis.org/gltut/.
//
// The tutorials on arcsynthesis are quite good, but they use a slightly
// modified version of the Unofficial OpenGL SDK.
// (http://glsdk.sourceforge.net/docs/html/index.html)
// I wanted a simple hello triangle application that does not use the entire
// SDK.
// Also, I am learning SDL2 these days, so I wanted to use it for managing the
// window instead of freeglut that the tutorials on arcsynthesis use. I looked
// around for a while and couldn't find a great expample of SDL2 along with
// the modern OpenGL Shading language. Hence I decided to write one.



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
    "   outputColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);\n"
    "}\n"
);

const float vertexPositions[] = {
    0.75f, 0.75f, 0.0f, 1.0f,
    0.75f, -0.75f, 0.0f, 1.0f,
    -0.75f, -0.75f, 0.0f, 1.0f,
};

GLuint positionBufferObject;
GLuint vao;
GLuint program;

int PrintSDLError(const std::string &prepend) {
        std::cerr << prepend << "Error: " << SDL_GetError() << std::endl;
        return 0;
}

GLuint CreateShader(GLenum eShaderType, const std::string &strShaderFile) {
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

        fprintf(stderr, "Compile failure in %s shader:\n%s\n",
                strShaderType, strInfoLog);
        delete[] strInfoLog;
    }

    return shader;
}

void InitializeVertexBuffer() {
    glGenBuffers(1, &positionBufferObject);

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int InitGL() {
    // Create shaders
    GLuint vertex_shader = CreateShader(GL_VERTEX_SHADER, strVertexShader);
    GLuint fragment_shader = CreateShader(GL_FRAGMENT_SHADER,
                                          strFragmentShader);

    // Create program
    std :: cout << "Create program" << std :: endl;
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);

    // Delete shader
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
        std :: cerr << "Linker failure: " << strInfoLog << std :: endl;
        delete[] strInfoLog;
    }

    InitializeVertexBuffer();
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    return 0;
}

int Render() {
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);
    glUseProgram(0);

    return 0;
}

int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL2_GL", // window title
                           100, // x position of window
                           100, // y position of window
                           640, // width of the screen
                           480, // height of the screen
                           SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if (window == NULL) {
        PrintSDLError("SDL_CreateWindow");
        return 1;
    }

    // If Major and Minor revisions are not set, shaders fail to compile and
    // give the following error:
    // Compile failure in vertex shader:
    // 0:1(10): error: GLSL 3.30 is not supported.
    // Supported versions are: 1.10, 1.20, 1.30, 1.00 ES, and 3.00 ES

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // If SDL_GL_DEPTH_SIZE is not set, results in only half of the window
    // being used sometimes.
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    //Request OpenGL 3.3 context.
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    if(gl_context == NULL) {
            PrintSDLError("SDL_GL_CreateContext");
            return 1;
    }
    // Initialize glew
    // If glewExperimental is not set to GL_TRUE,
    // the program crashes at glGenVertexArrays(1, &vao); with a segfault
    // see http://stackoverflow.com/questions/8302625/segmentation-fault-at-glgenvertexarrays-1-vao
    glewExperimental = GL_TRUE;
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
