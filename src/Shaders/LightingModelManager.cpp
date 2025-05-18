#include "../../include/Shaders/LightingModelManager.h"
#include <iostream>

LightingModelManager::LightingModelManager() 
    : currentModelType(LightingModelFactory::ModelType::BLINN_PHONG) {
    // Initialize with a default model type
}

bool LightingModelManager::initialize() {
    // Create all model instances
    createModels();
    
    // Initialize parameter information for UI display
    initializeParameterInfo();
    
    // Set the default model
    return setCurrentModel(currentModelType);
}

std::vector<LightingModelFactory::ModelType> LightingModelManager::getAvailableModels() const {
    return LightingModelFactory::getAvailableModels();
}

std::vector<std::string> LightingModelManager::getAvailableModelNames() const {
    std::vector<std::string> names;
    auto models = LightingModelFactory::getAvailableModels();
    
    for (const auto& model : models) {
        names.push_back(LightingModelFactory::getModelName(model));
    }
    
    return names;
}

std::vector<std::string> LightingModelManager::getAvailableModelDescriptions() const {
    std::vector<std::string> descriptions;
    auto models = LightingModelFactory::getAvailableModels();
    
    for (const auto& model : models) {
        descriptions.push_back(LightingModelFactory::getModelDescription(model));
    }
    
    return descriptions;
}

bool LightingModelManager::setCurrentModel(LightingModelFactory::ModelType modelType) {
    // Get the model instance for this type (either from cache or create new)
    auto model = getModel(modelType);
    
    if (!model) {
        std::cerr << "Failed to set lighting model: " << static_cast<int>(modelType) << std::endl;
        return false;
    }
    
    currentModelType = modelType;
    currentModel = model;
    
    // Update parameter values from the model
    updateCurrentModelParameterValues();
    
    return true;
}

bool LightingModelManager::setCurrentModelByName(const std::string& modelName) {
    auto modelType = LightingModelFactory::getModelTypeFromName(modelName);
    return setCurrentModel(modelType);
}

std::shared_ptr<LightingModel> LightingModelManager::getCurrentModel() const {
    return currentModel;
}

LightingModelFactory::ModelType LightingModelManager::getCurrentModelType() const {
    return currentModelType;
}

std::string LightingModelManager::getCurrentModelName() const {
    return LightingModelFactory::getModelName(currentModelType);
}

void LightingModelManager::applyToShader(std::shared_ptr<Shader> shader) const {
    if (currentModel) {
        currentModel->applyToShader(shader);
    }
}

bool LightingModelManager::setModelParameter(const std::string& paramName, float value) {
    if (!currentModel) {
        return false;
    }
    
    // Set parameter in the actual model
    currentModel->setFloatParameter(paramName, value);
    
    // Update the UI parameter value
    auto& params = modelParameters.at(currentModelType);
    for (auto& param : params) {
        if (param.name == paramName) {
            param.currentValue = value;
            return true;
        }
    }
    
    return false;
}

void LightingModelManager::resetModelParameters() {
    if (!currentModel) {
        return;
    }
    
    // Reset values from parameter info defaults
    auto& params = modelParameters.at(currentModelType);
    for (auto& param : params) {
        currentModel->setFloatParameter(param.name, param.defaultValue);
        param.currentValue = param.defaultValue;
    }
}

std::vector<LightingModelManager::ParameterInfo> LightingModelManager::getCurrentModelParameters() const {
    if (modelParameters.find(currentModelType) != modelParameters.end()) {
        return modelParameters.at(currentModelType);
    }
    return {};
}

std::shared_ptr<LightingModel> LightingModelManager::getModel(LightingModelFactory::ModelType modelType) {
    // First, check if we already have this model in our cache
    auto it = models.find(modelType);
    if (it != models.end()) {
        return it->second;
    }
    
    // Model not found, create a new one
    std::shared_ptr<LightingModel> model;
    
    switch (modelType) {
        case LightingModelFactory::ModelType::OREN_NAYAR:
            model = std::make_shared<OrenNayarModel>();
            break;
            
        case LightingModelFactory::ModelType::COOK_TORRANCE:
            model = std::make_shared<CookTorranceModel>();
            break;
            
        case LightingModelFactory::ModelType::CUSTOM_BRDF:
            model = std::make_shared<CustomBRDFModel>();
            break;
            
        // Implement other model types as they become available
        // For now we only create the ones we've implemented
            
        default:
            // Default to a null model - will be handled by caller
            break;
    }
    
    // Store in cache if created
    if (model) {
        models[modelType] = model;
    }
    
    return model;
}

std::shared_ptr<CustomBRDFModel> LightingModelManager::getCustomBRDFModel() {
    auto model = getModel(LightingModelFactory::ModelType::CUSTOM_BRDF);
    return std::dynamic_pointer_cast<CustomBRDFModel>(model);
}

