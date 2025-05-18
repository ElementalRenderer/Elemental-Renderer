#include "../../include/Shaders/PixelArtEffect.h"
#include <iostream>
#include <glad/glad.h>  // OpenGL loader... should be included before other OpenGL-related headers
#include <GLFW/glfw3.h> // input handling

PixelArtEffect::PixelArtEffect() 
    : ShaderEffect("Pixel Art Effect"), 
      framebuffer(0), 
      outputTexture(0) {

    // Set default parameters
    intParameters["pixelSize"] = DEFAULT_PIXEL_SIZE;
    intParameters["colorDepth"] = DEFAULT_COLOR_DEPTH;
    intParameters["ditheringEnabled"] = 0; // off by default
    intParameters["paletteMode"] = 0; // RGB by default
}

PixelArtEffect::~PixelArtEffect() {
    // Clean up OpenGL resources
    if (framebuffer != 0) {
        glDeleteFramebuffers(1, &framebuffer);
    }

    if (outputTexture != 0) {
        glDeleteTextures(1, &outputTexture);
    }
}

bool PixelArtEffect::initialize() {
    try {
        createShaders();

        // Initial width and height will be updated when first used
        createFramebuffer(1920, 1080);

        // Set initial uniform values
        shader->use();
        shader->setInt("pixelSize", intParameters["pixelSize"]);
        shader->setInt("colorDepth", intParameters["colorDepth"]);
        shader->setInt("ditheringEnabled", intParameters["ditheringEnabled"]);
        shader->setInt("paletteMode", intParameters["paletteMode"]);

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error initializing PixelArtEffect: " << e.what() << std::endl;
        return false;
    }
}

void PixelArtEffect::apply() {
    if (!enabled || !shader) {
        return;
    }

    // Bind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the scene to the framebuffer
    // (This would call back to the renderer to draw the scene)

    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Apply post-processing
    shader->use();

    // Update any dynamic parameters
    shader->setInt("screenTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, outputTexture);

    // Draw a full-screen quad
    // (This would use a quad mesh to render the processed texture)
}

void PixelArtEffect::setPixelSize(int size) {
    if (size < 1) size = 1;
    setIntParameter("pixelSize", size);
}

void PixelArtEffect::setColorDepth(int depth) {
    if (depth < 1) depth = 1;
    if (depth > 256) depth = 256;
    setIntParameter("colorDepth", depth);
}

void PixelArtEffect::setDithering(bool enabled) {
    setIntParameter("ditheringEnabled", enabled ? 1 : 0);
}

void PixelArtEffect::setPaletteMode(int mode) {
    setIntParameter("paletteMode", mode);
}

void PixelArtEffect::createShaders() {
    // Create a new shader program
    shader = std::make_shared<Shader>();

    // The actual shader code would be loaded from files, but for clarity
    // we'll define the core concepts of the pixel art shader here

    const char* vertexShaderCode = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;

        out vec2 TexCoord;

        void main() {
            gl_Position = vec4(aPos, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    const char* fragmentShaderCode = R"(
        #version 330 core
        in vec2 TexCoord;
        out vec4 FragColor;

        uniform sampler2D screenTexture;
        uniform int pixelSize;
        uniform int colorDepth;
        uniform int ditheringEnabled;
        uniform int paletteMode;

        // Dithering matrix (Bayer 4x4)
        const float ditherMatrix[16] = float[](
            0.0/16.0,  8.0/16.0,  2.0/16.0, 10.0/16.0,
            12.0/16.0, 4.0/16.0, 14.0/16.0,  6.0/16.0,
            3.0/16.0, 11.0/16.0,  1.0/16.0,  9.0/16.0,
            15.0/16.0, 7.0/16.0, 13.0/16.0,  5.0/16.0
        );

        // Example custom palette (CGA-like)
        const vec3 customPalette[16] = vec3[](
            vec3(0.0, 0.0, 0.0),      // Black
            vec3(0.0, 0.0, 0.67),     // Blue
            vec3(0.0, 0.67, 0.0),     // Green
            vec3(0.0, 0.67, 0.67),    // Cyan
            vec3(0.67, 0.0, 0.0),     // Red
            vec3(0.67, 0.0, 0.67),    // Magenta
            vec3(0.67, 0.33, 0.0),    // Brown
            vec3(0.67, 0.67, 0.67),   // Light gray
            vec3(0.33, 0.33, 0.33),   // Dark gray
            vec3(0.33, 0.33, 1.0),    // Light blue
            vec3(0.33, 1.0, 0.33),    // Light green
            vec3(0.33, 1.0, 1.0),     // Light cyan
            vec3(1.0, 0.33, 0.33),    // Light red
            vec3(1.0, 0.33, 1.0),     // Light magenta
            vec3(1.0, 1.0, 0.33),     // Yellow
            vec3(1.0, 1.0, 1.0)       // White
        );

        vec3 findClosestPaletteColor(vec3 color) {
            float minDist = 100000.0;
            vec3 closest = vec3(0.0);

            for (int i = 0; i < 16; i++) {
                float dist = distance(color, customPalette[i]);
                if (dist < minDist) {
                    minDist = dist;
                    closest = customPalette[i];
                }
            }

            return closest;
        }

        void main() {
            // Apply pixelation by snapping to a grid
            vec2 texSize = textureSize(screenTexture, 0);
            vec2 pixelGrid = vec2(pixelSize) / texSize;
            vec2 pixelatedUV = floor(TexCoord / pixelGrid) * pixelGrid + pixelGrid * 0.5;

            // Sample the texture
            vec3 color = texture(screenTexture, pixelatedUV).rgb;

            // Apply color quantization
            if (paletteMode == 0) {
                // RGB quantization
                float levels = float(colorDepth);

                // Apply dithering if enabled
                if (ditheringEnabled == 1) {
                    int x = int(mod(gl_FragCoord.x, 4.0));
                    int y = int(mod(gl_FragCoord.y, 4.0));
                    float dither = ditherMatrix[y * 4 + x];

                    // Add dither noise and quantize
                    color = floor((color + dither / levels) * levels) / levels;
                } else {
                    // Just quantize
                    color = floor(color * levels) / levels;
                }
            } else if (paletteMode == 1) {
                // Custom palette
                color = findClosestPaletteColor(color);
            }

            // Output the final color
            FragColor = vec4(color, 1.0);
        }
    )";

    // In a real implementation, load shaders from files instead
    // shader->loadFromFiles("pixelart_vert.glsl", "pixelart_frag.glsl");

    // Here we just simulate it
    shader->compile(vertexShaderCode, fragmentShaderCode);
}

void PixelArtEffect::createFramebuffer(int width, int height) {
    // Clean up previous resources if they exist
    if (framebuffer != 0) {
        glDeleteFramebuffers(1, &framebuffer);
    }

    if (outputTexture != 0) {
        glDeleteTextures(1, &outputTexture);
    }

    // Create a new framebuffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Create a texture to hold the render
    glGenTextures(1, &outputTexture);
    glBindTexture(GL_TEXTURE_2D, outputTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTexture, 0);

    // Create a renderbuffer for depth and stencil
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
