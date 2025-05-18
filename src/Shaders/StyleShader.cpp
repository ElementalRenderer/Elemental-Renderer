#include "../../include/Shaders/StyleShader.h"

StyleShader::StyleShader() : currentStyle(Style::DEFAULT) {
    // Constructor
}

StyleShader::~StyleShader() {
    // Clean up resources
}

bool StyleShader::initialize() {
    try {
        // Initialize all the style shaders
        initAnimeShader();
        initPixelArtShader();
        initIllustrationShader();
        initToonShader();
        initWatercolorShader();
        initSketchShader();
        
        return true;
    } catch (const std::exception& e) {
        // Log error
        return false;
    }
}

bool StyleShader::applyStyle(Style style) {
    auto shaderIt = shaders.find(style);
    if (shaderIt == shaders.end()) {
        return false;
    }
    
    // Apply the shader
    shaderIt->second->use();
    
    // Set style-specific parameters
    setDefaultParameters(style);
    
    currentStyle = style;
    return true;
}

std::shared_ptr<Shader> StyleShader::getShader(Style style) {
    auto shaderIt = shaders.find(style);
    if (shaderIt == shaders.end()) {
        return nullptr;
    }
    
    return shaderIt->second;
}

std::string StyleShader::getStyleName(Style style) {
    switch (style) {
        case Style::ANIME:
            return "Anime";
        case Style::PIXEL_ART:
            return "Pixel Art";
        case Style::ILLUSTRATION:
            return "Illustration";
        case Style::TOON:
            return "Toon";
        case Style::WATERCOLOR:
            return "Watercolor";
        case Style::SKETCH:
            return "Sketch";
        case Style::DEFAULT:
            return "Default";
        default:
            return "Unknown";
    }
}

std::string StyleShader::getStyleDescription(Style style) {
    switch (style) {
        case Style::ANIME:
            return "Cell-shaded anime style with bold outlines, inspired by Japanese animation";
        case Style::PIXEL_ART:
            return "Retro pixel art style with reduced colors and pixelated edges";
        case Style::ILLUSTRATION:
            return "Painterly illustration style with soft edges and artistic color blending";
        case Style::TOON:
            return "Classic cartoon-style rendering with flat colors and simplified shading";
        case Style::WATERCOLOR:
            return "Soft watercolor effect with gentle color bleeding and paper texture";
        case Style::SKETCH:
            return "Hand-drawn sketch effect with visible strokes and pencil texturing";
        case Style::DEFAULT:
            return "Standard physically-based rendering with realistic lighting and materials";
        default:
            return "Unknown style";
    }
}

int StyleShader::getStyleCount() {
    return 7; // Number of styles in the enum
}

StyleShader::Style StyleShader::getStyleFromIndex(int index) {
    switch (index) {
        case 0:
            return Style::DEFAULT;
        case 1:
            return Style::ANIME;
        case 2:
            return Style::PIXEL_ART;
        case 3:
            return Style::ILLUSTRATION;
        case 4:
            return Style::TOON;
        case 5:
            return Style::WATERCOLOR;
        case 6:
            return Style::SKETCH;
        default:
            return Style::DEFAULT;
    }
}

void StyleShader::initAnimeShader() {
    auto shader = std::make_shared<Shader>();
    
    // Vertex shader source for anime cell shading
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoords;
        
        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoords;
        out vec3 ViewPos;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        uniform vec3 viewPos;
        
        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexCoords = aTexCoords;
            ViewPos = viewPos;
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";
    
    // Fragment shader source for anime cell shading
    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoords;
        in vec3 ViewPos;
        
        uniform sampler2D diffuseTexture;
        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform vec3 objectColor;
        uniform float outlineThickness;
        uniform float celLevels;
        
        void main() {
            // Normalize the normal vector
            vec3 normal = normalize(Normal);
            
            // Calculate view direction
            vec3 viewDir = normalize(ViewPos - FragPos);
            
            // Calculate light direction
            vec3 lightDir = normalize(lightPos - FragPos);
            
            // Diffuse component with cel-shading (quantized lighting)
            float diff = max(dot(normal, lightDir), 0.0);
            diff = floor(diff * celLevels) / celLevels;
            
            // Apply colors
            vec3 diffuse = diff * lightColor;
            vec3 result = diffuse * objectColor;
            
            // Sample from texture if available
            if (textureSize(diffuseTexture, 0).x > 1) {
                result *= texture(diffuseTexture, TexCoords).rgb;
            }
            
            // Add rim lighting (edge highlighting)
            float rim = 1.0 - max(dot(viewDir, normal), 0.0);
            rim = smoothstep(0.5, 1.0, rim);
            result += rim * vec3(0.3, 0.3, 0.3);
            
            // Output with light ambient
            FragColor = vec4(result + vec3(0.1, 0.1, 0.1), 1.0);
            
            // Edge detection for outlines
            float edge = dot(viewDir, normal);
            if (edge < outlineThickness) {
                FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Black outline
            }
        }
    )";
    
    // Create the shader program
    shader->compileFromSource(vertexShaderSource, fragmentShaderSource);
    shaders[Style::ANIME] = shader;
}

