/**
 * @file WaterShader.h
 * @brief Water surface shader for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_WATER_SHADER_H
#define ELEMENTAL_RENDERER_WATER_SHADER_H

#include "../Shader.h"

namespace ElementalRenderer {

/**
 * @brief Specialized shader class for water surface rendering
 */
class WaterShader : public Shader {
public:
    /**
     * @brief Default constructor
     */
    WaterShader();
    
    /**
     * @brief Destructor
     */
    ~WaterShader();
    
    /**
     * @brief Load water surface shader
     * @return true if loading was successful, false otherwise
     */
    bool loadWaterShader();
    
    /**
     * @brief Set time parameter for water animation
     * @param time Current time value
     */
    void setTime(float time);
    
    /**
     * @brief Set water texture maps
     * @param dudvMap Distortion/DuDv map texture ID
     * @param normalMap Normal map texture ID
     */
    void setWaterMaps(unsigned int dudvMap, unsigned int normalMap);
    
    /**
     * @brief Set reflection and refraction textures
     * @param reflectionTexture Reflection texture ID
     * @param refractionTexture Refraction texture ID
     * @param depthTexture Depth texture ID
     */
    void setWaterTextures(unsigned int reflectionTexture, unsigned int refractionTexture, unsigned int depthTexture);
    
    /**
     * @brief Set water properties
     * @param waveStrength Strength of the waves
     * @param shineDamper Damping factor for specular highlights
     * @param reflectivity Reflectivity of the water
     */
    void setWaterProperties(float waveStrength, float shineDamper, float reflectivity);

private:
    // Shader source code for water surface
    static const char* s_vertexShaderSource;
    static const char* s_fragmentShaderSource;
    
    float m_waveStrength;
    float m_shineDamper;
    float m_reflectivity;
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_WATER_SHADER_H
