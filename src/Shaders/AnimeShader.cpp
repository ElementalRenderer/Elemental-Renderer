#include "../../include/Shaders/AnimeShader.h"
#include <iostream>

AnimeShader::AnimeShader() 
    : ShaderEffect("Anime Shader") {
    
    // Set default parameters
    floatParameters["outlineThickness"] = DEFAULT_OUTLINE_THICKNESS;
    intParameters["colorBands"] = DEFAULT_COLOR_BANDS;
    floatParameters["specularIntensity"] = DEFAULT_SPECULAR_INTENSITY;
    
    // Default outline color (black)
    floatParameters["outlineColorR"] = 0.0f;
    floatParameters["outlineColorG"] = 0.0f;
    floatParameters["outlineColorB"] = 0.0f;
}

AnimeShader::~AnimeShader() {

}

bool AnimeShader::initialize() {
    try {
        createShaders();

        shader->use();
        shader->setFloat("outlineThickness", floatParameters["outlineThickness"]);
        shader->setInt("colorBands", intParameters["colorBands"]);
        shader->setFloat("specularIntensity", floatParameters["specularIntensity"]);
        shader->setVec3("outlineColor", 
                        floatParameters["outlineColorR"],
                        floatParameters["outlineColorG"],
                        floatParameters["outlineColorB"]);
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error initializing AnimeShader: " << e.what() << std::endl;
        return false;
    }
}

void AnimeShader::apply() {
    if (!enabled || !shader) {
        return;
    }
    
    shader->use();
}

void AnimeShader::setOutlineColor(float r, float g, float b) {
    floatParameters["outlineColorR"] = r;
    floatParameters["outlineColorG"] = g;
    floatParameters["outlineColorB"] = b;
    
    if (shader) {
        shader->use();
        shader->setVec3("outlineColor", r, g, b);
    }
}

void AnimeShader::setOutlineThickness(float thickness) {
    setFloatParameter("outlineThickness", thickness);
}

void AnimeShader::setColorBands(int bands) {
    setIntParameter("colorBands", bands);
}

void AnimeShader::setSpecularIntensity(float intensity) {
    setFloatParameter("specularIntensity", intensity);
}

void AnimeShader::createShaders() {
    // create a new shader program
    shader = std::make_shared<Shader>();
    
    // The actual shader code would be loaded from files, but for clarity for people,
    // we'll define the core concepts of the anime shader here
    
    const char* vertexShaderCode = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoord;
        
        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoord;
        out vec3 ViewPos;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        uniform float outlineThickness;
        uniform bool isOutlinePass;
        
        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexCoord = aTexCoord;
            
            if (isOutlinePass) {
                vec3 extrudedPos = aPos + aNormal * outlineThickness;
                gl_Position = projection * view * model * vec4(extrudedPos, 1.0);
            } else {
                gl_Position = projection * view * model * vec4(aPos, 1.0);
            }
        }
    )";
    
    const char* fragmentShaderCode = R"(
        #version 330 core
        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoord;
        
        out vec4 FragColor;
        
        uniform sampler2D diffuseTexture;
        uniform vec3 lightPos;
        uniform vec3 viewPos;
        uniform vec3 lightColor;
        uniform vec3 objectColor;
        
        uniform bool isOutlinePass;
        uniform vec3 outlineColor;
        
        uniform int colorBands;
        uniform float specularIntensity;
        
        void main() {
            if (isOutlinePass) {
                FragColor = vec4(outlineColor, 1.0);
            } else {

                vec3 norm = normalize(Normal);
                vec3 lightDir = normalize(lightPos - FragPos);

                float diff = max(dot(norm, lightDir), 0.0);
                diff = floor(diff * float(colorBands)) / float(colorBands);
                
                vec3 diffuse = diff * lightColor;

                vec3 viewDir = normalize(viewPos - FragPos);
                vec3 reflectDir = reflect(-lightDir, norm);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0) * specularIntensity;
                spec = step(0.5, spec);
                
                vec3 specular = spec * lightColor;
                float ambientStrength = 0.3;
                vec3 ambient = ambientStrength * lightColor;
                vec3 result = (ambient + diffuse) * objectColor + specular;
                vec4 texColor = texture(diffuseTexture, TexCoord);
                FragColor = vec4(result, 1.0) * texColor;
            }
        }
    )";

    // shader->loadFromFiles("anime_vert.glsl", "anime_frag.glsl");
    shader->compile(vertexShaderCode, fragmentShaderCode);
}
