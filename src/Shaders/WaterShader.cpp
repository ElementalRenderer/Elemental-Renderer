/**
 * @file WaterShader.cpp
 * @brief Water surface shader implementation
 */

#include "Shaders/WaterShader.h"

namespace ElementalRenderer {

// Vertex Shader Source
const char* WaterShader::s_vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 WorldPos;
out vec4 ClipSpacePos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float time;

void main() {
    // Add some waviness to the water surface
    vec3 pos = aPos;
    float waveHeight = 0.05;
    float waveFreq = 2.0;
    pos.y += sin(pos.x * waveFreq + time) * cos(pos.z * waveFreq + time) * waveHeight;
    
    WorldPos = vec3(model * vec4(pos, 1.0));
    TexCoords = aTexCoords;
    
    ClipSpacePos = projection * view * vec4(WorldPos, 1.0);
    gl_Position = ClipSpacePos;
}
)";

// Fragment Shader Source
const char* WaterShader::s_fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec4 ClipSpacePos;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float time;
uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;

uniform float waveStrength;
uniform float shineDamper;
uniform float reflectivity;

const float waterDepthColorBlend = 0.5; // Controls how much water color is blended based on depth

void main() {
    // Normalize device coordinates
    vec2 ndc = (ClipSpacePos.xy / ClipSpacePos.w) * 0.5 + 0.5;
    
    // Get distortion from dudv map
    vec2 distortedTexCoords = texture(dudvMap, vec2(TexCoords.x + time * 0.05, TexCoords.y)).rg * 0.1;
    distortedTexCoords = TexCoords + vec2(distortedTexCoords.x, distortedTexCoords.y + time * 0.05);
    vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;
    
    // Reflection and refraction coords with distortion
    vec2 reflectionTexCoords = vec2(ndc.x, -ndc.y) + totalDistortion;
    reflectionTexCoords.x = clamp(reflectionTexCoords.x, 0.001, 0.999);
    reflectionTexCoords.y = clamp(reflectionTexCoords.y, -0.999, -0.001);
    
    vec2 refractionTexCoords = vec2(ndc.x, ndc.y) + totalDistortion;
    refractionTexCoords = clamp(refractionTexCoords, 0.001, 0.999);
    
    // Get depth of water
    float depth = texture(depthMap, refractionTexCoords).r;
    float floorDistance = 2.0 * 100.0 * 100.0 / (100.0 + 100.0 - (2.0 * depth - 1.0) * 100.0);
    float waterDistance = 2.0 * 100.0 * 100.0 / (100.0 + 100.0 - (2.0 * gl_FragCoord.z - 1.0) * 100.0);
    float waterDepth = floorDistance - waterDistance;
    
    // Get normal from normal map
    vec3 normal = texture(normalMap, distortedTexCoords).rgb;
    normal = vec3(normal.r * 2.0 - 1.0, normal.b * 3.0, normal.g * 2.0 - 1.0);
    normal = normalize(normal);
    
    // Fresnel effect (viewing angle influences reflection/refraction ratio)
    vec3 viewDir = normalize(viewPos - WorldPos);
    float refractiveFactor = dot(viewDir, normal);
    refractiveFactor = pow(refractiveFactor, 1.0);
    refractiveFactor = clamp(refractiveFactor, 0.0, 1.0);
    
    // Mix reflection and refraction
    vec3 reflectionColor = texture(reflectionTexture, reflectionTexCoords).rgb;
    vec3 refractionColor = texture(refractionTexture, refractionTexCoords).rgb;
    
    // Add blue tint to deeper water
    vec3 waterColor = vec3(0.0, 0.3, 0.5);
    refractionColor = mix(refractionColor, waterColor, clamp(waterDepth * waterDepthColorBlend, 0.0, 1.0));
    
    // Specular highlights
    vec3 lightDir = normalize(lightPos - WorldPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shineDamper);
    vec3 specular = lightColor * spec * reflectivity * clamp(waterDepth * 0.5, 0.0, 1.0);
    
    // Final color
    vec3 finalColor = mix(reflectionColor, refractionColor, refractiveFactor);
    finalColor += specular;
    
    // Add some transparency in shallow water
    float alpha = clamp(waterDepth / 3.0, 0.1, 1.0);
    
    FragColor = vec4(finalColor, alpha);
}
)";

WaterShader::WaterShader()
    : Shader(),
      m_waveStrength(0.02f),
      m_shineDamper(20.0f),
      m_reflectivity(0.5f) {
}

WaterShader::~WaterShader() {
}

bool WaterShader::loadWaterShader() {
    return loadFromSource(s_vertexShaderSource, s_fragmentShaderSource);
}

void WaterShader::setTime(float time) {
    use();
    setFloat("time", time);
}

void WaterShader::setWaterMaps(unsigned int dudvMap, unsigned int normalMap) {
    use();
    setInt("dudvMap", 0);
    setInt("normalMap", 1);
}

void WaterShader::setWaterTextures(unsigned int reflectionTexture, unsigned int refractionTexture, unsigned int depthTexture) {
    use();
    setInt("reflectionTexture", 2);
    setInt("refractionTexture", 3);
    setInt("depthMap", 4);
}

void WaterShader::setWaterProperties(float waveStrength, float shineDamper, float reflectivity) {
    m_waveStrength = waveStrength;
    m_shineDamper = shineDamper;
    m_reflectivity = reflectivity;
    
    use();
    setFloat("waveStrength", waveStrength);
    setFloat("shineDamper", shineDamper);
    setFloat("reflectivity", reflectivity);
}

} // namespace ElementalRenderer