void StyleShader::initPixelArtShader() {
    auto shader = std::make_shared<Shader>();
    
    // Vertex shader source for pixel art effect (standard)
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoords;
        
        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoords;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexCoords = aTexCoords;
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";
    
    // Fragment shader source for pixel art effect
    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoords;
        
        uniform sampler2D diffuseTexture;
        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform vec3 objectColor;
        uniform float pixelSize;   // Size of pixels
        uniform int colorLevels;   // Number of color levels (e.g., 8 for 8-bit style)
        
        void main() {
            // Pixelate the texture coordinates
            vec2 pixelCoords = floor(TexCoords * (1.0 / pixelSize)) * pixelSize;
            
            // Sample from texture with pixelated coordinates
            vec3 texColor;
            if (textureSize(diffuseTexture, 0).x > 1) {
                texColor = texture(diffuseTexture, pixelCoords).rgb;
            } else {
                texColor = objectColor;
            }
            
            // Basic lighting calculation (simplified for pixel art)
            vec3 normal = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(normal, lightDir), 0.0);
            
            // Quantize the lighting
            diff = floor(diff * 4.0) / 4.0;
            
            // Apply diffuse lighting
            vec3 result = (diff * lightColor + vec3(0.2)) * texColor;
            
            // Reduce color depth
            result = floor(result * float(colorLevels)) / float(colorLevels);
            
            // Output
            FragColor = vec4(result, 1.0);
        }
    )";
    
    // Create the shader program
    shader->compileFromSource(vertexShaderSource, fragmentShaderSource);
    shaders[Style::PIXEL_ART] = shader;
}

void StyleShader::initIllustrationShader() {
    auto shader = std::make_shared<Shader>();
    
    // Vertex shader source for illustration style
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoords;
        
        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoords;
        out vec3 ViewPos;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        uniform vec3 viewPos;
        
        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexCoords = aTexCoords;
            ViewPos = viewPos;
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";
    
    // Fragment shader source for illustration style
    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoords;
        in vec3 ViewPos;
        
        uniform sampler2D diffuseTexture;
        uniform sampler2D brushTexture;    // Brush stroke texture
        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform vec3 objectColor;
        uniform float brushStrength;       // Strength of brush effect
        uniform float colorBlending;       // How much colors blend together
        
        // Pseudo-random function
        float random(vec2 st) {
            return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
        }
        
        void main() {
            // Calculate basic lighting
            vec3 normal = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(normal, lightDir), 0.0);
            
            // Sample from texture if available
            vec3 texColor;
            if (textureSize(diffuseTexture, 0).x > 1) {
                texColor = texture(diffuseTexture, TexCoords).rgb;
            } else {
                texColor = objectColor;
            }
            
            // Apply diffuse lighting with a soft transition (illustration style)
            diff = smoothstep(0.1, 0.7, diff);
            
            // Sample brush texture for a painterly effect
            vec2 brushCoord = TexCoords * 5.0; // Scale for smaller brush strokes
            vec3 brushColor = texture(brushTexture, brushCoord).rgb;
            
            // Add some noise to break up the uniformity
            float noise = random(TexCoords) * 0.1;
            
            // Blend the colors for a painterly effect
            vec3 baseColor = texColor * (diff * lightColor + vec3(0.2));
            vec3 blendedColor = mix(baseColor, brushColor, brushStrength * (1.0 - diff));
            
            // Apply color blending for softer edges
            float edgeBlend = dot(normal, normalize(ViewPos - FragPos));
            edgeBlend = pow(edgeBlend, 3.0) * colorBlending;
            
            // Final color with noise
            vec3 finalColor = mix(blendedColor, texColor * lightColor, edgeBlend) + noise;
            
            // Output
            FragColor = vec4(finalColor, 1.0);
        }
    )";
    
    // Create the shader program
    shader->compileFromSource(vertexShaderSource, fragmentShaderSource);
    shaders[Style::ILLUSTRATION] = shader;
}

