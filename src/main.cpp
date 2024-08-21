//  g++ src/*.cpp src/glad.c -I./include/ -I./common/thirdparty/ -o prog -lSDL2 -ldl

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

// Our libraries
#include "Camera.hpp"

struct App {
    int mScreenWidth = 640;
    int mScreenHeight = 480;
    SDL_Window* mGraphicsApplicationWindow = nullptr;
    SDL_GLContext mOpenGLContext = nullptr;  
    bool mQuit = false; // If true, we quit
    // Program Object (for our shaders)
    GLuint mGraphicsPipelineShaderProgram = 0;

    Camera mCamera;
};

struct Transform {
    float x, y, z;
};

struct Mesh3D {
    // Our VAO in this project, made temporarily global
    GLuint mVertexArrayObject = 0;
    // Our VBO in thise project, made temporarily global
    GLuint mVertexBufferObject = 0;
    // Index Buffer Object (IBO)
    // This is used to store the array of indices that we want
    // to draw from, when we do indexed drawing
    GLuint mIndexBufferObject = 0;

    // This is the graphics pipeline used with this mesh
    GLuint mPipeline = 0;
    Transform mTransform;
    float m_uRotate = 0.1f;
    float m_uScale = 0.5f;
};

// ^^^^^^^^^^^^^^^^ Globals ^^^^^^^^^^^^^^^^
App gApp;
Mesh3D gMesh1;
Mesh3D gMesh2;

// ^^^^^^^^^^^^^^^^ Error Handling Routines ^^^^^^^^^^^^^^^^

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
    gApp.mGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void MeshDataVertexSpecification(Mesh3D* mesh) {
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
    glGenVertexArrays(1, &mesh->mVertexArrayObject);
    glBindVertexArray(mesh->mVertexArrayObject);

    // Start generating our VBO 
    glGenBuffers(1, &mesh->mVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->mVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER,                   // Kind of buffer we are working with
                vertexData.size() * sizeof(GLfloat),// Size of data in bytes
                vertexData.data(),                  // Raw array of data
                GL_STATIC_DRAW);                    // How we intend to use the data


    const std::vector<GLuint> indexBufferObject {2, 0, 1, 3, 2, 1};
    // Setting up Index Buffer Object (IBO)
    glGenBuffers(1, &mesh->mIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 mesh->mIndexBufferObject);
    // Populate our Index Buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indexBufferObject.size()*sizeof(GLuint),
                 indexBufferObject.data(),
                 GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,        // Attribute 0 corresponds to the enabledVertexAttribArray
                          3,        // The number of components
                          GL_FLOAT, // Type
                          false, // is the data normalized
                          sizeof(GLfloat)*6,        // stride (gap between data)
                          (void*)0);// offset

    // Linking attribs in our VBO
    // Color information
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 
                          3, // r,g,b (that's why 3) 
                          GL_FLOAT, 
                          false, 
                          sizeof(GLfloat)*6, 
                          (GLvoid*)(sizeof(GLfloat)*3));

    // Unbind our current bound
    glBindVertexArray(0);
    // Disable any arttibutes becuase we don't want to leave them open
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void InitializeProgram(App *app) {
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

void Input() {
    SDL_Event e;

    static int mouseX = gApp.mScreenWidth/2;
    static int mouseY = gApp.mScreenHeight/2;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            std::cout << "Goodbye!" << std::endl;
            gApp.mQuit = true;
        } else if (e.type == SDL_MOUSEMOTION) {
            mouseX += e.motion.xrel;
            mouseY += e.motion.yrel;
            gApp.mCamera.MouseLook(mouseX, mouseY);
        }
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_LCTRL] && state[SDL_SCANCODE_E]) {
        gApp.mQuit = true;
    }

    if (state[SDL_SCANCODE_UP]) {
        gApp.mCamera.MoveForward(0.05f);
    }
    if (state[SDL_SCANCODE_DOWN]) {
        gApp.mCamera.MoveBackward(0.05f);
    }
    
    if (state[SDL_SCANCODE_LEFT]) {
        gApp.mCamera.MoveLeft(0.01f);
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        gApp.mCamera.MoveRight(0.01f);
    }
}

void MeshSetPipeline(Mesh3D *mesh, GLuint pipeline) {
    mesh->mPipeline = pipeline;
}

