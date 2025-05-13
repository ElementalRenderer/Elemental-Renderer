/**
 * @file ShadowShader.h
 * @brief Shadow mapping shader for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_SHADOW_SHADER_H
#define ELEMENTAL_RENDERER_SHADOW_SHADER_H

#include "../Shader.h"

namespace ElementalRenderer {

/**
 * @brief Specialized shader class for shadow mapping
 */
class ShadowShader : public Shader {
public:
    /**
     * @brief Default constructor
     */
    ShadowShader();
    
    /**
     * @brief Destructor
     */
    ~ShadowShader();
    
    /**
     * @brief Load shadow depth shader for generating shadow maps
     * @return true if loading was successful, false otherwise
     */
    bool loadShadowDepthShader();
    
    /**
     * @brief Load main rendering shader with shadow mapping
     * @return true if loading was successful, false otherwise
     */
    bool loadMainShaderWithShadows();
    
    /**
     * @brief Set light space matrix for shadow calculation
     * @param lightSpaceMatrix Matrix that transforms world space to light space
     */
    void setLightSpaceMatrix(const glm::mat4& lightSpaceMatrix);
    
    /**
     * @brief Set shadow map texture
     * @param shadowMap Texture ID of the shadow map
     */
    void setShadowMap(unsigned int shadowMap);
    
    /**
     * @brief Configure shadow mapping parameters
     * @param bias Depth bias to reduce shadow acne
     * @param pcfSize Size of PCF (Percentage Closer Filtering) kernel
     */
    void configureShadows(float bias, int pcfSize);

private:
    // Shader source code for shadow mapping
    static const char* s_depthVertexShaderSource;
    static const char* s_depthFragmentShaderSource;
    
    static const char* s_mainVertexShaderSource;
    static const char* s_mainFragmentShaderSource;
    
    float m_shadowBias;
    int m_pcfSize;
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_SHADOW_SHADER_H
