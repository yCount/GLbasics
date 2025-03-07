#ifndef MESHDATA_HPP
#define MESHDATA_HPP

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

// Define a struct for VertexData
struct Vertex {
    GLfloat x, y, z; // Position
    GLfloat r, g, b; // Color
};

// Mesh data struct to store predefined vertex/index data
struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
};

const MeshData GenerateSphere(unsigned int subdivisions);

// Define reusable mesh templates
namespace MeshTemplates {
    const MeshData Square = {
        {
            //  x      y     z  |   r     g     b
            { -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f }, // Vertex 1
            {  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f }, // Vertex 2
            { -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f }, // Vertex 3
            {  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f }  // Vertex 4
        },
        { 2, 0, 1, 3, 2, 1 } // Indices
    };

    const MeshData Cube = {
        {
            {-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f},
            { 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f},
            { 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f},
            {-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f},
            {-0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f},
            { 0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f},
            { 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f},
            {-0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f}
        },
        { 0, 1, 2, 2, 3, 0,  // Front face
        1, 5, 6, 6, 2, 1,  // Right face
        5, 4, 7, 7, 6, 5,  // Back face
        4, 0, 3, 3, 7, 4,  // Left face
        3, 2, 6, 6, 7, 3,  // Top face
        4, 5, 1, 1, 0, 4 } // Bottom face
    };

    const MeshData Tetrahedron = {
        {
            {  0.0f, 0.5f,   0.5f,  1.0f, 0.0f, 0.0f }, // Vertex 0 - Top
            {  0.5f, 0.5f,   0.0f,  0.0f, 1.0f, 0.0f }, // Vertex 1 - Front Left
            {  0.0f, 0.0f,  0.0f,  0.0f, 0.0f, 1.0f }, // Vertex 2 - Front Right
            {  0.5f, 0.0f,  0.5f,  1.0f, 1.0f, 0.0f }  // Vertex 3 - Back
        },
        {0, 1, 2, // Front Face
        0, 2, 3, // Right Face
        0, 3, 1, // Left Face
        1, 2, 3}  // Bottom Face
    };
    
    const MeshData Sphere = GenerateSphere(9);
}

#endif