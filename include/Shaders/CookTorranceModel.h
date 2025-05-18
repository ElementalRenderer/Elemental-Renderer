#pragma once

#include "LightingModel.h"

/**
 * CookTorranceModel implements the Cook-Torrance microfacet specular reflection model,
 * which is physically based and provides realistic rendering for a wide range of materials.
 */
class CookTorranceModel : public LightingModel {
public:
    CookTorranceModel();
    ~CookTorranceModel() override = default;
    
    // Get model description
    std::string getDescription() const override;
    
    // Apply model to shader
    void applyToShader(std::shared_ptr<Shader> shader) override;
    
    // Get shader code fragment
    std::string getShaderCode() const override;
    
    // Set roughness parameter
    void setRoughness(float roughness);
    
    // Set metallic parameter
    void setMetallic(float metallic);
    
    // Set fresnel (F0) parameter
    void setFresnel(float fresnel);
    
    // Set distribution function type (0=Beckmann, 1=GGX, 2=Blinn)
    void setDistributionFunction(int type);
    
    // Get current parameters
    float getRoughness() const;
    float getMetallic() const;
    float getFresnel() const;
    int getDistributionFunction() const;
    
private:
    // Default parameters
    static constexpr float DEFAULT_ROUGHNESS = 0.3f;
    static constexpr float DEFAULT_METALLIC = 0.0f;
    static constexpr float DEFAULT_FRESNEL = 0.04f; // Typical value for dielectrics
    static constexpr int DEFAULT_DISTRIBUTION = 1;  // GGX by default
};
