#pragma once

#include "LightingModel.h"
#include "OrenNayarModel.h"
#include "CookTorranceModel.h"
#include "CustomBRDFModel.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

/**
 * LightingModelManager handles the creation, selection and management of lighting models
 * throughout the rendering pipeline. It serves as the central point for lighting model operations.
 */
class LightingModelManager {
public:
    LightingModelManager();
    ~LightingModelManager() = default;
    
    // Initialize the manager
    bool initialize();
    
    // Get available lighting model types
    std::vector<LightingModelFactory::ModelType> getAvailableModels() const;
    
    // Get names of available lighting models
    std::vector<std::string> getAvailableModelNames() const;
    
    // Get descriptions of available lighting models
    std::vector<std::string> getAvailableModelDescriptions() const;
    
    // Set current lighting model by type
    bool setCurrentModel(LightingModelFactory::ModelType modelType);
    
    // Set current lighting model by name
    bool setCurrentModelByName(const std::string& modelName);
    
    // Get current lighting model
    std::shared_ptr<LightingModel> getCurrentModel() const;
    
    // Get current lighting model type
    LightingModelFactory::ModelType getCurrentModelType() const;
    
    // Get current lighting model name
    std::string getCurrentModelName() const;
    
    // Apply current lighting model to a shader
    void applyToShader(std::shared_ptr<Shader> shader) const;
    
    // Set a parameter for the current lighting model
    bool setModelParameter(const std::string& paramName, float value);
    
    // Reset parameters for the current lighting model
    void resetModelParameters();
    
    // Parameter information for UI display
    struct ParameterInfo {
        std::string name;
        std::string displayName;
        std::string description;
        float minValue;
        float maxValue;
        float defaultValue;
        float currentValue;
    };
    
    // Get parameters for the current lighting model
    std::vector<ParameterInfo> getCurrentModelParameters() const;
    
    // Get specific lighting model by type (creates if doesn't exist)
    std::shared_ptr<LightingModel> getModel(LightingModelFactory::ModelType modelType);
    
    // Access to the custom BRDF model for direct manipulation
    std::shared_ptr<CustomBRDFModel> getCustomBRDFModel();
    
private:
    // Currently active lighting model
    LightingModelFactory::ModelType currentModelType;
    std::shared_ptr<LightingModel> currentModel;
    
    // Cache of created lighting models
    std::unordered_map<LightingModelFactory::ModelType, std::shared_ptr<LightingModel>> models;
    
    // Create model instances for each type
    void createModels();
    
    // Initialize parameter information for each model type
    void initializeParameterInfo();
    
    // Parameter info storage
    std::unordered_map<LightingModelFactory::ModelType, std::vector<ParameterInfo>> modelParameters;
    
    // Helper methods to initialize parameters for various models
    void initOrenNayarParameters();
    void initCookTorranceParameters();
    void initCustomBRDFParameters();
    void initPhongParameters();
    void initBlinnPhongParameters();
    void initGGXParameters();
    void initPhysicallyBasedParameters();
    
    // Helper to create a parameter info object
    ParameterInfo createParameterInfo(
        const std::string& name,
        const std::string& displayName,
        const std::string& description,
        float minValue,
        float maxValue,
        float defaultValue,
        float currentValue = 0.0f);
    
    // Update the current model parameters based on values in the model
    void updateCurrentModelParameterValues();
};
