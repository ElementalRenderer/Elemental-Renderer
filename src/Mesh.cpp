/**
 * @file Mesh.cpp
 * @brief Implementation of the Mesh class
 */

#include "Mesh.h"
#include "Material.h"
#include <iostream>
#include <glm/gtc/constants.hpp>

namespace ElementalRenderer {

// Vertex implementation
// Constructor and methods are all inline in header file

// Mesh implementation
Mesh::Mesh()
    : m_primitiveType(PrimitiveType::TRIANGLES)
    , m_vao(0)
    , m_vbo(0)
    , m_ebo(0)
{
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, 
           std::shared_ptr<Material> material)
    : m_vertices(vertices)
    , m_indices(indices)
    , m_material(material)
    , m_primitiveType(PrimitiveType::TRIANGLES)
    , m_vao(0)
    , m_vbo(0)
    , m_ebo(0)
{
    calculateTangents();
    setupMesh();
}

Mesh::~Mesh() {
    // In a real implementation, we would clean up OpenGL resources here
    // glDeleteVertexArrays(1, &m_vao);
    // glDeleteBuffers(1, &m_vbo);
    // glDeleteBuffers(1, &m_ebo);
}

bool Mesh::loadFromFile(const std::string& path) {
    // In a real implementation, we would use a library like Assimp to load meshes
    std::cout << "Loading mesh from " << path << std::endl;
    
    // For now, just create a simple default mesh (a triangle)
    m_vertices = {
        Vertex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 1.0f))
    };
    
    m_indices = { 0, 1, 2 };
    
    calculateTangents();
    setupMesh();
    
    return true;
}

void Mesh::setData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    m_vertices = vertices;
    m_indices = indices;
    
    calculateTangents();
    setupMesh();
}

void Mesh::setMaterial(std::shared_ptr<Material> material) {
    m_material = material;
}

std::shared_ptr<Material> Mesh::getMaterial() const {
    return m_material;
}

void Mesh::render() const {
    // In a real implementation, we would bind the VAO and draw the mesh
    // If we have a material, apply it first
    if (m_material) {
        m_material->apply();
    }
    
    // Bind vertex array object
    // glBindVertexArray(m_vao);
    
    // Choose draw method based on primitive type
    // switch (m_primitiveType) {
    //     case PrimitiveType::TRIANGLES:
    //         glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    //         break;
    //     case PrimitiveType::POINTS:
    //         glDrawElements(GL_POINTS, m_indices.size(), GL_UNSIGNED_INT, 0);
    //         break;
    //     // Add more cases for other primitive types
    // }
    
    // Unbind VAO
    // glBindVertexArray(0);
    
    // For now, just output that we're rendering
    std::cout << "Rendering mesh with " << m_vertices.size() << " vertices and " 
              << m_indices.size() << " indices" << std::endl;
}

void Mesh::setPrimitiveType(PrimitiveType type) {
    m_primitiveType = type;
}

