#pragma once

#include <string>
#include <memory>
#include <vector>
#include "../Shader.h"

/**
 * LightingModel base class defines the interface for different BRDF implementations
 * that can be swapped at runtime to change how lighting is calculated
 */
class LightingModel {
public:
    LightingModel(const std::string& name);
    virtual ~LightingModel() = default;
    
    // Get the model name
    const std::string& getName() const { return name; }
    
    // Get the model description
    virtual std::string getDescription() const = 0;
    
    // Apply the lighting model to a shader
    virtual void applyToShader(std::shared_ptr<Shader> shader) = 0;
    
    // Get shader code fragment for this lighting model
    // This will be inserted into shader templates
    virtual std::string getShaderCode() const = 0;
    
    // Get and set parameters
    float getFloatParameter(const std::string& name) const;
    void setFloatParameter(const std::string& name, float value);
    
    // Helper method to set shader parameters
    void updateShaderParameters(std::shared_ptr<Shader> shader);
    
protected:
    std::string name;
    std::unordered_map<std::string, float> parameters;
};

/**
 * LightingModelFactory provides a central registry for creating and accessing lighting models
 */
class LightingModelFactory {
public:
    // Available lighting model types
    enum class ModelType {
        PHONG,              // Classic Phong model
        BLINN_PHONG,        // Improved Blinn-Phong model
        OREN_NAYAR,         // Oren-Nayar diffuse model for rough surfaces
        COOK_TORRANCE,      // Cook-Torrance microfacet specular model
        GGX,                // GGX/Trowbridge-Reitz microfacet model
        CUSTOM_BRDF,        // User-defined custom BRDF
        PHYSICALLY_BASED    // Full PBR with multiple components
    };
    
    // get a lighting model by type
    static std::shared_ptr<LightingModel> create(ModelType type);
    
    // Get all available model types
    static std::vector<ModelType> getAvailableModels();
    
    // Get a human-readable name for a model type
    static std::string getModelName(ModelType type);
    
    // Get a description for a model type
    static std::string getModelDescription(ModelType type);
    
    // Get model type from string name
    static ModelType getModelTypeFromName(const std::string& name);
    
    // Get total model count
    static int getModelCount();
};