void LightingModelManager::createModels() {
    // Create model instances for the implemented types
    // This prepopulates our cache
    models[LightingModelFactory::ModelType::OREN_NAYAR] = std::make_shared<OrenNayarModel>();
    models[LightingModelFactory::ModelType::COOK_TORRANCE] = std::make_shared<CookTorranceModel>();
    models[LightingModelFactory::ModelType::CUSTOM_BRDF] = std::make_shared<CustomBRDFModel>();
    
    // Others will be created on-demand or as they're implemented
}

void LightingModelManager::initializeParameterInfo() {
    // Initialize parameter info for each implemented model
    initOrenNayarParameters();
    initCookTorranceParameters();
    initCustomBRDFParameters();
    
    // Additional parameter initializations will be added as models are implemented
    // initPhongParameters();
    // initBlinnPhongParameters();
    // initGGXParameters();
    // initPhysicallyBasedParameters();
}

void LightingModelManager::initOrenNayarParameters() {
    auto& params = modelParameters[LightingModelFactory::ModelType::OREN_NAYAR];
    
    // Get default values from the actual model
    auto model = std::dynamic_pointer_cast<OrenNayarModel>(
        getModel(LightingModelFactory::ModelType::OREN_NAYAR));
    
    if (!model) return;
    
    float roughness = model->getRoughness();
    
    params.push_back(createParameterInfo(
        "roughness", 
        "Roughness", 
        "Controls the surface roughness. Higher values make the surface more matte.",
        0.0f, 1.0f, roughness, roughness
    ));
}

void LightingModelManager::initCookTorranceParameters() {
    auto& params = modelParameters[LightingModelFactory::ModelType::COOK_TORRANCE];
    
    // Get default values from the actual model
    auto model = std::dynamic_pointer_cast<CookTorranceModel>(
        getModel(LightingModelFactory::ModelType::COOK_TORRANCE));
    
    if (!model) return;
    
    float roughness = model->getRoughness();
    float metallic = model->getMetallic();
    float fresnel = model->getFresnel();
    int distribution = model->getDistributionFunction();
    
    params.push_back(createParameterInfo(
        "roughness", 
        "Roughness", 
        "Controls the microsurface roughness affecting specular highlights.",
        0.01f, 1.0f, roughness, roughness
    ));
    
    params.push_back(createParameterInfo(
        "metallic", 
        "Metallic", 
        "Controls how metallic the surface appears (0=dielectric, 1=metal).",
        0.0f, 1.0f, metallic, metallic
    ));
    
    params.push_back(createParameterInfo(
        "fresnel", 
        "Fresnel (F0)", 
        "Base reflectivity at normal incidence. Typically 0.04 for dielectrics, higher for metals.",
        0.0f, 1.0f, fresnel, fresnel
    ));
    
    params.push_back(createParameterInfo(
        "distribution", 
        "Distribution Function", 
        "Microfacet distribution: 0=Beckmann, 1=GGX, 2=Blinn-Phong.",
        0.0f, 2.0f, static_cast<float>(distribution), static_cast<float>(distribution)
    ));
}

void LightingModelManager::initCustomBRDFParameters() {
    auto& params = modelParameters[LightingModelFactory::ModelType::CUSTOM_BRDF];
    
    // Get custom parameters from the custom BRDF model
    auto model = std::dynamic_pointer_cast<CustomBRDFModel>(
        getModel(LightingModelFactory::ModelType::CUSTOM_BRDF));
    
    if (!model) return;
    
    auto modelParams = model->getParameterInfo();
    
    for (const auto& p : modelParams) {
        params.push_back(createParameterInfo(
            p.name, 
            p.name, // Use name as display name
            "Custom parameter", // Generic description
            p.minValue,
            p.maxValue,
            p.defaultValue,
            model->getFloatParameter(p.name)
        ));
    }
}

void LightingModelManager::initPhongParameters() {
    // To be implemented when Phong model is added
}

void LightingModelManager::initBlinnPhongParameters() {
    // To be implemented when Blinn-Phong model is added
}

void LightingModelManager::initGGXParameters() {
    // To be implemented when GGX model is added
}

void LightingModelManager::initPhysicallyBasedParameters() {
    // To be implemented when full PBR model is added
}

LightingModelManager::ParameterInfo LightingModelManager::createParameterInfo(
    const std::string& name,
    const std::string& displayName,
    const std::string& description,
    float minValue,
    float maxValue,
    float defaultValue,
    float currentValue) {
    
    ParameterInfo info;
    info.name = name;
    info.displayName = displayName;
    info.description = description;
    info.minValue = minValue;
    info.maxValue = maxValue;
    info.defaultValue = defaultValue;
    info.currentValue = (currentValue != 0.0f) ? currentValue : defaultValue;
    
    return info;
}

void LightingModelManager::updateCurrentModelParameterValues() {
    if (!currentModel || modelParameters.find(currentModelType) == modelParameters.end()) {
        return;
    }
    
    auto& params = modelParameters.at(currentModelType);
    
    for (auto& param : params) {
        param.currentValue = currentModel->getFloatParameter(param.name);
    }
}
