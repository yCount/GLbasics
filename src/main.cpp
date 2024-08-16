//  g++ src/*.cpp src/glad.c -I./include/ -I./../common/thirdparty/ -o prog -lSDL2 -ldl

// Third Party Libraries
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// C++ Standard Template Library (STL)
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

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


// Index Buffer Object (IBO)
// This is used to store the array of indices that we want
// to draw from, when we do indexed drawing
GLuint gIndexBufferObject = 0;

float g_uOffset = -2.0f;
float g_uRotate = 0.0f;
float g_uScale = 0.5f;

// Program Object (for our shaders)
GLuint gGraphicsPipelineShaderProgram = 0;

// ^^^^^^^^^^^^^^^^ Error Handling Routines ^^^^^^^^^^^^^^^^^^^

static void GLCLearAllErrors() {
    while (glGetError() != GL_NO_ERROR) {
    }
}

// Returns True if we had error
static bool GLChekErrorStatus(const char* function, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "OpenGL Error: " << error << 
                  "\tLine: " << line << 
                  "\tfunction: " << function << std::endl;
        return true;
    }
    return false;
}
#define GLCheck(x) GLCLearAllErrors(); x; GLCheckErrorStatus(#x, __LINE__);

void GetOpenGLVersionInfo() {
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

std::string LoadShaderAsString(const std::string& filename) {
    // Resulting shader program loaded as a single string
    std::string result = "";
    
    std::string line = "";
    std::ifstream myFile(filename.c_str());

    if (myFile.is_open()) {
        while (std::getline(myFile, line)) {
            result += line + "\n";
        }
        myFile.close();
    }
    return result;
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

    std::string vertexShaderSource = LoadShaderAsString("./shaders/vert.glsl");
    std::string fragmentShaderSource = LoadShaderAsString("./shaders/frag.glsl");

    gGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void VertexSpecification() {
    // Geometry Data
    const std::vector<GLfloat> vertexData{
        //x     y     z
        //r     g     b
        -0.5f, -0.5f, 0.0f,// vertex 1
        1.0f, 0.0f, 0.0f,  // color
        0.5f, -0.5f, 0.0f, // vertex 2
        0.0f, 1.0f, 0.0f,  // color
        -0.5f, 0.5f, 0.0f, // vertex 3
        0.0f, 0.0f, 1.0f,  //color
        0.5f, 0.5f, 0.0f,  // vertex 4
        1.0f, 0.0f, 0.0f   //color
    };

    // We start setting things up on the GPU
    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    // Start generating our VBO 
    glGenBuffers(1, &gVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER,                   // Kind of buffer we are working with
                vertexData.size() * sizeof(GLfloat),// Size of data in bytes
                vertexData.data(),                  // Raw array of data
                GL_STATIC_DRAW);                    // How we intend to use the data


    const std::vector<GLuint> indexBufferObject {2, 0, 1, 3, 2, 1};
    // Setting up Index Buffer Object (IBO)
    glGenBuffers(1, &gIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 gIndexBufferObject);
    // Populate our Index Buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indexBufferObject.size()*sizeof(GLuint),
                 indexBufferObject.data(),
                 GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,        // Attribute 0 corresponds to the enabledVertexAttribArray
                          3,        // The number of components
                          GL_FLOAT, // Type
                          GL_FALSE, // is the data normalized
                          sizeof(GL_FLOAT)*6,        // stride (gap between data)
                          (void*)0);// offset

    // Linking attribs in our VBO
    // Color information
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 
                          3, // r,g,b (that's why 3) 
                          GL_FLOAT, 
                          GL_FALSE, 
                          sizeof(GL_FLOAT)*6, 
                          (GLvoid*)(sizeof(GL_FLOAT)*3));

    // Unbind our current bound
    glBindVertexArray(0);
    // Disable any arttibutes becuase we don't want to leave them open
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
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
                              gScreenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

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

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_LCTRL] && state[SDL_SCANCODE_E]) {
        gQuit = true;
    }

    if (state[SDL_SCANCODE_UP]) {
        g_uOffset+=0.01f;
        std::cout << "g_uOffset: " << g_uOffset << std::endl;
    }
    if (state[SDL_SCANCODE_DOWN]) {
        g_uOffset-=0.01f;
        std::cout << "g_uOffset: " << g_uOffset << std::endl;
    }
    
    if (state[SDL_SCANCODE_LEFT]) {
        g_uRotate+=1.0f;
        std::cout << "g_uRotate: " << g_uRotate << std::endl;
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        g_uRotate-=1.0f;
        std::cout << "g_uRotate: " << g_uRotate << std::endl;
    }
}

void PreDraw() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glViewport(0, 0, gScreenWidth, gScreenHeight);
    glClearColor(1.f, 1.f, 0.f, 1.f);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(gGraphicsPipelineShaderProgram);

    // Model transform by translating our object into world space
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, g_uOffset));

    // Uppdate our model matrix by applying a rotation after our translation
    model = glm::rotate(model, glm::radians(g_uRotate), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(g_uScale, g_uScale, g_uScale));

    GLint u_ModelMatrixLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_ModelMatrix");

    if (u_ModelMatrixLocation >= 0) {
        // glUniform1f(u_ModelMatrixLocation, g_uOffset); // specify value for the uniform variable
        glUniformMatrix4fv(u_ModelMatrixLocation, 1, GL_FALSE, &model[0][0]);
    } else {
        std::cout << "Could not find u_ModelMatrix, maybe a misspelling" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Projection matrix (in perspective)
    glm::mat4 perspective = glm::perspective(glm::radians(45.0f),
                                             (float)gScreenWidth/(float)gScreenHeight,
                                             0.1f,
                                             10.0f);

    // Retrieve our location of our Model Matrix
    GLint u_ProjectionLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_Perspective");

    if (u_ProjectionLocation >= 0) {
        glUniformMatrix4fv(u_ProjectionLocation, 1, GL_FALSE, &perspective[0][0]);
    } else {
        std::cout << "Could not find u_Perspectve, maybe a misspelling" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Draw() {
    glBindVertexArray(gVertexArrayObject);
    
    // glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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