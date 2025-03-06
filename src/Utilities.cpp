#include "Utilities.hpp"

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
