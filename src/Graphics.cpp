#include "Graphics.hpp"
#include "App.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

void CreateGraphicsPipeline(App* app) {
    std::string vertexShaderSource = LoadShaderAsString("./shaders/vert.glsl");
    std::string fragmentShaderSource = LoadShaderAsString("./shaders/frag.glsl");
    app->mGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}

GLuint CreateShaderProgram(const std::string& vertexshadersource, const std::string& fragmentshadersource) {
    GLuint programObject = glCreateProgram();

    GLuint myVertexShader = CompileShader(GL_VERTEX_SHADER, vertexshadersource);
    GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentshadersource);

    glAttachShader(programObject, myVertexShader);
    glAttachShader(programObject, myFragmentShader);
    glLinkProgram(programObject);

    glValidateProgram(programObject);
    
    // Once our final program Object has been created, Detach and Delete the shaders.
    glDetachShader(programObject, myVertexShader);
    glDetachShader(programObject, myFragmentShader);

    glDeleteShader(myVertexShader);
    glDeleteShader(myFragmentShader);

    return programObject;
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

void MeshDataVertexSpecification(Mesh3D* mesh, const MeshData& meshData) {
    
    // Setting things up on the GPU
    glGenVertexArrays(1, &mesh->mVertexArrayObject);
    glBindVertexArray(mesh->mVertexArrayObject);

    // Generating VBO 
    glGenBuffers(1, &mesh->mVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->mVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER,                   // Kind of buffer
                meshData.vertices.size()*sizeof(Vertex),// Size of data in bytes
                meshData.vertices.data(),                  // Raw array of data
                GL_STATIC_DRAW);                    // Intend to use the data


    // Setting up Index Buffer Object (IBO)
    glGenBuffers(1, &mesh->mIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 mesh->mIndexBufferObject);
    // Populate the Index Buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 meshData.indices.size()*sizeof(GLuint),
                 meshData.indices.data(),
                 GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,        // Attribute 0 corresponds to the enabledVertexAttribArray
                          3,        // The number of components
                          GL_FLOAT, // Type
                          false, // is the data normalized
                          sizeof(Vertex),        // stride (gap between data)
                          (GLvoid*)offsetof(Vertex, x));// offset

    // Linking attribs in VBO
    // Color information
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 
                          3, // r,g,b (that's why 3) 
                          GL_FLOAT, 
                          false, 
                          sizeof(GLfloat)*6, 
                          (GLvoid*)(sizeof(GLfloat)*3));
    
    // Setting index count
    mesh->mIndexCount = static_cast<GLsizei>(meshData.indices.size());

    // Unbind current bound
    glBindVertexArray(0);
    // Disable any arttibutes as if deconstructor
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void MeshSetPipeline(Mesh3D* mesh, GLuint pipeline) {
    mesh->mPipeline = pipeline;
}

void MeshDraw(Mesh3D* mesh, App app) {
    glUseProgram(mesh->mPipeline);

    MeshRotateY(mesh, 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));

    GLint u_ModelMatrixLocation = FindUniformLocation(mesh->mPipeline, "u_ModelMatrix");
    glUniformMatrix4fv(u_ModelMatrixLocation, 1, false, &mesh->mTransform.mModelMatrix[0][0]);

    glm::mat4 view = app.mCamera.GetViewMatrix();

    GLint u_ViewLocation = FindUniformLocation(mesh->mPipeline, "u_ViewMatrix");
    glUniformMatrix4fv(u_ViewLocation, 1, false, &view[0][0]);

    // Projection matrix (in perspective)
    glm::mat4 perspective = app.mCamera.GetProjectionMatrix();

    GLint u_ProjectionLocation = FindUniformLocation(mesh->mPipeline, "u_Perspective");
    glUniformMatrix4fv(u_ProjectionLocation, 1, false, &perspective[0][0]);

    if (mesh == nullptr) {
        return;
    }

    glBindVertexArray(mesh->mVertexArrayObject);
    
    glDrawElements(GL_TRIANGLES, mesh->mIndexCount, GL_UNSIGNED_INT, 0);

    glUseProgram(0);
}

GLuint FindUniformLocation(GLuint pipeline, const GLchar* name) {
    GLuint location = glGetUniformLocation(pipeline, name);
    if (location < 0) {
        std::cerr << "Could not find " << name << " maybe a misspelling" << std::endl;
        exit(EXIT_FAILURE);
    }
    return location;
}

void MeshTraslate(Mesh3D* mesh, float x, float y, float z) {
    mesh->mTransform.mModelMatrix = glm::translate(mesh->mTransform.mModelMatrix, glm::vec3(x, y, z));
}

void MeshRotateY(Mesh3D *mesh, float yAngle, glm::vec3 axis) {
    mesh->mTransform.mModelMatrix = glm::rotate(mesh->mTransform.mModelMatrix, glm::radians(yAngle), axis);
}

void MeshScale(Mesh3D* mesh, float x, float y, float z) {
    mesh->mTransform.mModelMatrix = glm::scale(mesh->mTransform.mModelMatrix, glm::vec3(x, y, z));
}

void MeshScale(Mesh3D* mesh, float s) {
    MeshScale(mesh, s, s, s);
}
