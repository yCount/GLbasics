#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "App.hpp"
#include "Utilities.hpp"

struct Transform {
    glm::mat4 mModelMatrix{ glm::mat4(1.0f) };
};

struct Mesh3D {
    GLuint mVertexArrayObject = 0;
    GLuint mVertexBufferObject = 0;
    // Index Buffer Object (IBO)
    // This is used to store the array of indices that we want
    // to draw from, when we do indexed drawing
    GLuint mIndexBufferObject = 0;
    // This is the graphics pipeline used with this mesh
    GLuint mPipeline = 0;
    Transform mTransform;
};

void CreateGraphicsPipeline(App* app);
GLuint CreateShaderProgram(const std::string& vertexshadersource, const std::string& fragmentshadersource);
GLuint CompileShader(GLuint type, const std::string& source);
void MeshDataVertexSpecification(Mesh3D* mesh);
void MeshSetPipeline(Mesh3D* mesh, GLuint pipeline);
void MeshDraw(Mesh3D* mesh, App app);
GLuint FindUniformLocation(GLuint pipeline, const GLchar* name);
void MeshTraslate(Mesh3D* mesh, float x, float y, float z);
void MeshRotateY(Mesh3D *mesh, float yAngle, glm::vec3 axis);
void MeshScale(Mesh3D* mesh, float x, float y, float z);
void MeshScale(Mesh3D* mesh, float s);

#endif