#pragma once

#include "StyleShader.h"
#include <memory>
#include <vector>
#include <string>

/**
 * StyleShaderManager provides a centralized way to manage and apply
 * different artistic shader styles to the renderer
 */
class StyleShaderManager {
public:
    StyleShaderManager();
    ~StyleShaderManager();
    
    // Initialize the shader manager
    bool initialize();
    
    // Apply a style by name
    bool applyStyleByName(const std::string& styleName);
    
    // Apply a style by index
    bool applyStyleByIndex(int index);
    
    // Apply a style directly
    bool applyStyle(StyleShader::Style style);
    
    // Get the currently active style
    StyleShader::Style getCurrentStyle() const;
    
    // Get the name of the currently active style
    std::string getCurrentStyleName() const;
    
    // Get a list of all available style names
    std::vector<std::string> getAvailableStyleNames() const;
    
    // Get a list of all available style descriptions
    std::vector<std::string> getAvailableStyleDescriptions() const;
    
    // Get style parameters that can be adjusted by users
    struct StyleParameter {
        std::string name;
        std::string displayName;
        std::string description;
        float minValue;
        float maxValue;
        float defaultValue;
        float currentValue;
        enum Type { FLOAT, INT, BOOL } type;
    };
    
    // Get parameters for the current style
    std::vector<StyleParameter> getCurrentStyleParameters() const;
    
    // Set a parameter value for the current style
    bool setStyleParameter(const std::string& paramName, float value);
    
    // Reset all parameters for the current style to defaults
    void resetStyleParameters();
    
    // Get the shader program for the current style
    std::shared_ptr<Shader> getCurrentShader() const;
    
private:
    // The underlying style shader
    std::unique_ptr<StyleShader> styleShader;
    
    // Currently active style
    StyleShader::Style currentStyle;
    
    // Map of style parameters for each style
    std::unordered_map<StyleShader::Style, std::vector<StyleParameter>> styleParameters;
    
    // Initialize parameters for each style
    void initializeStyleParameters();
    
    // Helper to initialize parameters for a specific style
    void initAnimeShaderParameters();
    void initPixelArtParameters();
    void initIllustrationParameters();
    void initToonParameters();
    void initWatercolorParameters();
    void initSketchParameters();
};
