#pragma once

#include "LightingModel.h"

/**
 * OrenNayarModel implements the Oren-Nayar diffuse reflection model which
 * accounts for microfacet roughness, making it ideal for materials like
 * clay, paper, fabric, and other matte surfaces.
 */
class OrenNayarModel : public LightingModel {
public:
    OrenNayarModel();
    ~OrenNayarModel() override = default;
    
    // Get model description
    std::string getDescription() const override;
    
    // Apply model to shader
    void applyToShader(std::shared_ptr<Shader> shader) override;
    
    // Get shader code fragment
    std::string getShaderCode() const override;
    
    // Set roughness parameter (sigma)
    void setRoughness(float roughness);
    
    // Get roughness parameter
    float getRoughness() const;
    
private:
    // Default parameters
    static constexpr float DEFAULT_ROUGHNESS = 0.3f;
};
