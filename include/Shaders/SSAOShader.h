/**
 * @file SSAOShader.h
 * @brief Screen Space Ambient Occlusion shader for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_SSAO_SHADER_H
#define ELEMENTAL_RENDERER_SSAO_SHADER_H

#include "../Shader.h"
#include <vector>

namespace ElementalRenderer {

/**
 * @brief Specialized shader class for Screen Space Ambient Occlusion
 */
class SSAOShader : public Shader {
public:
    /**
     * @brief Default constructor
     */
    SSAOShader();
    
    /**
     * @brief Destructor
     */
    ~SSAOShader();
    
    /**
     * @brief Load main SSAO shader
     * @return true if loading was successful, false otherwise
     */
    bool loadSSAOShader();
    
    /**
     * @brief Load SSAO blur shader for smoothing the occlusion result
     * @return true if loading was successful, false otherwise
     */
    bool loadSSAOBlurShader();
    
    /**
     * @brief Generate sample kernel for SSAO calculations
     * @param kernelSize Number of samples in the kernel
     */
    void generateSampleKernel(int kernelSize);
    
    /**
     * @brief Generate noise texture for SSAO
     * @param size Size of the noise texture (typically small, e.g., 4x4)
     * @return Texture ID of the generated noise texture
     */
    unsigned int generateNoiseTexture(int size);
    
    /**
     * @brief Set SSAO parameters
     * @param radius Sampling radius
     * @param bias Occlusion bias
     */
    void setSSAOParameters(float radius, float bias);
    
    /**
     * @brief Set texture inputs for SSAO calculation
     * @param positionTexture Position G-buffer texture ID
     * @param normalTexture Normal G-buffer texture ID
     * @param noiseTexture Noise texture ID
     */
    void setTextureInputs(unsigned int positionTexture, unsigned int normalTexture, unsigned int noiseTexture);

private:
    // Shader source code for SSAO
    static const char* s_ssaoVertexShaderSource;
    static const char* s_ssaoFragmentShaderSource;
    static const char* s_blurVertexShaderSource;
    static const char* s_blurFragmentShaderSource;
    
    std::vector<glm::vec3> m_sampleKernel;
    unsigned int m_noiseTexture;
    int m_kernelSize;
    float m_radius;
    float m_bias;
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_SSAO_SHADER_H
