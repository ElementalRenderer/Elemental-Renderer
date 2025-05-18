#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 albedo;
uniform float roughness;
uniform float metallic;
uniform float ao;
uniform float emissive;

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform vec3 viewPos;

uniform int lightingModel;

struct LightingParams {
    float roughness;
    float metallic;
    float fresnel;
    float distribution;
    float param1;
    float param2;
    float param3;
    float param4;
};
uniform LightingParams lighting;

const float PI = 3.14159265359;

vec3 calculatePhong(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 albedo, float specularPower);
vec3 calculateBlinnPhong(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 albedo, float specularPower);
vec3 calculateOrenNayar(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 albedo, float roughness);
vec3 calculateCookTorrance(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 albedo, 
                          float roughness, float metallic, float F0, int distribution);
vec3 calculateGGX(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 albedo, 
                 float roughness, float metallic, float F0);
vec3 calculateCustomBRDF(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 albedo, 
                        float param1, float param2, float param3, float param4);
vec3 calculatePBR(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 albedo, 
                 float roughness, float metallic, float ao);

vec3 calculateOrenNayar(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 albedo, float roughness) {
    // Calculate angles
    float NdotL = max(dot(normal, lightDir), 0.0);
    float NdotV = max(dot(normal, viewDir), 0.0);

    if (NdotL < 0.001) return vec3(0.0);

    float angleVL = max(0.0, dot(viewDir, lightDir));

    float thetaI = acos(NdotL);
    float thetaR = acos(NdotV);
    float sigma2 = roughness * roughness;
    float A = 1.0 - (0.5 * sigma2 / (sigma2 + 0.33));
    float B = 0.45 * sigma2 / (sigma2 + 0.09);
    float alpha = max(thetaI, thetaR);
    float beta = min(thetaI, thetaR);
    float C = sin(alpha) * tan(beta);
    float diffuse = NdotL * (A + B * max(0.0, angleVL) * C);
    
    return albedo * diffuse;
}

vec3 calculateCookTorrance(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 albedo, 
                          float roughness, float metallic, float F0, int distribution) {

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float NdotV = max(dot(normal, viewDir), 0.0);
    float NdotH = max(dot(normal, halfwayDir), 0.0);
    float VdotH = max(dot(viewDir, halfwayDir), 0.0);
    if (NdotL < 0.001) return vec3(0.0);

    float alpha = roughness * roughness;

    float D;
    if (distribution == 0) {
        float alpha2 = alpha * alpha;
        float NdotH2 = NdotH * NdotH;
        float exponent = (NdotH2 - 1.0) / (alpha2 * NdotH2);
        D = exp(exponent) / (3.14159265359 * alpha2 * NdotH2 * NdotH2);
    }
    else if (distribution == 1) {

        float alpha2 = alpha * alpha;
        float denom = NdotH * NdotH * (alpha2 - 1.0) + 1.0;
        D = alpha2 / (3.14159265359 * denom * denom);
    }
    else {

        float normalization = (2.0 + 2.0/alpha) / (2.0 * 3.14159265359);
        float power = 2.0 / (alpha * alpha) - 2.0;
        D = normalization * pow(NdotH, power);
    }

    float G1_v = 2.0 * NdotV / (NdotV + sqrt(alpha + (1.0 - alpha) * NdotV * NdotV));
    float G1_l = 2.0 * NdotL / (NdotL + sqrt(alpha + (1.0 - alpha) * NdotL * NdotL));
    float G = G1_v * G1_l;
    

    float F = F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
    float specular = (D * G * F) / (4.0 * NdotV * NdotL);
    
    // Calculate diffuse component (reduced for metallic surfaces)
    float diffuse = (1.0 - F) * (1.0 - metallic) * NdotL / 3.14159265359;
    
    // Combine diffuse and specular
    return albedo * (diffuse + specular * NdotL);
}

// Phong lighting model
vec3 calculatePhong(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 albedo, float specularPower) {
    float NdotL = max(dot(normal, lightDir), 0.0);
    
    // Early exit if surface is facing away from light
    if (NdotL < 0.001) return vec3(0.0);
    
    // Diffuse term
    vec3 diffuse = albedo * NdotL;
    
    // Reflection vector
    vec3 reflectDir = reflect(-lightDir, normal);
    
    // Specular term
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularPower);
    vec3 specular = vec3(0.3) * spec; // Simple white specular
    
    return diffuse + specular;
}

// Blinn-Phong lighting model
vec3 calculateBlinnPhong(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 albedo, float specularPower) {
    float NdotL = max(dot(normal, lightDir), 0.0);
    
    // Early exit if surface is facing away from light
    if (NdotL < 0.001) return vec3(0.0);
    
    // Diffuse term
    vec3 diffuse = albedo * NdotL;
    
    // Halfway vector
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    // Specular term (using halfway vector)
    float spec = pow(max(dot(normal, halfwayDir), 0.0), specularPower);
    vec3 specular = vec3(0.3) * spec; // Simple white specular
    
    return diffuse + specular;
}

