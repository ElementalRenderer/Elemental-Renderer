#pragma once
#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "../Shader.h"
#include "LightingModel.h"

/**
 * ShaderEffect encapsulates a shader with its parameters and associated lighting model
 */
class ShaderEffect {
public:
    ShaderEffect(const std::string& name);
    virtual ~ShaderEffect();
    
    const std::string& getName() const { return name; }
    bool isEnabled() const { return enabled; }
    void setEnabled(bool enable) { enabled = enable; }
    
    // Parameter getters and setters
    float getFloatParameter(const std::string& name) const;
    void setFloatParameter(const std::string& name, float value);
    
    int getIntParameter(const std::string& name) const;
    void setIntParameter(const std::string& name, int value);
    
    void setVec2Parameter(const std::string& name, float x, float y);
    void setVec3Parameter(const std::string& name, float x, float y, float z);
    void setVec4Parameter(const std::string& name, float x, float y, float z, float w);
    
    // Lighting model management
    void setLightingModel(std::shared_ptr<LightingModel> model);
    std::shared_ptr<LightingModel> getLightingModel() const { return lightingModel; }
    
    // Set lighting model by type
    void setLightingModelType(LightingModelFactory::ModelType modelType);
    
    // Get current lighting model type
    LightingModelFactory::ModelType getLightingModelType() const { return currentLightingModelType; }
    
    // Get lighting model name
    std::string getLightingModelName() const;
    
    // Apply lighting model parameters to shader
    void applyLightingModel();
    
    // Resource cleanup
    virtual void cleanup();
    
protected:
    std::string name;
    bool enabled;
    
    std::shared_ptr<Shader> shader;
    
    std::unordered_map<std::string, float> floatParameters;
    std::unordered_map<std::string, int> intParameters;
    
    // Lighting model support
    std::shared_ptr<LightingModel> lightingModel;
    LightingModelFactory::ModelType currentLightingModelType;
};
#include <string>
#include <memory>
#include <unordered_map>
#include "../Shader.h"

/**
 * ShaderEffect class serves as a base class for all stylized rendering effects
 * This can be used for both material shaders and post-processing effects
 */
class ShaderEffect {
public:
    ShaderEffect(const std::string& name);
    virtual ~ShaderEffect();

    // Getters
    const std::string& getName() const { return name; }
    std::shared_ptr<Shader> getShader() const { return shader; }
    
    // Check if effect is enabled
    bool isEnabled() const { return enabled; }
    
    // Enable/disable effect
    void setEnabled(bool value) { enabled = value; }
    
    // Get and set parameters
    float getFloatParameter(const std::string& name) const;
    void setFloatParameter(const std::string& name, float value);
    
    int getIntParameter(const std::string& name) const;
    void setIntParameter(const std::string& name, int value);
    
    void setVec2Parameter(const std::string& name, float x, float y);
    void setVec3Parameter(const std::string& name, float x, float y, float z);
    void setVec4Parameter(const std::string& name, float x, float y, float z, float w);
    
    // Apply the effect (to be implemented by derived classes)
    virtual void apply() = 0;
    
    // Initialize effect-specific resources
    virtual bool initialize() = 0;
    
    // Clean up resources
    virtual void cleanup();
    
protected:
    std::string name;
    bool enabled;
    std::shared_ptr<Shader> shader;
    
    // Parameters that can be adjusted by users
    std::unordered_map<std::string, float> floatParameters;
    std::unordered_map<std::string, int> intParameters;
};
