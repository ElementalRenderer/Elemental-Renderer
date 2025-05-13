/**
 * @file ShadowShader.cpp
 * @brief Shadow mapping shader implementation
 */

#include "Shaders/ShadowShader.h"

namespace ElementalRenderer {

// shadow Map Generation Vertex Shader
const char* ShadowShader::s_depthVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main() {
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}
)";

// shadow Map Generation Fragment Shader
const char* ShadowShader::s_depthFragmentShaderSource = R"(
#version 330 core

void main() {
    // No need to output anything, we just care about depth values
}
)";

// main Rendering with Shadows Vertex Shader
const char* ShadowShader::s_mainVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

// main Rendering with Shadows Fragment Shader
const char* ShadowShader::s_mainFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightSpace;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float shadowBias;
uniform int pcfSize;

float ShadowCalculation(vec4 fragPosLightSpace) {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // get closest depth value from light's perspective
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(shadowBias * (1.0 - dot(normal, lightDir)), shadowBias * 0.1);
    
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    
    int halfPcfSize = pcfSize / 2;
    for(int x = -halfPcfSize; x <= halfPcfSize; ++x) {
        for(int y = -halfPcfSize; y <= halfPcfSize; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= (pcfSize * pcfSize);
    
    // keep the shadow at 0.0 when outside the light's view frustum
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main() {
    vec3 color = texture(diffuseTexture, TexCoords).rgb;
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(1.0);
    
    // ambient
    vec3 ambient = 0.3 * color;
    
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;
    
    // calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    
    FragColor = vec4(lighting, 1.0);
}
)";

ShadowShader::ShadowShader() 
    : Shader(),
      m_shadowBias(0.005f),
      m_pcfSize(3) {
}

ShadowShader::~ShadowShader() {
}

bool ShadowShader::loadShadowDepthShader() {
    return loadFromSource(s_depthVertexShaderSource, s_depthFragmentShaderSource);
}

bool ShadowShader::loadMainShaderWithShadows() {
    return loadFromSource(s_mainVertexShaderSource, s_mainFragmentShaderSource);
}

void ShadowShader::setLightSpaceMatrix(const glm::mat4& lightSpaceMatrix) {
    use();
    setMat4("lightSpaceMatrix", lightSpaceMatrix);
}

void ShadowShader::setShadowMap(unsigned int shadowMap) {
    use();
    setInt("shadowMap", 1); // assuming that 1 is the texture unit for the shadow map
}

void ShadowShader::configureShadows(float bias, int pcfSize) {
    m_shadowBias = bias;
    m_pcfSize = pcfSize;
    
    use();
    setFloat("shadowBias", bias);
    setInt("pcfSize", pcfSize);
}

} // namespace ElementalRenderer
