#include "MeshData.hpp"

#include <map>

const MeshData GenerateSphere(unsigned int subdivisions) {
    MeshData sphere;

    // Starts with a Tetrahedron
    std::vector<Vertex> vertices = {
        { 1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f },
        {-1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.0f },
        {-1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 1.0f },
        { 1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 0.0f }
    };

    std::vector<GLuint> indices = {
        0, 1, 2,  1, 2, 3,  2, 3, 0,  3, 0, 1
    };

    // Subdivision loop
    for (unsigned int i = 0; i < subdivisions; ++i) {
        std::vector<GLuint> newIndices;
        std::map<std::pair<GLuint, GLuint>, GLuint> midpointCache;

        auto getMidpoint = [&](GLuint a, GLuint b) -> GLuint {
            std::pair<GLuint, GLuint> edge = (a < b) ? std::make_pair(a, b) : std::make_pair(b, a);
            if (midpointCache.find(edge) != midpointCache.end()) {
                return midpointCache[edge];
            }
            
            // Compute midpoint
            Vertex mid;
            mid.x = (vertices[a].x + vertices[b].x) * 0.5f;
            mid.y = (vertices[a].y + vertices[b].y) * 0.5f;
            mid.z = (vertices[a].z + vertices[b].z) * 0.5f;
            
            // Normalize to unit sphere
            float length = sqrt(mid.x * mid.x + mid.y * mid.y + mid.z * mid.z);
            mid.x /= length;
            mid.y /= length;
            mid.z /= length;
            
            mid.r = (vertices[a].r + vertices[b].r) * 0.5f;
            mid.g = (vertices[a].g + vertices[b].g) * 0.5f;
            mid.b = (vertices[a].b + vertices[b].b) * 0.5f;

            GLuint index = vertices.size();
            vertices.push_back(mid);
            midpointCache[edge] = index;
            return index;
        };

        for (size_t j = 0; j < indices.size(); j += 3) {
            GLuint a = indices[j], b = indices[j + 1], c = indices[j + 2];
            GLuint ab = getMidpoint(a, b);
            GLuint bc = getMidpoint(b, c);
            GLuint ca = getMidpoint(c, a);

            // Generate new indices
            newIndices.insert(newIndices.end(), {a, ab, ca, ab, b, bc, bc, c, ca, ab, bc, ca});
        }

        // Replace old indices with new ones
        indices.clear();
        indices = std::move(newIndices);
    }

    sphere.vertices = vertices;
    sphere.indices = indices;
    return sphere;
}