std::shared_ptr<Mesh> Mesh::createCube(float size) {
    // Half the size for vertices
    float halfSize = size * 0.5f;
    
    // Create 8 vertices for the corners of the cube
    std::vector<Vertex> vertices = {
        // Front face
        Vertex(glm::vec3(-halfSize, -halfSize, halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)),  // 0: bottom-left-front
        Vertex(glm::vec3(halfSize, -halfSize, halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)),   // 1: bottom-right-front
        Vertex(glm::vec3(halfSize, halfSize, halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)),    // 2: top-right-front
        Vertex(glm::vec3(-halfSize, halfSize, halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)),   // 3: top-left-front
        
        // Back face
        Vertex(glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)), // 4: bottom-left-back
        Vertex(glm::vec3(halfSize, -halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)),  // 5: bottom-right-back
        Vertex(glm::vec3(halfSize, halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)),   // 6: top-right-back
        Vertex(glm::vec3(-halfSize, halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)),  // 7: top-left-back
        
        // Right face
        Vertex(glm::vec3(halfSize, -halfSize, halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),    // 8: bottom-right-front
        Vertex(glm::vec3(halfSize, -halfSize, -halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),   // 9: bottom-right-back
        Vertex(glm::vec3(halfSize, halfSize, -halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)),    // 10: top-right-back
        Vertex(glm::vec3(halfSize, halfSize, halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)),     // 11: top-right-front
        
        // Left face
        Vertex(glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)), // 12: bottom-left-back
        Vertex(glm::vec3(-halfSize, -halfSize, halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),  // 13: bottom-left-front
        Vertex(glm::vec3(-halfSize, halfSize, halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)),   // 14: top-left-front
        Vertex(glm::vec3(-halfSize, halfSize, -halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)),  // 15: top-left-back
        
        // Top face
        Vertex(glm::vec3(-halfSize, halfSize, halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),    // 16: top-left-front
        Vertex(glm::vec3(halfSize, halfSize, halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),     // 17: top-right-front
        Vertex(glm::vec3(halfSize, halfSize, -halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),    // 18: top-right-back
        Vertex(glm::vec3(-halfSize, halfSize, -halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),   // 19: top-left-back
        
        // Bottom face
        Vertex(glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)), // 20: bottom-left-back
        Vertex(glm::vec3(halfSize, -halfSize, -halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),  // 21: bottom-right-back
        Vertex(glm::vec3(halfSize, -halfSize, halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),   // 22: bottom-right-front
        Vertex(glm::vec3(-halfSize, -halfSize, halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f))   // 23: bottom-left-front
    };
    
    // Create indices for the cube (6 faces * 2 triangles * 3 indices)
    std::vector<unsigned int> indices = {
        // Front face
        0, 1, 2,
        2, 3, 0,
        
        // Back face
        5, 4, 7,
        7, 6, 5,
        
        // Right face
        8, 9, 10,
        10, 11, 8,
        
        // Left face
        12, 13, 14,
        14, 15, 12,
        
        // Top face
        16, 17, 18,
        18, 19, 16,
        
        // Bottom face
        20, 21, 22,
        22, 23, 20
    };
    
    // Create and return the mesh
    auto mesh = std::make_shared<Mesh>(vertices, indices);
    return mesh;
}

std::shared_ptr<Mesh> Mesh::createSphere(float radius, int sectors, int stacks) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    // Generate vertices
    float sectorStep = 2 * glm::pi<float>() / sectors;
    float stackStep = glm::pi<float>() / stacks;
    
    for (int i = 0; i <= stacks; ++i) {
        float stackAngle = glm::pi<float>() / 2 - i * stackStep;  // Starting from pi/2 to -pi/2
        float xy = radius * glm::cos(stackAngle);                 // r * cos(u)
        float z = radius * glm::sin(stackAngle);                  // r * sin(u)
        
        // Add vertices for each sector in this stack
        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * sectorStep;  // Starting from 0 to 2pi
            
            // Vertex position
            float x = xy * glm::cos(sectorAngle);  // r * cos(u) * cos(v)
            float y = xy * glm::sin(sectorAngle);  // r * cos(u) * sin(v)
            
            // Normalized vertex normal
            glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
            
            // Texture coordinates
            float s = static_cast<float>(j) / sectors;
            float t = static_cast<float>(i) / stacks;
            
            vertices.push_back(Vertex(
                glm::vec3(x, y, z),    // Position
                normal,                 // Normal
                glm::vec2(s, t)         // Texture coordinates
            ));
        }
    }
    
    // Generate indices
    for (int i = 0; i < stacks; ++i) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;
        
        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            // 2 triangles per sector
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            
            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
    
    // Create and return the mesh
    auto mesh = std::make_shared<Mesh>(vertices, indices);
    return mesh;
}

std::shared_ptr<Mesh> Mesh::createPlane(float width, float depth, int segmentsX, int segmentsZ) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    float halfWidth = width * 0.5f;
    float halfDepth = depth * 0.5f;
    float segmentWidth = width / segmentsX;
    float segmentDepth = depth / segmentsZ;
    
    // Generate vertices
    for (int z = 0; z <= segmentsZ; ++z) {
        for (int x = 0; x <= segmentsX; ++x) {
            float xPos = -halfWidth + x * segmentWidth;
            float zPos = -halfDepth + z * segmentDepth;
            
            // Position
            glm::vec3 position(xPos, 0.0f, zPos);
            
            // Normal (up)
            glm::vec3 normal(0.0f, 1.0f, 0.0f);
            
            // Texture coordinates
            float u = static_cast<float>(x) / segmentsX;
            float v = static_cast<float>(z) / segmentsZ;
            glm::vec2 texCoord(u, v);
            
            vertices.push_back(Vertex(position, normal, texCoord));
        }
    }
    
    // Generate indices
    for (int z = 0; z < segmentsZ; ++z) {
        for (int x = 0; x < segmentsX; ++x) {
            int topLeft = z * (segmentsX + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (segmentsX + 1) + x;
            int bottomRight = bottomLeft + 1;
            
            // First triangle
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            
            // Second triangle
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
    
    // Create and return the mesh
    auto mesh = std::make_shared<Mesh>(vertices, indices);
    return mesh;
}

void Mesh::setupMesh() {
    // In a real implementation, this would set up VAO, VBO, and EBO in OpenGL
    // For example:
    /*
    // Create and bind the VAO
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    
    // Create and bind the VBO
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);
    
    // Create and bind the EBO
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);
    
    // Set the vertex attribute pointers
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    
    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    
    // Texture coordinates attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    
    // Tangent attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    
    // Unbind the VAO
    glBindVertexArray(0);
    */
    
    std::cout << "Mesh setup complete with " << m_vertices.size() << " vertices and " 
              << m_indices.size() << " indices" << std::endl;
}

void Mesh::calculateTangents() {
    // Skip if we don't have enough vertices or indices
    if (m_vertices.size() < 3 || m_indices.size() < 3) {
        return;
    }
    
    // Initialize tangents
    for (auto& vertex : m_vertices) {
        vertex.tangent = glm::vec3(0.0f);
    }
    
    // Calculate tangents for each triangle
    for (size_t i = 0; i < m_indices.size(); i += 3) {
        unsigned int i0 = m_indices[i];
        unsigned int i1 = m_indices[i + 1];
        unsigned int i2 = m_indices[i + 2];
        
        // Get triangle vertices
        const glm::vec3& v0 = m_vertices[i0].position;
        const glm::vec3& v1 = m_vertices[i1].position;
        const glm::vec3& v2 = m_vertices[i2].position;
        
        // Get triangle texture coordinates
        const glm::vec2& uv0 = m_vertices[i0].texCoords;
        const glm::vec2& uv1 = m_vertices[i1].texCoords;
        const glm::vec2& uv2 = m_vertices[i2].texCoords;
        
        // Calculate triangle edges
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        
        // Calculate differences in texture coordinates
        glm::vec2 deltaUV1 = uv1 - uv0;
        glm::vec2 deltaUV2 = uv2 - uv0;
        
        // Calculate tangent
        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        
        glm::vec3 tangent;
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        
        // Add calculated tangent to the vertices of this triangle
        m_vertices[i0].tangent += tangent;
        m_vertices[i1].tangent += tangent;
        m_vertices[i2].tangent += tangent;
    }
    
    // Normalize tangents
    for (auto& vertex : m_vertices) {
        vertex.tangent = glm::normalize(vertex.tangent);
    }
}

} // namespace ElementalRenderer
