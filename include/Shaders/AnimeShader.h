#pragma once

#include "ShaderEffect.h"

/**
 * AnimeShader implements a cel-shading technique commonly used
 * in anime-style games and renders
 */
class AnimeShader : public ShaderEffect {
public:
    AnimeShader();
    ~AnimeShader() override;
    
    // Initialize shader and parameters
    bool initialize() override;
    
    // Apply the anime shader effect
    void apply() override;
    
    // Additional anime-specific methods
    void setOutlineColor(float r, float g, float b);
    void setOutlineThickness(float thickness);
    void setColorBands(int bands);
    void setSpecularIntensity(float intensity);
    
private:
    // Default parameter values
    static constexpr float DEFAULT_OUTLINE_THICKNESS = 0.02f;
    static constexpr int DEFAULT_COLOR_BANDS = 3;
    static constexpr float DEFAULT_SPECULAR_INTENSITY = 0.7f;
    
    // Helper methods
    void createShaders();
};
