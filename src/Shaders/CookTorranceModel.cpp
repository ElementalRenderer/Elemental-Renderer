#include "../../include/Shaders/CookTorranceModel.h"

CookTorranceModel::CookTorranceModel() : LightingModel("Cook-Torrance") {
    // Initialize default parameters
    parameters["roughness"] = DEFAULT_ROUGHNESS;
    parameters["metallic"] = DEFAULT_METALLIC;
    parameters["fresnel"] = DEFAULT_FRESNEL;
    parameters["distribution"] = static_cast<float>(DEFAULT_DISTRIBUTION);
}

std::string CookTorranceModel::getDescription() const {
    return "Cook-Torrance microfacet specular reflection model is physically based "
           "and suitable for rendering metals, plastics, and other complex materials with "
           "accurate specular highlights.";
}

void CookTorranceModel::applyToShader(std::shared_ptr<Shader> shader) {
    if (!shader) return;
    
    // Update all parameters
    updateShaderParameters(shader);
    
    // Set other shader-specific configurations
    shader->setInt("lightingModel", 3); // ID for Cook-Torrance model
}

std::string CookTorranceModel::getShaderCode() const {
    // GLSL implementation of Cook-Torrance model
    return R"(
        // Cook-Torrance specular reflection model
        vec3 calculateCookTorrance(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 albedo, 
                                  float roughness, float metallic, float F0, int distribution) {
            // Halfway vector
            vec3 halfwayDir = normalize(lightDir + viewDir);
            
            // Basic dot products we need
            float NdotL = max(dot(normal, lightDir), 0.0);
            float NdotV = max(dot(normal, viewDir), 0.0);
            float NdotH = max(dot(normal, halfwayDir), 0.0);
            float VdotH = max(dot(viewDir, halfwayDir), 0.0);
            
            // Early exit if surface is facing away from light
            if (NdotL < 0.001) return vec3(0.0);
            
            // Roughness squared (Disney-style remapping)
            float alpha = roughness * roughness;
            
            // ------- Calculate Distribution (D) -------
            float D;
            if (distribution == 0) {
                // Beckmann distribution
                float alpha2 = alpha * alpha;
                float NdotH2 = NdotH * NdotH;
                float exponent = (NdotH2 - 1.0) / (alpha2 * NdotH2);
                D = exp(exponent) / (3.14159265359 * alpha2 * NdotH2 * NdotH2);
            }
            else if (distribution == 1) {
                // GGX/Trowbridge-Reitz distribution
                float alpha2 = alpha * alpha;
                float denom = NdotH * NdotH * (alpha2 - 1.0) + 1.0;
                D = alpha2 / (3.14159265359 * denom * denom);
            }
            else {
                // Blinn-Phong approximation
                float normalization = (2.0 + 2.0/alpha) / (2.0 * 3.14159265359);
                float power = 2.0 / (alpha * alpha) - 2.0;
                D = normalization * pow(NdotH, power);
            }
            
            // ------- Calculate Geometric Attenuation (G) -------
            // Smith's method
            float G1_v = 2.0 * NdotV / (NdotV + sqrt(alpha + (1.0 - alpha) * NdotV * NdotV));
            float G1_l = 2.0 * NdotL / (NdotL + sqrt(alpha + (1.0 - alpha) * NdotL * NdotL));
            float G = G1_v * G1_l;
            
            // ------- Calculate Fresnel Term (F) -------
            // Schlick's approximation
            float F = F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
            
            // ------- Combine for Cook-Torrance Specular Term -------
            float specular = (D * G * F) / (4.0 * NdotV * NdotL);
            
            // Calculate diffuse component (reduced for metallic surfaces)
            float diffuse = (1.0 - F) * (1.0 - metallic) * NdotL / 3.14159265359;
            
            // Combine diffuse and specular
            return albedo * (diffuse + specular * NdotL);
        }
    )";
}

void CookTorranceModel::setRoughness(float roughness) {
    // Clamp roughness to valid range [0.01,1]
    roughness = std::max(0.01f, std::min(1.0f, roughness));
    setFloatParameter("roughness", roughness);
}

void CookTorranceModel::setMetallic(float metallic) {
    // Clamp metallic to valid range [0,1]
    metallic = std::max(0.0f, std::min(1.0f, metallic));
    setFloatParameter("metallic", metallic);
}

void CookTorranceModel::setFresnel(float fresnel) {
    // Clamp fresnel to valid range [0,1]
    fresnel = std::max(0.0f, std::min(1.0f, fresnel));
    setFloatParameter("fresnel", fresnel);
}

void CookTorranceModel::setDistributionFunction(int type) {
    // Clamp type to valid range [0,2]
    type = std::max(0, std::min(2, type));
    setFloatParameter("distribution", static_cast<float>(type));
}

float CookTorranceModel::getRoughness() const {
    return getFloatParameter("roughness");
}

float CookTorranceModel::getMetallic() const {
    return getFloatParameter("metallic");
}

float CookTorranceModel::getFresnel() const {
    return getFloatParameter("fresnel");
}

int CookTorranceModel::getDistributionFunction() const {
    return static_cast<int>(getFloatParameter("distribution"));
}
