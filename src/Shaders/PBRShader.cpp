/**
 * @file PBRShader.cpp
 * @brief rendering shader implementation
 */

#include "Shaders/PBRShader.h"
#include <glm/gtc/type_ptr.hpp>

namespace ElementalRenderer {

// vertex shaders
const char* PBRShader::s_vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;
out mat3 TBN;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    TexCoords = aTexCoords;
    WorldPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
    TBN = mat3(T, B, N);
    
    gl_Position = projection * view * vec4(WorldPos, 1.0);
}
)";

// frag shader source method
const char* PBRShader::s_fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;
in mat3 TBN;

// Material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// non-textured rendering
uniform vec3 albedoValue;
uniform float metallicValue;
uniform float roughnessValue;
uniform float aoValue;
/**
 * @file PBRShader.cpp
 * @brief Implementation of the PBRShader class
 */

#include "Shaders/PBRShader.h"
#include <iostream>

namespace ElementalRenderer {

// static shader source constants
const char* PBRShader::s_vertexShaderSource = R"(
#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    TexCoords = aTexCoords;
    WorldPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    
    vec3 T = normalize(mat3(model) * aTangent);
    vec3 B = normalize(mat3(model) * aBitangent);
    vec3 N = normalize(mat3(model) * aNormal);
    TBN = mat3(T, B, N);
    
    gl_Position = projection * view * vec4(WorldPos, 1.0);
}
)";

const char* PBRShader::s_fragmentShaderSource = R"(
#version 410 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;
in mat3 TBN;

// material textures
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// non-textured rendering
uniform vec3 albedoValue;
uniform float metallicValue;
uniform float roughnessValue;
uniform float aoValue;

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform int lightCount;

uniform vec3 camPos;
uniform bool useTextures;

const float PI = 3.14159265359;

// PBR functions
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

void main() {
    // sample material properties from textures or use uniform values
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
    vec3 normal;
    
    if (useTextures) {
        albedo = texture(albedoMap, TexCoords).rgb;
        metallic = texture(metallicMap, TexCoords).r;
        roughness = texture(roughnessMap, TexCoords).r;
        ao = texture(aoMap, TexCoords).r;
        
        // get normal from normal map
        normal = texture(normalMap, TexCoords).rgb;
        normal = normal * 2.0 - 1.0;
        normal = normalize(TBN * normal);
    } else {
        albedo = albedoValue;
        metallic = metallicValue;
        roughness = roughnessValue;
        ao = aoValue;
        normal = normalize(Normal);
    }

    // simple lighting for now
    vec3 lighting = vec3(0.0);
    vec3 N = normalize(normal);
    vec3 V = normalize(camPos - WorldPos);
    
    for (int i = 0; i < lightCount; i++) {
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        
        // simple diffuse lighting
        float diff = max(dot(N, L), 0.0);
        lighting += diff * lightColors[i] * albedo;
    }
    
    // add ambient
    vec3 ambient = vec3(0.03) * albedo * ao;
    lighting += ambient;
    
    // gamma correction
    lighting = lighting / (lighting + vec3(1.0));
    lighting = pow(lighting, vec3(1.0/2.2));
    
    FragColor = vec4(lighting, 1.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
)";

// TODO: better PBRShader methods would go here

} // namespace ElementalRenderer
// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform int lightCount;

uniform vec3 camPos;
uniform bool useTextures;

const float PI = 3.14159265359;

// PBR calculations
vec3 getNormalFromMap() {
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;
    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main() {
    // material shit idk
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
    
    if (useTextures) {
        albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
        metallic = texture(metallicMap, TexCoords).r;
        roughness = texture(roughnessMap, TexCoords).r;
        ao = texture(aoMap, TexCoords).r;
    } else {
        albedo = albedoValue;
        metallic = metallicValue;
        roughness = roughnessValue;
        ao = aoValue;
    }
    
    // get real
    vec3 N;
    if (useTextures) {
        N = getNormalFromMap();
    } else {
        N = normalize(Normal);
    }
    
    vec3 V = normalize(camPos - WorldPos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < lightCount; ++i) {
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;     

        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    
    vec3 color = ambient + Lo;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    FragColor = vec4(color, 1.0);
}
)";

PBRShader::PBRShader() : Shader() {
}

PBRShader::~PBRShader() {
}

bool PBRShader::loadStandardPBR() {
    return loadFromSource(s_vertexShaderSource, s_fragmentShaderSource);
}

void PBRShader::setMaterialProperties(const glm::vec3& albedo, float metallic, float roughness, float ao) {
    use();
    setBool("useTextures", false);
    setVec3("albedoValue", albedo);
    setFloat("metallicValue", metallic);
    setFloat("roughnessValue", roughness);
    setFloat("aoValue", ao);
}

void PBRShader::setTextureMaps(unsigned int albedoMap, unsigned int normalMap, unsigned int metallicMap, 
                                unsigned int roughnessMap, unsigned int aoMap) {
    use();
    setBool("useTextures", true);

    setInt("albedoMap", 0);
    setInt("normalMap", 1);
    setInt("metallicMap", 2);
    setInt("roughnessMap", 3);
    setInt("aoMap", 4);
}

void PBRShader::setLights(const glm::vec3* positions, const glm::vec3* colors, int count) {
    use();
    
    count = std::min(count, 4);
    setInt("lightCount", count);
    
    for (int i = 0; i < count; i++) {
        std::string posName = "lightPositions[" + std::to_string(i) + "]";
        std::string colorName = "lightColors[" + std::to_string(i) + "]";
        
        setVec3(posName, positions[i]);
        setVec3(colorName, colors[i]);
    }
}

} // namespace ElementalRenderer
