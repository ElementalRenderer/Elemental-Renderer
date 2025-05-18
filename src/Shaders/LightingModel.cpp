#include "../../include/Shaders/LightingModel.h"
#include <iostream>

LightingModel::LightingModel(const std::string& name) : name(name) {
}

float LightingModel::getFloatParameter(const std::string& name) const {
    auto it = parameters.find(name);
    if (it != parameters.end()) {
        return it->second;
    }
    std::cerr << "Warning: Parameter '" << name << "' not found in lighting model '" 
              << this->name << "'" << std::endl;
    return 0.0f;
}

void LightingModel::setFloatParameter(const std::string& name, float value) {
    parameters[name] = value;
}

void LightingModel::updateShaderParameters(std::shared_ptr<Shader> shader) {
    if (!shader) return;
    
    shader->use();
    
    // Update all parameters in the shader
    for (const auto& param : parameters) {
        std::string uniformName = "lighting." + param.first;
        shader->setFloat(uniformName, param.second);
    }
}

// --- LightingModelFactory Implementation ---

std::shared_ptr<LightingModel> LightingModelFactory::create(ModelType type) {
    // This will be implemented in specific model files
    // We'll forward declare the creation functions here
    
    // For now, let's create a simple placeholder that will be completed later
    // when we implement specific models
    switch (type) {
        case ModelType::PHONG:
            // return std::make_shared<PhongLightingModel>();
            break;
        case ModelType::BLINN_PHONG:
            // return std::make_shared<BlinnPhongLightingModel>();
            break;
        case ModelType::OREN_NAYAR:
            // return std::make_shared<OrenNayarLightingModel>();
            break;
        case ModelType::COOK_TORRANCE:
            // return std::make_shared<CookTorranceLightingModel>();
            break;
        case ModelType::GGX:
            // return std::make_shared<GGXLightingModel>();
            break;
        case ModelType::CUSTOM_BRDF:
            // return std::make_shared<CustomBRDFLightingModel>();
            break;
        case ModelType::PHYSICALLY_BASED:
            // return std::make_shared<PhysicallyBasedLightingModel>();
            break;
    }
    
    // Default to Blinn-Phong for now
    std::cerr << "Warning: Requested lighting model not yet implemented. Using placeholder." << std::endl;
    return nullptr;
}

std::vector<LightingModelFactory::ModelType> LightingModelFactory::getAvailableModels() {
    return {
        ModelType::PHONG,
        ModelType::BLINN_PHONG,
        ModelType::OREN_NAYAR,
        ModelType::COOK_TORRANCE,
        ModelType::GGX,
        ModelType::CUSTOM_BRDF,
        ModelType::PHYSICALLY_BASED
    };
}

std::string LightingModelFactory::getModelName(ModelType type) {
    switch (type) {
        case ModelType::PHONG:
            return "Phong";
        case ModelType::BLINN_PHONG:
            return "Blinn-Phong";
        case ModelType::OREN_NAYAR:
            return "Oren-Nayar";
        case ModelType::COOK_TORRANCE:
            return "Cook-Torrance";
        case ModelType::GGX:
            return "GGX/Trowbridge-Reitz";
        case ModelType::CUSTOM_BRDF:
            return "Custom BRDF";
        case ModelType::PHYSICALLY_BASED:
            return "Physically Based";
        default:
            return "Unknown";
    }
}

std::string LightingModelFactory::getModelDescription(ModelType type) {
    switch (type) {
        case ModelType::PHONG:
            return "Classic Phong lighting model with ambient, diffuse, and specular components.";
        case ModelType::BLINN_PHONG:
            return "Improved Blinn-Phong model that uses a halfway vector for more realistic specular highlights.";
        case ModelType::OREN_NAYAR:
            return "Diffuse reflection model that accounts for microfacet roughness, ideal for matte surfaces like clay or fabric.";
        case ModelType::COOK_TORRANCE:
            return "Microfacet-based specular reflection model for rough surfaces with physically-based distribution.";
        case ModelType::GGX:
            return "GGX/Trowbridge-Reitz model provides more realistic specular highlights with longer tails.";
        case ModelType::CUSTOM_BRDF:
            return "User-defined custom BRDF implementation for specialized material rendering.";
        case ModelType::PHYSICALLY_BASED:
            return "Complete physically based rendering (PBR) model that accurately simulates material properties.";
        default:
            return "Unknown lighting model.";
    }
}

LightingModelFactory::ModelType LightingModelFactory::getModelTypeFromName(const std::string& name) {
    // Simple string matching, could be improved with a map
    if (name == "Phong") return ModelType::PHONG;
    if (name == "Blinn-Phong") return ModelType::BLINN_PHONG;
    if (name == "Oren-Nayar") return ModelType::OREN_NAYAR;
    if (name == "Cook-Torrance") return ModelType::COOK_TORRANCE;
    if (name == "GGX/Trowbridge-Reitz") return ModelType::GGX;
    if (name == "Custom BRDF") return ModelType::CUSTOM_BRDF;
    if (name == "Physically Based") return ModelType::PHYSICALLY_BASED;
    
    // Default
    return ModelType::BLINN_PHONG;
}

int LightingModelFactory::getModelCount() {
    return 7; // Number of models in the enum
}
