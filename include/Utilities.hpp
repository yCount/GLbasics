#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>

static void GLCLearAllErrors();
static bool GLChekErrorStatus(const char* function, int line);
void GetOpenGLVersionInfo();
std::string LoadShaderAsString(const std::string& filename);

#endif
