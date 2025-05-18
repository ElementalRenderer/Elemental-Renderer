#pragma once

#include "ShaderEffect.h"

/**
 * PixelArtEffect implements a post-processing effect that transforms
 * rendered scenes into a pixel art style with customizable resolution and palette
 */
class PixelArtEffect : public ShaderEffect {
public:
    PixelArtEffect();
    ~PixelArtEffect() override;
    
    // Initialize shader and parameters
    bool initialize() override;
    
    // Apply the pixel art post-processing
    void apply() override;
    
    // Set pixel size (resolution)
    void setPixelSize(int size);
    
    // Set color depth (number of colors per channel)
    void setColorDepth(int depth);
    
    // Enable/disable dithering
    void setDithering(bool enabled);
    
    // Set palette mode (0: RGB, 1: Custom palette)
    void setPaletteMode(int mode);
    
private:
    // Additional resources
    unsigned int framebuffer;
    unsigned int outputTexture;
    
    // Default parameter values
    static constexpr int DEFAULT_PIXEL_SIZE = 4;
    static constexpr int DEFAULT_COLOR_DEPTH = 5;
    
    // Helper methods
    void createShaders();
    void createFramebuffer(int width, int height);
};
