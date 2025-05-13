/**
 * @file PostProcessShader.cpp
 * @brief Post-processing shader implementation
 */

#include "Shaders/PostProcessShader.h"

namespace ElementalRenderer {

// Vertex Shader Source
const char* PostProcessShader::s_vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
}
)";

// frag Shader Source
const char* PostProcessShader::s_fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float time;
uniform float strength;
uniform int effectType; // 0: none, 1: blur, 2: grayscale, 3: vignette, 4: chromatic aberration

// Blur kernel
const float offset = 1.0 / 300.0;  
vec2 offsets[9] = vec2[](
    vec2(-offset,  offset),  // top-left
    vec2( 0.0f,    offset),  // top-center
    vec2( offset,  offset),  // top-right
    vec2(-offset,  0.0f),    // center-left
    vec2( 0.0f,    0.0f),    // center-center
    vec2( offset,  0.0f),    // center-right
    vec2(-offset, -offset),  // bottom-left
    vec2( 0.0f,   -offset),  // bottom-center
    vec2( offset, -offset)   // bottom-right    
);

float kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);

void main() {             
    if (effectType == 0) {
        // no effect
        FragColor = texture(screenTexture, TexCoords);
    } 
    else if (effectType == 1) {
        // blur effect
        vec3 sampleTex[9];
        for(int i = 0; i < 9; i++) {
            sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i] * strength));
        }
        vec3 col = vec3(0.0);
        for(int i = 0; i < 9; i++)
            col += sampleTex[i] * kernel[i];
        
        // mix with original based on strength
        vec3 originalColor = texture(screenTexture, TexCoords).rgb;
        col = mix(originalColor, col, strength);
        
        FragColor = vec4(col, 1.0);
    } 
    else if (effectType == 2) {
        // grayscale effect
        vec3 col = texture(screenTexture, TexCoords).rgb;
        float average = 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;
        vec3 grayscale = vec3(average);
        
        // mix with original based on strength
        col = mix(col, grayscale, strength);
        
        FragColor = vec4(col, 1.0);
    }
    else if (effectType == 3) {
        // vignette effect
        vec3 col = texture(screenTexture, TexCoords).rgb;
        vec2 center = vec2(0.5, 0.5);
        float distance = length(TexCoords - center);
        float vignette = smoothstep(0.8, 0.2, distance * strength);
        col = col * vignette;
        
        FragColor = vec4(col, 1.0);
    }
    else if (effectType == 4) {
        // chromatic aberration
        float aberrationStrength = 0.01 * strength;
        
        vec3 col;
        col.r = texture(screenTexture, TexCoords + vec2(aberrationStrength, 0.0)).r;
        col.g = texture(screenTexture, TexCoords).g;
        col.b = texture(screenTexture, TexCoords - vec2(aberrationStrength, 0.0)).b;
        
        FragColor = vec4(col, 1.0);
    }
}
)";

PostProcessShader::PostProcessShader() 
    : Shader(), 
      m_currentEffect(PostProcessEffect::NONE),
      m_effectStrength(0.5f) {
}

PostProcessShader::~PostProcessShader() {
}

bool PostProcessShader::loadStandardPostProcess() {
    return loadFromSource(s_vertexShaderSource, s_fragmentShaderSource);
}

void PostProcessShader::setEffect(PostProcessEffect effect) {
    m_currentEffect = effect;
    use();
    setInt("effectType", static_cast<int>(effect));
}

void PostProcessShader::setEffectStrength(float strength) {
    m_effectStrength = strength;
    use();
    setFloat("strength", strength);
}

void PostProcessShader::setTime(float time) {
    use();
    setFloat("time", time);
}

PostProcessEffect PostProcessShader::getEffect() const {
    return m_currentEffect;
}

} // namespace ElementalRenderer
