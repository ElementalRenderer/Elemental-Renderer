/**
 * @file Mesh.h
 * @brief Mesh data structure for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_MESH_H
#define ELEMENTAL_RENDERER_MESH_H

#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace ElementalRenderer {

class Material;

/**
 * @brief Vertex structure containing position, normal, and texture coordinates
 */
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    
    Vertex() : position(0.0f), normal(0.0f), texCoords(0.0f), tangent(0.0f), bitangent(0.0f) {}
    
    Vertex(const glm::vec3& pos, const glm::vec3& norm = glm::vec3(0.0f), 
           const glm::vec2& tex = glm::vec2(0.0f), const glm::vec3& tan = glm::vec3(0.0f), 
           const glm::vec3& bitan = glm::vec3(0.0f))
        : position(pos), normal(norm), texCoords(tex), tangent(tan), bitangent(bitan) {}
};

/**
 * @brief Class for handling mesh data
 */
class Mesh {
public:
    enum class PrimitiveType {
        POINTS,
        LINES,
        LINE_STRIP,
        LINE_LOOP,
        TRIANGLES,
        TRIANGLE_STRIP,
        TRIANGLE_FAN
    };
    
    Mesh();
    
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, 
         std::shared_ptr<Material> material = nullptr);
    
    ~Mesh();
    
    bool loadFromFile(const std::string& path);
    
    void setData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    
    void setMaterial(std::shared_ptr<Material> material);
    
    std::shared_ptr<Material> getMaterial() const;
    
    void render() const;
    
    void setPrimitiveType(PrimitiveType type);

    static std::shared_ptr<Mesh> createCube(float size = 1.0f);
    
    static std::shared_ptr<Mesh> createSphere(float radius = 1.0f, int rings = 16, int sectors = 32);
    
    static std::shared_ptr<Mesh> createPlane(float width = 1.0f, float height = 1.0f);

private:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::shared_ptr<Material> m_material;
    PrimitiveType m_primitiveType;
    
    unsigned int m_vao;
    unsigned int m_vbo;
    unsigned int m_ebo;
    
    void setupMesh();
    
    void calculateTangents();
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_MESH_H
