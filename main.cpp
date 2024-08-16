//  g++ main.cpp ./src/glad.c -I./include/ -o prog -lSDL2 -ldl
#include <iostream>
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <vector>

// Globals
int gScreenWidth = 640;
int gScreenHeight = 480;
SDL_Window* gGraphicsApplicationWindow = nullptr;
SDL_GLContext gOpenGLContext = nullptr;

bool gQuit = false; // If true, we quit

// Our VAO in this project, made temporarily global
GLuint gVertexArrayObject = 0;
// Our VBO in this project, made temporarily global
GLuint gVertexBufferObject = 0;

// Program Object (for our shaders)
GLuint gGraphicsPipelineShaderProgram = 0;

// Vertex shader executes once per vertex, and will be in charge of
// the final position of the vertex.
const std::string gVertexShaderSource =
    "#version 410 core\n"
    "in vec4 position;\n"
    "void main() {\n"
    "   gl_Position = vec4(position.x, position.y, position.z, position.w);\n"
    "}\n";

// Fragment shader executes once per fragment (i.e. roughly for every pixel that will be rasterized),
// and in part determines the final color that will be sent to the screen.
const std::string gFragmentShaderSource =
    "#version 410 core\n"
    "out vec4 color;\n"
    "void main() {\n"
    "   color = vec4(1.0f, 0.5f, 0.0f, 1.0f);\n"
    "}\n";

void GetOpenGLVersionInfo() {
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

GLuint CompileShader(GLuint type, const std::string& source) {
    GLuint shaderObject;

    // Making sure the type for debug
    if (type == GL_VERTEX_SHADER) {
        shaderObject = glCreateShader(GL_VERTEX_SHADER);
    } else if (type == GL_FRAGMENT_SHADER) {
        shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    }

    const char* src = source.c_str();
    glShaderSource(shaderObject, 1, &src, nullptr);
    glCompileShader(shaderObject);

    return shaderObject;
}

GLuint CreateShaderProgram(const std::string& vertexshadersource, const std::string& fragmentshadersource) {
    GLuint programObject = glCreateProgram();

    GLuint myVertexShader = CompileShader(GL_VERTEX_SHADER, vertexshadersource);
    GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentshadersource);

    glAttachShader(programObject, myVertexShader);
    glAttachShader(programObject, myFragmentShader);
    glLinkProgram(programObject);

    // Validate our program
    glValidateProgram(programObject);
    
    // Once our final program Object has been created, we ca detach and delete our individual shaders.
    glDetachShader(programObject, myVertexShader);
    glDetachShader(programObject, myFragmentShader);

    glDeleteShader(myVertexShader);
    glDeleteShader(myFragmentShader);

    return programObject;
}

void CreateGraphicsPipeline() {
    gGraphicsPipelineShaderProgram = CreateShaderProgram(gVertexShaderSource, gFragmentShaderSource);
}

void VertexSpecification() {
    // Geometry Data
    const std::vector<GLfloat> vertexPosition{
        //x     y     z
        -0.8f, -0.8f, 0.0f, // vertex 1
        0.8f, -0.8f, 0.0f, // vertex 2
        0.0f, 0.8f, 0.0f  // vertex 3
    };

    // We start setting things up on the GPU
    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    // Start generating our VBO
    glGenBuffers(1, &gVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, vertexPosition.size() * sizeof(GLfloat), 
                vertexPosition.data(), GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Unbind our current bound
    glBindVertexArray(0);
    // Disable any arttibutes becuase we don't want to leave them open
    glDisableVertexAttribArray(0);
}

void InitializeProgram() {
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


    gGraphicsApplicationWindow = SDL_CreateWindow("OpenGL Window",
                              0, 0, gScreenWidth,
                              gScreenWidth, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (gGraphicsApplicationWindow==nullptr) {
        std::cout << "SDL_Window was not able to be created" << std::endl;
        exit(1);
    }

    gOpenGLContext = SDL_GL_CreateContext(gGraphicsApplicationWindow);

    if (gOpenGLContext==nullptr) {
        std::cout << "OpenGl context not avaliable" << std::endl;
        exit(1);
    }

    // intialize the Glad library
    if(!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::cout << "glad was not initialized" << std::endl;
        exit(1);
    }
}

void Input() {
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            std::cout << "Goodbye!" << std::endl;
            gQuit = true;
        }
    }
}

void PreDraw() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glViewport(0, 0, gScreenWidth, gScreenHeight);
    glClearColor(1.f, 1.f, 0.f, 1.f);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(gGraphicsPipelineShaderProgram);
}

void Draw() {
    glBindVertexArray(gVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUseProgram(0);
}

void MainLoop() {
    while (!gQuit) {
        Input();

        PreDraw();

        Draw();

        // Update the screen
        SDL_GL_SwapWindow(gGraphicsApplicationWindow);
    }
}

void CleanUp() {
    SDL_DestroyWindow(gGraphicsApplicationWindow);

    SDL_Quit();
}

int main(int argc, char* args[]) {
    
    InitializeProgram();

    VertexSpecification();

    CreateGraphicsPipeline();

    GetOpenGLVersionInfo();

    MainLoop();

    CleanUp();

    return 0;
}