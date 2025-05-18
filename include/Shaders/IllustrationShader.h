#pragma once

#include "ShaderEffect.h"

/**
 * IllustrationShader provides a non-photorealistic rendering style
 * that resembles hand-drawn illustrations with ink and watercolor effects
 */
class IllustrationShader : public ShaderEffect {
public:
    IllustrationShader();
    ~IllustrationShader() override;
    
    // Initialize shader and parameters
    bool initialize() override;
    
    // Apply the illustration shader effect
    void apply() override;
    
    // Illustration-specific methods
    void setInkThickness(float thickness);
    void setInkNoise(float noise);
    void setPaintBlending(float blending);
    void setColorSaturation(float saturation);
    void setEdgeDetectionThreshold(float threshold);
    
private:
    // Default parameter values
    static constexpr float DEFAULT_INK_THICKNESS = 1.0f;
    static constexpr float DEFAULT_INK_NOISE = 0.4f;
    static constexpr float DEFAULT_PAINT_BLENDING = 0.5f;
    static constexpr float DEFAULT_COLOR_SATURATION = 1.2f;
    static constexpr float DEFAULT_EDGE_THRESHOLD = 0.1f;
    
    // Helper methods
    void createShaders();
};
