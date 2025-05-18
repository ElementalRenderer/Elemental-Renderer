#include "../../include/Shaders/OrenNayarModel.h"

OrenNayarModel::OrenNayarModel() : LightingModel("Oren-Nayar") {
    // Initialize default parameters
    parameters["roughness"] = DEFAULT_ROUGHNESS;
}

std::string OrenNayarModel::getDescription() const {
    return "Oren-Nayar diffuse reflection model accounts for microfacet roughness, "
           "providing realistic shading for matte surfaces like clay, fabric, and rough materials.";
}

void OrenNayarModel::applyToShader(std::shared_ptr<Shader> shader) {
    if (!shader) return;
    
    // Update all parameters
    updateShaderParameters(shader);
    
    // Set other shader-specific configurations if needed
    shader->setInt("lightingModel", 2); // ID for Oren-Nayar model
}

std::string OrenNayarModel::getShaderCode() const {
    // GLSL implementation of Oren-Nayar model
    return R"(
        // Oren-Nayar diffuse reflection model
        vec3 calculateOrenNayar(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 albedo, float roughness) {
            // Calculate angles
            float NdotL = max(dot(normal, lightDir), 0.0);
            float NdotV = max(dot(normal, viewDir), 0.0);
            
            // Early exit if surface is facing away from light
            if (NdotL < 0.001) return vec3(0.0);
            
            // Compute the cosine of the angle between the view and light directions
            float angleVL = max(0.0, dot(viewDir, lightDir));
            
            // Compute the angles from normal for view and light directions
            float thetaI = acos(NdotL);
            float thetaR = acos(NdotV);
            
            // Compute the roughness parameters
            float sigma2 = roughness * roughness;
            float A = 1.0 - (0.5 * sigma2 / (sigma2 + 0.33));
            float B = 0.45 * sigma2 / (sigma2 + 0.09);
            
            // Compute alpha and beta (angles)
            float alpha = max(thetaI, thetaR);
            float beta = min(thetaI, thetaR);
            
            // Calculate the Oren-Nayar diffuse term
            float C = sin(alpha) * tan(beta);
            
            // Final Oren-Nayar diffuse term
            float diffuse = NdotL * (A + B * max(0.0, angleVL) * C);
            
            return albedo * diffuse;
        }
    )";
}

void OrenNayarModel::setRoughness(float roughness) {
    // Clamp roughness to valid range [0,1]
    roughness = std::max(0.0f, std::min(1.0f, roughness));
    setFloatParameter("roughness", roughness);
}

float OrenNayarModel::getRoughness() const {
    return getFloatParameter("roughness");
}
