#include "../../include/Shaders/CustomBRDFModel.h"

CustomBRDFModel::CustomBRDFModel() : LightingModel("Custom BRDF") {
    // Default BRDF is just a simple Lambertian diffuse
    customBRDFFunction = []() -> std::string {
        return R"(
            // Default custom BRDF (simple Lambertian diffuse)
            vec3 calculateCustomBRDF(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 albedo, 
                                    float param1, float param2, float param3, float param4) {
                float NdotL = max(dot(normal, lightDir), 0.0);
                return albedo * NdotL / 3.14159265359;
            }
        )";
    };
    
    customDescription = "Custom BRDF implementation (currently using default Lambertian diffuse)";
    
    // Add some default parameters
    addParameter("param1", 0.5f, 0.0f, 1.0f);
    addParameter("param2", 0.5f, 0.0f, 1.0f);
    addParameter("param3", 0.5f, 0.0f, 1.0f);
    addParameter("param4", 0.5f, 0.0f, 1.0f);
}

std::string CustomBRDFModel::getDescription() const {
    return customDescription;
}

void CustomBRDFModel::applyToShader(std::shared_ptr<Shader> shader) {
    if (!shader) return;
    
    // Update all parameters
    updateShaderParameters(shader);
    
    // Set other shader-specific configurations
    shader->setInt("lightingModel", 6); // ID for Custom BRDF model
}

std::string CustomBRDFModel::getShaderCode() const {
    // Return the custom BRDF code
    return customBRDFFunction ? customBRDFFunction() : "";
}

void CustomBRDFModel::setCustomBRDF(const BRDFFunction& brdfFunction, const std::string& description) {
    customBRDFFunction = brdfFunction;
    
    if (!description.empty()) {
        customDescription = description;
    }
}

void CustomBRDFModel::addParameter(const std::string& name, float defaultValue, float min, float max) {
    // Add the parameter to the parameters map
    parameters[name] = defaultValue;
    
    // Add parameter info for UI display and validation
    ParameterInfo info;
    info.name = name;
    info.defaultValue = defaultValue;
    info.minValue = min;
    info.maxValue = max;
    
    parameterInfo.push_back(info);
}

std::vector<CustomBRDFModel::ParameterInfo> CustomBRDFModel::getParameterInfo() const {
    return parameterInfo;
}
