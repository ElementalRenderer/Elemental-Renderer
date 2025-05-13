/**
 * @file PostProcessShader.cpp
 * @brief Implementation of the PostProcessShader class
 */

#include "Shaders/PostProcessShader.h"
#include <iostream>

namespace ElementalRenderer {

// Define the static shader source constants
const char* PostProcessShader::s_vertexShaderSource = R"(
#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}
)";

const char* PostProcessShader::s_fragmentShaderSource = R"(
#version 410 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float effectStrength;

void main() {
    vec3 color = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(color, 1.0);
}
)";

// Implementation of the PostProcessShader class methods would go here

} // namespace ElementalRenderer
