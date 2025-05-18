#pragma once

#include "LightingModel.h"
#include <functional>

/**
 * CustomBRDFModel allows developers to implement and inject their own custom BRDFs
 * without modifying the core rendering engine.
 */
class CustomBRDFModel : public LightingModel {
public:
    // Type definition for custom BRDF computation functions
    using BRDFFunction = std::function<std::string()>;
    
    CustomBRDFModel();
    ~CustomBRDFModel() override = default;
    
    // Get model description
    std::string getDescription() const override;
    
    // Apply model to shader
    void applyToShader(std::shared_ptr<Shader> shader) override;
    
    // Get shader code fragment
    std::string getShaderCode() const override;
    
    // Set custom BRDF function
    void setCustomBRDF(const BRDFFunction& brdfFunction, const std::string& description = "");
    
    // Add custom parameter
    void addParameter(const std::string& name, float defaultValue, float min, float max);
    
    // Get custom parameter information
    struct ParameterInfo {
        std::string name;
        float defaultValue;
        float minValue;
        float maxValue;
    };
    
    // Get parameter information
    std::vector<ParameterInfo> getParameterInfo() const;
    
private:
    BRDFFunction customBRDFFunction;
    std::string customDescription;
    std::vector<ParameterInfo> parameterInfo;
};
