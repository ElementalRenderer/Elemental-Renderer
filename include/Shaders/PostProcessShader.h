/**
 * @file PostProcessShader.h
 * @brief Post-processing shader for screen space effects
 */

#ifndef ELEMENTAL_RENDERER_POST_PROCESS_SHADER_H
#define ELEMENTAL_RENDERER_POST_PROCESS_SHADER_H

#include "../Shader.h"

namespace ElementalRenderer {

/**
 * @brief Post-processing effect types
 */
enum class PostProcessEffect {
    NONE,
    BLUR,
    GRAYSCALE,
    VIGNETTE,
    CHROMATIC_ABERRATION,
    CUSTOM
};

/**
 * @brief Specialized shader class for post-processing effects
 */
class PostProcessShader : public Shader {
public:
    /**
     * @brief Default constructor
     */
    PostProcessShader();
    
    /**
     * @brief Destructor
     */
    ~PostProcessShader();
    
    /**
     * @brief Load standard post-processing shader
     * @return true if loading was successful, false otherwise
     */
    bool loadStandardPostProcess();
    
    /**
     * @brief Set the effect type
     * @param effect The post-processing effect to use
     */
    void setEffect(PostProcessEffect effect);
    
    /**
     * @brief Set effect strength parameter
     * @param strength Value between 0.0 and 1.0
     */
    void setEffectStrength(float strength);
    
    /**
     * @brief Set time parameter for animated effects
     * @param time Current time value
     */
    void setTime(float time);
    
    /**
     * @brief Get current effect type
     * @return Current post-processing effect
     */
    PostProcessEffect getEffect() const;

private:
    PostProcessEffect m_currentEffect;
    float m_effectStrength;

    static const char* s_vertexShaderSource;
    static const char* s_fragmentShaderSource;

};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_POST_PROCESS_SHADER_H
