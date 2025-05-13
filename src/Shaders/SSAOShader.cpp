/**
 * @file SSAOShader.cpp
 * @brief Screen Space Ambient Occlusion shader implementation
 */

#include "Shaders/SSAOShader.h"
#include <random>

namespace ElementalRenderer {

// SSAO Vertex Shader
const char* SSAOShader::s_ssaoVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}
)";

// SSAO Fragment Shader
const char* SSAOShader::s_ssaoFragmentShaderSource = R"(
#version 330 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];
uniform mat4 projection;
uniform int kernelSize;
uniform float radius;
uniform float bias;

// tile noise texture over screen based on screen dimensions divided by noise size
uniform vec2 noiseScale;

void main() {
    // Get input for SSAO algorithm
    vec3 fragPos = texture(gPosition, TexCoords).xyz;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);
    
    // Create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    // Iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i) {
        // Get sample position
        vec3 samplePos = TBN * samples[i]; // From tangent to view-space
        samplePos = fragPos + samplePos * radius; 
        
        // Project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // Get sample depth
        float sampleDepth = texture(gPosition, offset.xy).z; // Get depth value of kernel sample
        
        // Range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    
    occlusion = 1.0 - (occlusion / kernelSize);
    
    FragColor = occlusion;
}
)";

// SSAO Blur Vertex Shader
const char* SSAOShader::s_blurVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}
)";

// SSAO Blur Fragment Shader
const char* SSAOShader::s_blurFragmentShaderSource = R"(
#version 330 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D ssaoInput;

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0.0;
    
    for (int x = -2; x < 2; ++x) {
        for (int y = -2; y < 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, TexCoords + offset).r;
        }
    }
    
    FragColor = result / (4.0 * 4.0);
}
)";

SSAOShader::SSAOShader()
    : Shader(),
      m_noiseTexture(0),
      m_kernelSize(64),
      m_radius(0.5f),
      m_bias(0.025f) {
}

SSAOShader::~SSAOShader() {
    if (m_noiseTexture != 0) {
        glDeleteTextures(1, &m_noiseTexture);
    }
}

bool SSAOShader::loadSSAOShader() {
    return loadFromSource(s_ssaoVertexShaderSource, s_ssaoFragmentShaderSource);
}

bool SSAOShader::loadSSAOBlurShader() {
    return loadFromSource(s_blurVertexShaderSource, s_blurFragmentShaderSource);
}

void SSAOShader::generateSampleKernel(int kernelSize) {
    m_kernelSize = kernelSize;
    m_sampleKernel.resize(kernelSize);
    
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;
    
    for (int i = 0; i < kernelSize; ++i) {
        // Create sample in tangent space
        glm::vec3 sample(
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator)
        );
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        
        // Scale samples s.t. they're more focused near the origin
        float scale = static_cast<float>(i) / static_cast<float>(kernelSize);
        scale = 0.1f + 0.9f * scale * scale; // Lerp
        sample *= scale;
        m_sampleKernel[i] = sample;
    }
    
    // Update the shader with sample kernel
    use();
    for (int i = 0; i < kernelSize; ++i) {
        std::string uniformName = "samples[" + std::to_string(i) + "]";
        setVec3(uniformName, m_sampleKernel[i]);
    }
    setInt("kernelSize", kernelSize);
}

unsigned int SSAOShader::generateNoiseTexture(int size) {
    std::vector<glm::vec3> ssaoNoise;
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;
    
    for (int i = 0; i < size * size; i++) {
        // Rotate around z-axis (in tangent space)
        glm::vec3 noise(
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator) * 2.0f - 1.0f,
            0.0f
        );
        ssaoNoise.push_back(noise);
    }
    
    // Create and set up the noise texture
    unsigned int noiseTexture;
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    m_noiseTexture = noiseTexture;
    
    use();
    setVec2("noiseScale", glm::vec2(800.0f / size, 600.0f / size)); // Assuming 800x600 screen resolution
    
    return noiseTexture;
}

void SSAOShader::setSSAOParameters(float radius, float bias) {
    m_radius = radius;
    m_bias = bias;
    
    use();
    setFloat("radius", radius);
    setFloat("bias", bias);
}

void SSAOShader::setTextureInputs(unsigned int positionTexture, unsigned int normalTexture, unsigned int noiseTexture) {
    use();
    setInt("gPosition", 0);
    setInt("gNormal", 1);
    setInt("texNoise", 2);
}

} // namespace ElementalRenderer