GLuint FindUniformLocation(GLuint pipeline, const GLchar* name) {
    GLuint location = glGetUniformLocation(pipeline, name);
    if (location < 0) {
        std::cerr << "Could not find " << name << " maybe a misspelling" << std::endl;
        exit(EXIT_FAILURE);
    }
    return location;
}

void MeshDraw(Mesh3D *mesh) {
    glUseProgram(mesh->mPipeline);

    mesh->m_uRotate -= 1.0f;

    // Model transform by translating our object into world space
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(mesh->mTransform.x, mesh->mTransform.y, mesh->mTransform.z));

    // Uppdate our model matrix by applying a rotation after our translation
    model = glm::rotate(model, glm::radians(mesh->m_uRotate), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(mesh->m_uScale, mesh->m_uScale, mesh->m_uScale));

    GLint u_ModelMatrixLocation = FindUniformLocation(mesh->mPipeline, "u_ModelMatrix");
    glUniformMatrix4fv(u_ModelMatrixLocation, 1, false, &model[0][0]);

    glm::mat4 view = gApp.mCamera.GetViewMatrix();

    GLint u_ViewLocation = FindUniformLocation(mesh->mPipeline, "u_ViewMatrix");
    glUniformMatrix4fv(u_ViewLocation, 1, false, &view[0][0]);

    // Projection matrix (in perspective)
    glm::mat4 perspective = gApp.mCamera.GetProjectionMatrix();

    GLint u_ProjectionLocation = FindUniformLocation(mesh->mPipeline, "u_Perspective");
    glUniformMatrix4fv(u_ProjectionLocation, 1, false, &perspective[0][0]);

    if (mesh == nullptr) {
        return;
    }

    glBindVertexArray(mesh->mVertexArrayObject);
    
    // glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glUseProgram(0);
}

void MainLoop() {

    // Locks Mouse Cursor to the Middle of the Screen
    SDL_WarpMouseInWindow(gApp.mGraphicsApplicationWindow, gApp.mScreenWidth/2, gApp.mScreenHeight/2);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    while (!gApp.mQuit) {
        Input();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        glViewport(0, 0, gApp.mScreenWidth, gApp.mScreenHeight);
        glClearColor(1.f, 1.f, 0.f, 1.f);

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


        MeshDraw(&gMesh1);

        MeshDraw(&gMesh2);

        // Update the screen
        SDL_GL_SwapWindow(gApp.mGraphicsApplicationWindow);
    }
}

void CleanUp() {
    SDL_DestroyWindow(gApp.mGraphicsApplicationWindow);

    glDeleteBuffers(1, &gMesh1.mVertexBufferObject);
    glDeleteVertexArrays(1, &gMesh1.mVertexArrayObject);

    glDeleteBuffers(1, &gMesh2.mVertexBufferObject);
    glDeleteVertexArrays(1, &gMesh2.mVertexArrayObject);

    SDL_Quit();
}

int main(int argc, char* args[]) {
    
    // 1. Setup the graphics program
    InitializeProgram(&gApp);

    // Setup Camera
    gApp.mCamera.SetProjectionMatrix(glm::radians(45.0f), (float)gApp.mScreenWidth/(float)gApp.mScreenHeight, 0.1f, 10.0f);

    // 2. Setup our geometry
    MeshDataVertexSpecification(&gMesh1);
    gMesh1.mTransform.x = 0.0f;
    gMesh1.mTransform.y = 0.0f;
    gMesh1.mTransform.z = -2.0f;

    MeshDataVertexSpecification(&gMesh2);
    gMesh2.mTransform.x = 0.25f;
    gMesh2.mTransform.y = 0.25f;
    gMesh2.mTransform.z = -2.0f;

    // 3. Create our graphics pipeline
    CreateGraphicsPipeline();

    // 3.5 For each mesh, set them to the pipeline
    MeshSetPipeline(&gMesh1, gApp.mGraphicsPipelineShaderProgram);
    MeshSetPipeline(&gMesh2, gApp.mGraphicsPipelineShaderProgram);

    // 4. Call the main application loop
    MainLoop();

    // 5. Call the cleanup function when the program terminates
    CleanUp();

    return 0;
}
