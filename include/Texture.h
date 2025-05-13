/**
 * @file Texture.h
 * @brief Texture handling for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_TEXTURE_H
#define ELEMENTAL_RENDERER_TEXTURE_H

#include <string>

namespace ElementalRenderer {

/**
 * @brief Class for handling textures
 */
class Texture {
public:
    enum class FilterMode {
        NEAREST,
        LINEAR,
        NEAREST_MIPMAP_NEAREST,
        LINEAR_MIPMAP_NEAREST,
        NEAREST_MIPMAP_LINEAR,
        LINEAR_MIPMAP_LINEAR
    };
    
    enum class WrapMode {
        REPEAT,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER
    };
    
    Texture();
    
    ~Texture();
    
    bool loadFromFile(const std::string& path, bool generateMipMaps = true);
    
    bool loadFromMemory(const unsigned char* data, int width, int height, int channels, bool generateMipMaps = true);
    
    void bind(unsigned int unit = 0) const;
    
    void setFilterMode(FilterMode minFilter, FilterMode magFilter);
    
    void setWrapMode(WrapMode sWrap, WrapMode tWrap);
    
    unsigned int getId() const;
    
    int getWidth() const;
    
    int getHeight() const;
    
    int getChannels() const;

private:
    unsigned int m_textureId;
    int m_width;
    int m_height;
    int m_channels;
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_TEXTURE_H
