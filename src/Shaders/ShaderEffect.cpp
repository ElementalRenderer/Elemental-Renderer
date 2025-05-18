#include "../../include/Shaders/ShaderEffect.h"
#include "../../include/Shaders/LightingModelManager.h"
#include <iostream>

ShaderEffect::ShaderEffect(const std::string& name)
    : name(name), enabled(true), currentLightingModelType(LightingModelFactory::ModelType::BLINN_PHONG) {
}

ShaderEffect::~ShaderEffect() {
    cleanup();
}

float ShaderEffect::getFloatParameter(const std::string& name) const {
    auto it = floatParameters.find(name);
    if (it != floatParameters.end()) {
        return it->second;
    }
    std::cerr << "Warning: Float parameter '" << name << "' not found in effect '" << this->name << "'" << std::endl;
    return 0.0f;
}

void ShaderEffect::setFloatParameter(const std::string& name, float value) {
    floatParameters[name] = value;
    
    // if shader is loaded, actually update the uniform
    if (shader) {
        shader->use();
        shader->setFloat(name, value);
    }
}

int ShaderEffect::getIntParameter(const std::string& name) const {
    auto it = intParameters.find(name);
    if (it != intParameters.end()) {
        return it->second;
    }
    std::cerr << "Warning: Int parameter '" << name << "' not found in effect '" << this->name << "'" << std::endl;
    return 0;
}

void ShaderEffect::setIntParameter(const std::string& name, int value) {
    intParameters[name] = value;

    if (shader) {
        shader->use();
        shader->setInt(name, value);
    }
}

void ShaderEffect::setVec2Parameter(const std::string& name, float x, float y) {
    if (shader) {
        shader->use();
        shader->setVec2(name, x, y);
    }
}

void ShaderEffect::setVec3Parameter(const std::string& name, float x, float y, float z) {
    if (shader) {
        shader->use();
        shader->setVec3(name, x, y, z);
    }
}

void ShaderEffect::setVec4Parameter(const std::string& name, float x, float y, float z, float w) {
    if (shader) {
        shader->use();
        shader->setVec4(name, x, y, z, w);
    }
}

void ShaderEffect::setLightingModel(std::shared_ptr<LightingModel> model) {
    if (!model) return;
    
    lightingModel = model;
    
    // Determine the type from the name (not ideal but works for now)
    currentLightingModelType = LightingModelFactory::getModelTypeFromName(model->getName());
    
    // Apply the lighting model to the shader
    applyLightingModel();
}

void ShaderEffect::setLightingModelType(LightingModelFactory::ModelType modelType) {
    // Create a manager to get the model
    static LightingModelManager manager;
    static bool initialized = false;
    
    if (!initialized) {
        manager.initialize();
        initialized = true;
    }
    
    auto model = manager.getModel(modelType);
    if (model) {
        setLightingModel(model);
        currentLightingModelType = modelType;
    }
}

std::string ShaderEffect::getLightingModelName() const {
    return lightingModel ? lightingModel->getName() : "None";
}

void ShaderEffect::applyLightingModel() {
    if (!shader || !lightingModel) return;
    
    // Apply the lighting model to the shader
    lightingModel->applyToShader(shader);
    
    // The shader may need to be recompiled with the new lighting model code
    // This would typically be handled by a more complex shader compilation system
    // For now, we assume the shader already includes all possible lighting models
}

void ShaderEffect::cleanup() {
    // Base cleanup, can be extended by derived classes
    shader.reset();
    lightingModel.reset();
}
