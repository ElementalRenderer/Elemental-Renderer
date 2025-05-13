/**
 * @file PBRShader.h
 * @brief Physically Based Rendering shader for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_PBR_SHADER_H
#define ELEMENTAL_RENDERER_PBR_SHADER_H

#include "../Shader.h"

namespace ElementalRenderer {

/**
 * @brief Specialized shader class for Physically Based Rendering
 */
class PBRShader : public Shader {
public:
    /**
     * @brief Default constructor
     */
    PBRShader();
    
    /**
     * @brief Destructor
     */
    ~PBRShader();
    
    /**
     * @brief Load standard PBR shader
     * @return true if loading was successful, false otherwise
     */
    bool loadStandardPBR();
    
    /**
     * @brief Set parameters for PBR material
     * @param albedo Base color of the material
     * @param metallic Metallic value (0.0 to 1.0)
     * @param roughness Roughness value (0.0 to 1.0)
     * @param ao Ambient occlusion value (0.0 to 1.0)
     */
    void setMaterialProperties(const glm::vec3& albedo, float metallic, float roughness, float ao);
    
    /**
     * @brief Set texture maps for PBR material
     * @param albedoMap Albedo texture ID
     * @param normalMap Normal texture ID
     * @param metallicMap Metallic texture ID
     * @param roughnessMap Roughness texture ID
     * @param aoMap Ambient occlusion texture ID
     */
    void setTextureMaps(unsigned int albedoMap, unsigned int normalMap, unsigned int metallicMap, 
                        unsigned int roughnessMap, unsigned int aoMap);
                        
    /**
     * @brief Set light information for PBR calculations
     * @param positions Array of light positions
     * @param colors Array of light colors
     * @param count Number of lights
     */
    void setLights(const glm::vec3* positions, const glm::vec3* colors, int count);

private:
    // Shader source code for PBR
    static const char* s_vertexShaderSource;
    static const char* s_fragmentShaderSource;
};
/**
 * @file PBRShader.h
 * @brief PBR (Physically Based Rendering) shader for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_PBR_SHADER_H
#define ELEMENTAL_RENDERER_PBR_SHADER_H

#include "Shader.h"

namespace ElementalRenderer {

/**
 * @brief Class for handling PBR (Physically Based Rendering) shaders
 */
class PBRShader : public Shader {
public:
    PBRShader();
    
    ~PBRShader() override = default;
    
    void setAlbedo(const glm::vec3& albedo);
    
    void setMetallic(float metallic);
    
    void setRoughness(float roughness);
    
    void setAO(float ao);
    
    void setUseTextures(bool useTextures);
    
    static std::shared_ptr<PBRShader> create();

private:
    // Shader source code for PBR rendering
    static const char* s_vertexShaderSource;
    static const char* s_fragmentShaderSource;
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_PBR_SHADER_H
} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_PBR_SHADER_H
