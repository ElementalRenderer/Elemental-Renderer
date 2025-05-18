#pragma once

#include "ShaderEffect.h"
#include "LightingModel.h"
#include <memory>
#include <string>

/**
 * ShaderEffectFactory provides methods to easily create shader effects
 * with different lighting models for common use cases.
 */
class ShaderEffectFactory {
public:
    // Create a standard material effect with a specific lighting model
    static std::shared_ptr<ShaderEffect> createStandardMaterial(
        const std::string& name,
        LightingModelFactory::ModelType lightingModel = LightingModelFactory::ModelType::BLINN_PHONG);
    
    // Create a physically-based material effect
    static std::shared_ptr<ShaderEffect> createPBRMaterial(
        const std::string& name);
    
    // Create a custom effect with a user-defined BRDF
    static std::shared_ptr<ShaderEffect> createCustomBRDFMaterial(
        const std::string& name,
        CustomBRDFModel::BRDFFunction brdfFunction,
        const std::string& description = "Custom BRDF Material");
        
    // Create a specific lighting model
    static std::shared_ptr<LightingModel> createLightingModel(
        LightingModelFactory::ModelType modelType);
        
private:
    // Helper to load and compile shader for standard materials
    static std::shared_ptr<Shader> loadStandardShader();
    
    // Helper to load and compile shader for PBR materials
    static std::shared_ptr<Shader> loadPBRShader();
};
