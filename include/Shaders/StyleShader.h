#pragma once

#include "../Shader.h"
#include <string>
#include <memory>
#include <unordered_map>

/**
 * StyleShader class provides pre-defined artistic shaders and effects
 * such as anime-style, pixel-art, and illustration-style rendering
 */
class StyleShader {
public:
    // Available shader styles
    enum class Style {
        ANIME,          // Cell-shaded anime style with outlines
        PIXEL_ART,      // Pixelated retro-style effect
        ILLUSTRATION,   // Painterly illustration style
        TOON,           // Classic toon shader
        WATERCOLOR,     // Soft watercolor effect
        SKETCH,         // Hand-drawn sketch effect
        DEFAULT         // Standard physically-based rendering
    };
    
    StyleShader();
    ~StyleShader();
    
    // Initialize all style shaders
    bool initialize();
    
    // Apply a specific style to the current rendering pipeline
    bool applyStyle(Style style);
    
    // Get the shader program for a specific style
    std::shared_ptr<Shader> getShader(Style style);
    
    // Get a human-readable name for a style
    static std::string getStyleName(Style style);
    
    // Get a description of a style
    static std::string getStyleDescription(Style style);
    
    // Get total number of available styles
    static int getStyleCount();
    
    // Get style enum from index
    static Style getStyleFromIndex(int index);
    
private:
    // Storage for shader programs
    std::unordered_map<Style, std::shared_ptr<Shader>> shaders;
    
    // Currently active style
    Style currentStyle;
    
    // Helper methods to initialize specific shaders
    void initAnimeShader();
    void initPixelArtShader();
    void initIllustrationShader();
    void initToonShader();
    void initWatercolorShader();
    void initSketchShader();
    
    // Shader parameter management
    void setDefaultParameters(Style style);
};