void StyleShader::initToonShader() {
    auto shader = std::make_shared<Shader>();
    
    // Vertex shader source for toon shader
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoords;
        
        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoords;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexCoords = aTexCoords;
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";
    
    // Fragment shader source for toon shader
    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoords;
        
        uniform sampler2D diffuseTexture;
        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform vec3 objectColor;
        uniform int toonLevels;  // Number of shading levels
        
        void main() {
            // Normalize the normal vector
            vec3 normal = normalize(Normal);
            
            // Calculate light direction
            vec3 lightDir = normalize(lightPos - FragPos);
            
            // Calculate diffuse component
            float diff = max(dot(normal, lightDir), 0.0);
            
            // Quantize the lighting into steps for toon effect
            diff = floor(diff * float(toonLevels)) / float(toonLevels);
            
            // Sample from texture if available
            vec3 texColor;
            if (textureSize(diffuseTexture, 0).x > 1) {
                texColor = texture(diffuseTexture, TexCoords).rgb;
            } else {
                texColor = objectColor;
            }
            
            // Apply lighting
            vec3 result = (diff * lightColor + vec3(0.1)) * texColor;
            
            // Output
            FragColor = vec4(result, 1.0);
        }
    )";
    
    // Create the shader program
    shader->compileFromSource(vertexShaderSource, fragmentShaderSource);
    shaders[Style::TOON] = shader;
}

void StyleShader::initWatercolorShader() {
    auto shader = std::make_shared<Shader>();
    
    // Vertex shader source for watercolor effect
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoords;
        
        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoords;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexCoords = aTexCoords;
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";
    
    // Fragment shader source for watercolor effect
    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoords;
        
        uniform sampler2D diffuseTexture;
        uniform sampler2D paperTexture;    // Paper texture for watercolor effect
        uniform sampler2D noiseTexture;    // Noise for edge variation
        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform vec3 objectColor;
        uniform float wetness;             // Controls edge bleeding
        uniform float colorSaturation;     // Controls color intensity
        
        // Simplex noise function - simplified for shader example
        float noise(vec2 st) {
            return texture(noiseTexture, st).r;
        }
        
        void main() {
            // Sample textures
            vec3 texColor;
            if (textureSize(diffuseTexture, 0).x > 1) {
                texColor = texture(diffuseTexture, TexCoords).rgb;
            } else {
                texColor = objectColor;
            }
            
            vec3 paperColor = texture(paperTexture, TexCoords * 4.0).rgb;
            
            // Basic lighting
            vec3 normal = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(normal, lightDir), 0.0);
            
            // Add edge darkening for watercolor effect
            float edgeNoise = noise(TexCoords * 10.0);
            float edgeDarkening = smoothstep(0.4, 0.5, abs(dot(normal, vec3(0.0, 0.0, 1.0))));
            edgeDarkening = mix(edgeDarkening, edgeDarkening * edgeNoise, wetness);
            
            // Color bleeding effect
            vec2 offset = vec2(edgeNoise, edgeNoise) * wetness * 0.02;
            vec3 bleedColor = texture(diffuseTexture, TexCoords + offset).rgb;
            
            // Combine effects
            vec3 baseColor = mix(texColor, bleedColor, wetness * 0.5);
            baseColor = mix(baseColor, baseColor * paperColor, 0.3);
            baseColor = mix(baseColor * 0.7, baseColor, colorSaturation);
            
            // Apply lighting with edge darkening
            vec3 result = (diff * lightColor + vec3(0.2)) * baseColor;
            result = mix(result * 0.7, result, 1.0 - edgeDarkening * 0.5);
            
            // Paper texture influence
            result = result * mix(vec3(1.0), paperColor, 0.2);
            
            // Output
            FragColor = vec4(result, 1.0);
        }
    )";
    
    // Create the shader program
    shader->compileFromSource(vertexShaderSource, fragmentShaderSource);
    shaders[Style::WATERCOLOR] = shader;
}

