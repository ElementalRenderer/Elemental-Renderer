/**
 * @file Material.h
 * @brief Material system for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_MATERIAL_H
#define ELEMENTAL_RENDERER_MATERIAL_H

#include <memory>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace ElementalRenderer {

class Shader;
class Texture;

/**
 * @brief Class for handling material properties
 */
class Material {
public:
    Material();
    
    explicit Material(std::shared_ptr<Shader> shader);
    
    void setShader(std::shared_ptr<Shader> shader);
    
    std::shared_ptr<Shader> getShader() const;
    
    void setTexture(const std::string& name, std::shared_ptr<Texture> texture, unsigned int unit = 0);
    
    std::shared_ptr<Texture> getTexture(const std::string& name) const;
    
    void setFloat(const std::string& name, float value);
    
    void setInt(const std::string& name, int value);
    
    void setBool(const std::string& name, bool value);
    
    void setVec2(const std::string& name, const glm::vec2& value);
    
    void setVec3(const std::string& name, const glm::vec3& value);
    
    void setVec4(const std::string& name, const glm::vec4& value);
    
    void setMat2(const std::string& name, const glm::mat2& value);
    
    void setMat3(const std::string& name, const glm::mat3& value);
    
    void setMat4(const std::string& name, const glm::mat4& value);
    
    void apply() const;
    
    static std::shared_ptr<Material> createPBRMaterial(const glm::vec3& albedo = glm::vec3(1.0f), 
                                                      float metallic = 0.0f, 
                                                      float roughness = 0.5f);

private:
    std::shared_ptr<Shader> m_shader;
    
    struct TextureSlot {
        std::shared_ptr<Texture> texture;
        unsigned int unit;
    };
    
    std::unordered_map<std::string, TextureSlot> m_textures;
    std::unordered_map<std::string, float> m_floatProperties;
    std::unordered_map<std::string, int> m_intProperties;
    std::unordered_map<std::string, bool> m_boolProperties;
    std::unordered_map<std::string, glm::vec2> m_vec2Properties;
    std::unordered_map<std::string, glm::vec3> m_vec3Properties;
    std::unordered_map<std::string, glm::vec4> m_vec4Properties;
    std::unordered_map<std::string, glm::mat2> m_mat2Properties;
    std::unordered_map<std::string, glm::mat3> m_mat3Properties;
    std::unordered_map<std::string, glm::mat4> m_mat4Properties;
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_MATERIAL_H
