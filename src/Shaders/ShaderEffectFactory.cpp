#include "../../include/Shaders/ShaderEffectFactory.h"
#include "../../include/Shaders/LightingModelManager.h"
#include "../../include/Shader.h"
#include <iostream>

std::shared_ptr<ShaderEffect> ShaderEffectFactory::createStandardMaterial(
    const std::string& name,
    LightingModelFactory::ModelType lightingModel) {
    
    // Create shader effect
    auto effect = std::make_shared<ShaderEffect>(name);
    
    // Load standard shader
    effect->shader = loadStandardShader();
    
    if (!effect->shader) {
        std::cerr << "Failed to load standard shader for material: " << name << std::endl;
        return nullptr;
    }
    
    // Set default material parameters
    effect->setVec3Parameter("albedo", 0.8f, 0.8f, 0.8f);
    effect->setFloatParameter("roughness", 0.5f);
    effect->setFloatParameter("metallic", 0.0f);
    effect->setFloatParameter("ao", 1.0f);
    effect->setFloatParameter("emissive", 0.0f);
    
    // Set the lighting model
    effect->setLightingModelType(lightingModel);
    
    return effect;
}

std::shared_ptr<ShaderEffect> ShaderEffectFactory::createPBRMaterial(
    const std::string& name) {
    
    // PBR materials use the physically based lighting model
    return createStandardMaterial(name, LightingModelFactory::ModelType::PHYSICALLY_BASED);
}

std::shared_ptr<ShaderEffect> ShaderEffectFactory::createCustomBRDFMaterial(
    const std::string& name,
    CustomBRDFModel::BRDFFunction brdfFunction,
    const std::string& description) {
    
    // Create standard material with CUSTOM_BRDF lighting model type
    auto effect = createStandardMaterial(name, LightingModelFactory::ModelType::CUSTOM_BRDF);
    
    if (!effect) {
        return nullptr;
    }
    
    // Get custom BRDF model and set the custom function
    auto customBRDF = std::dynamic_pointer_cast<CustomBRDFModel>(effect->getLightingModel());
    
    if (customBRDF) {
        customBRDF->setCustomBRDF(brdfFunction, description);
        
        // Re-apply to shader (since the code has changed)
        effect->applyLightingModel();
    }
    
    return effect;
}

std::shared_ptr<LightingModel> ShaderEffectFactory::createLightingModel(
    LightingModelFactory::ModelType modelType) {
    
    // Create lighting model manager to get models
    static LightingModelManager manager;
    static bool initialized = false;
    
    if (!initialized) {
        manager.initialize();
        initialized = true;
    }
    
    return manager.getModel(modelType);
}

std::shared_ptr<Shader> ShaderEffectFactory::loadStandardShader() {
    // In a real implementation, this would compile the shader from source
    // For this example, we'll just create an empty shader
    // (Actual implementation would load vertex and fragment shaders)
    
    // Placeholder for now - would normally load from file or embedded resource
    return std::make_shared<Shader>();
}

std::shared_ptr<Shader> ShaderEffectFactory::loadPBRShader() {
    // For now, PBR uses the same shader as standard materials
    // In a full implementation, this might use a more specialized shader
    return loadStandardShader();
}