void StyleShader::initSketchShader() {
    auto shader = std::make_shared<Shader>();
    
    // Vertex shader source for sketch effect
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoords;
        
        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoords;
        out mat3 TBN;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexCoords = aTexCoords;
            
            // Calculate TBN matrix for normal mapping
            vec3 T = normalize(vec3(model * vec4(1.0, 0.0, 0.0, 0.0)));
            vec3 B = normalize(vec3(model * vec4(0.0, 1.0, 0.0, 0.0)));
            vec3 N = normalize(vec3(model * vec4(0.0, 0.0, 1.0, 0.0)));
            TBN = mat3(T, B, N);
            
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";
    
    // Fragment shader source for sketch effect
    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoords;
        in mat3 TBN;
        
        uniform sampler2D diffuseTexture;
        uniform sampler2D sketchTexture;    // Pencil stroke texture
        uniform sampler2D paperTexture;     // Paper texture
        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform vec3 objectColor;
        uniform vec3 viewPos;
        uniform float strokeDensity;        // Density of sketch strokes
        uniform float strokeWidth;          // Width of sketch strokes
        
        void main() {
            // Normalize the normal vector
            vec3 normal = normalize(Normal);
            
            // View and light directions
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 lightDir = normalize(lightPos - FragPos);
            
            // Calculate how much the surface faces away from the light
            float NdotL = max(dot(normal, lightDir), 0.0);
            float intensity = 1.0 - NdotL;
            
            // Add variation based on view angle
            float edgeIntensity = 1.0 - max(dot(normal, viewDir), 0.0);
            intensity = mix(intensity, intensity * edgeIntensity, 0.3);
            
            // Sample sketch texture at different scales for variation
            vec2 sketchCoord1 = TexCoords * 5.0;
            vec2 sketchCoord2 = TexCoords * 10.0 + vec2(0.1, 0.1);
            
            float sketch1 = texture(sketchTexture, sketchCoord1).r;
            float sketch2 = texture(sketchTexture, sketchCoord2).r;
            
            // Combine sketch textures for variation
            float sketchFactor = mix(sketch1, sketch2, 0.5);
            
            // Adjust sketch based on desired density and width
            sketchFactor = smoothstep(1.0 - strokeWidth, 1.0, sketchFactor);
            float sketchMask = intensity * strokeDensity;
            
            // Sample from diffuse texture
            vec3 texColor;
            if (textureSize(diffuseTexture, 0).x > 1) {
                texColor = texture(diffuseTexture, TexCoords).rgb;
            } else {
                texColor = objectColor;
            }
            
            // Sample paper texture
            vec3 paperColor = texture(paperTexture, TexCoords * 2.0).rgb;
            
            // Base lighting (lighter than normal for sketch look)
            vec3 baseColor = texColor * (lightColor * (NdotL * 0.5 + 0.5));
            
            // Paper color influence
            baseColor = mix(baseColor, baseColor * paperColor, 0.3);
            
            // Apply sketch effect - darker strokes where light is less
            float sketchContribution = sketchFactor * sketchMask;
            vec3 sketchColor = mix(baseColor, vec3(0.0, 0.0, 0.0), sketchContribution);
            
            // Output
            FragColor = vec4(sketchColor, 1.0);
        }
    )";
    
    // Create the shader program
    shader->compileFromSource(vertexShaderSource, fragmentShaderSource);
    shaders[Style::SKETCH] = shader;
}

void StyleShader::setDefaultParameters(Style style) {
    auto shader = shaders[style];
    if (!shader) return;
    
    shader->use();
    
    // Set common parameters
    shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    shader->setVec3("objectColor", 1.0f, 1.0f, 1.0f);
    
    // Set style-specific parameters
    switch (style) {
        case Style::ANIME:
            shader->setFloat("outlineThickness", 0.3f);
            shader->setFloat("celLevels", 3.0f);
            break;
        case Style::PIXEL_ART:
            shader->setFloat("pixelSize", 0.05f); // Size of virtual pixels
            shader->setInt("colorLevels", 8);     // 8-bit color depth
            break;
        case Style::ILLUSTRATION:
            shader->setFloat("brushStrength", 0.4f);
            shader->setFloat("colorBlending", 0.6f);
            break;
        case Style::TOON:
            shader->setInt("toonLevels", 4);  // Number of toon shading levels
            break;
        case Style::WATERCOLOR:
            shader->setFloat("wetness", 0.7f);
            shader->setFloat("colorSaturation", 0.8f);
            break;
        case Style::SKETCH:
            shader->setFloat("strokeDensity", 0.8f);
            shader->setFloat("strokeWidth", 0.6f);
            break;
        default:
            break;
    }
}