// GGX/Trowbridge-Reitz lighting model
vec3 calculateGGX(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 albedo, 
                 float roughness, float metallic, float F0) {
    // This is essentially a specialized version of Cook-Torrance using GGX distribution
    // Passing GGX (1) as the distribution type
    return calculateCookTorrance(normal, lightDir, viewDir, albedo, roughness, metallic, F0, 1);
}

// Custom BRDF stub - this will be replaced by the custom implementation
vec3 calculateCustomBRDF(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 albedo, 
                        float param1, float param2, float param3, float param4) {
    // Default implementation (simple Lambertian diffuse)
    float NdotL = max(dot(normal, lightDir), 0.0);
    return albedo * NdotL / PI;
}

// Full physically-based rendering model
vec3 calculatePBR(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 albedo, 
                 float roughness, float metallic, float ao) {
    // This is a complete PBR model with energy conservation, etc.
    // For now, we'll use Cook-Torrance with GGX as the base
    
    // Halfway vector
    vec3 H = normalize(viewDir + lightDir);
    
    // Dot products for all the key vectors
    float NdotL = max(dot(normal, lightDir), 0.001);
    float NdotV = max(dot(normal, viewDir), 0.001);
    float NdotH = max(dot(normal, H), 0.0);
    float HdotV = max(dot(H, viewDir), 0.0);
    
    // F0 represents the base reflectivity
    // For non-metals (dialectrics) this is typically 0.04
    // For metals, we use the albedo color
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    
    // ------- Fresnel-Schlick -------
    vec3 F = F0 + (1.0 - F0) * pow(1.0 - HdotV, 5.0);
    
    // ------- Distribution (GGX/Trowbridge-Reitz) -------
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = NdotH * NdotH * (alpha2 - 1.0) + 1.0;
    float D = alpha2 / (PI * denom * denom);
    
    // ------- Geometry (Smith's method with GGX) -------
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    float G1_v = NdotV / (NdotV * (1.0 - k) + k);
    float G1_l = NdotL / (NdotL * (1.0 - k) + k);
    float G = G1_v * G1_l;
    
    // ------- Specular BRDF -------
    vec3 specular = (D * F * G) / (4.0 * NdotV * NdotL);
    
    // ------- Diffuse BRDF -------
    // We scale by 1.0 - F because the energy that's reflected can't also be refracted
    // We scale by 1.0 - metallic because metals don't have a diffuse component
    vec3 diffuse = (1.0 - F) * (1.0 - metallic) * albedo / PI;
    
    // Combine lighting and apply ambient occlusion
    vec3 result = (diffuse + specular) * NdotL;
    result *= ao;
    
    return result;
}

void main() {
    // Normalize vectors
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Calculate lighting
    vec3 result = vec3(0.0);
    
    // Ambient component
    vec3 ambient = albedo * 0.03 * ao;
    
    // Process each light
    for(int i = 0; i < 4; i++) {
        vec3 lightDir = normalize(lightPositions[i] - FragPos);
        
        // Apply the selected lighting model
        vec3 lightContribution;
        
        if (lightingModel == 0) {
            // Phong model
            float specularPower = 32.0;
            lightContribution = calculatePhong(norm, lightDir, viewDir, albedo, specularPower);
        }
        else if (lightingModel == 1) {
            // Blinn-Phong model
            float specularPower = 32.0;
            lightContribution = calculateBlinnPhong(norm, lightDir, viewDir, albedo, specularPower);
        }
        else if (lightingModel == 2) {
            // Oren-Nayar model
            lightContribution = calculateOrenNayar(norm, lightDir, viewDir, albedo, lighting.roughness);
        }
        else if (lightingModel == 3) {
            // Cook-Torrance model
            lightContribution = calculateCookTorrance(
                norm, lightDir, viewDir, albedo, 
                lighting.roughness, lighting.metallic, lighting.fresnel, int(lighting.distribution)
            );
        }
        else if (lightingModel == 4) {
            // GGX model
            lightContribution = calculateGGX(
                norm, lightDir, viewDir, albedo, 
                lighting.roughness, lighting.metallic, lighting.fresnel
            );
        }
        else if (lightingModel == 5) {
            // Full PBR model
            lightContribution = calculatePBR(
                norm, lightDir, viewDir, albedo, 
                lighting.roughness, lighting.metallic, ao
            );
        }
        else if (lightingModel == 6) {
            // Custom BRDF model
            lightContribution = calculateCustomBRDF(
                norm, lightDir, viewDir, albedo, 
                lighting.param1, lighting.param2, lighting.param3, lighting.param4
            );
        }
        else {
            // Default to Blinn-Phong if no valid model is selected
            float specularPower = 32.0;
            lightContribution = calculateBlinnPhong(norm, lightDir, viewDir, albedo, specularPower);
        }
        
        // Apply light color and attenuation
        float distance = length(lightPositions[i] - FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        
        result += lightContribution * lightColors[i] * attenuation;
    }
    
    // Add ambient lighting
    result += ambient;
    
    // Add emissive component if any
    result += albedo * emissive;
    
    // Gamma correction (assuming gamma = 2.2)
    result = pow(result, vec3(1.0/2.2));
    
    // Final color
    FragColor = vec4(result, 1.0);
}
