#include "../../include/Shaders/StyleShaderManager.h"
#include <algorithm>
#include <iostream>

StyleShaderManager::StyleShaderManager() : currentStyle(StyleShader::Style::DEFAULT) {
    styleShader = std::make_unique<StyleShader>();
}

StyleShaderManager::~StyleShaderManager() {
    // Resource cleanup happens automatically with unique_ptr
}

bool StyleShaderManager::initialize() {
    bool success = styleShader->initialize();
    if (success) {
        initializeStyleParameters();
    }
    return success;
}

bool StyleShaderManager::applyStyleByName(const std::string& styleName) {
    // Convert to lowercase for case-insensitive comparison
    std::string lowerName = styleName;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    // Check against all style names
    for (int i = 0; i < StyleShader::getStyleCount(); i++) {
        StyleShader::Style style = StyleShader::getStyleFromIndex(i);
        std::string currentName = StyleShader::getStyleName(style);
        std::transform(currentName.begin(), currentName.end(), currentName.begin(), ::tolower);
        
        if (currentName == lowerName) {
            return applyStyle(style);
        }
    }
    
    std::cerr << "Style not found: " << styleName << std::endl;
    return false;
}

bool StyleShaderManager::applyStyleByIndex(int index) {
    if (index < 0 || index >= StyleShader::getStyleCount()) {
        std::cerr << "Style index out of range: " << index << std::endl;
        return false;
    }
    
    StyleShader::Style style = StyleShader::getStyleFromIndex(index);
    return applyStyle(style);
}

bool StyleShaderManager::applyStyle(StyleShader::Style style) {
    bool success = styleShader->applyStyle(style);
    if (success) {
        currentStyle = style;
    }
    return success;
}

StyleShader::Style StyleShaderManager::getCurrentStyle() const {
    return currentStyle;
}

std::string StyleShaderManager::getCurrentStyleName() const {
    return StyleShader::getStyleName(currentStyle);
}

std::vector<std::string> StyleShaderManager::getAvailableStyleNames() const {
    std::vector<std::string> names;
    names.reserve(StyleShader::getStyleCount());
    
    for (int i = 0; i < StyleShader::getStyleCount(); i++) {
        names.push_back(StyleShader::getStyleName(StyleShader::getStyleFromIndex(i)));
    }
    
    return names;
}

std::vector<std::string> StyleShaderManager::getAvailableStyleDescriptions() const {
    std::vector<std::string> descriptions;
    descriptions.reserve(StyleShader::getStyleCount());
    
    for (int i = 0; i < StyleShader::getStyleCount(); i++) {
        descriptions.push_back(StyleShader::getStyleDescription(StyleShader::getStyleFromIndex(i)));
    }
    
    return descriptions;
}

std::vector<StyleShaderManager::StyleParameter> StyleShaderManager::getCurrentStyleParameters() const {
    auto it = styleParameters.find(currentStyle);
    if (it != styleParameters.end()) {
        return it->second;
    }
    
    // Return empty vector if style not found
    return {};
}

bool StyleShaderManager::setStyleParameter(const std::string& paramName, float value) {
    auto it = styleParameters.find(currentStyle);
    if (it == styleParameters.end()) {
        return false;
    }
    
    // Find the parameter by name
    for (auto& param : it->second) {
        if (param.name == paramName) {
            // Enforce limits
            value = std::max(param.minValue, std::min(value, param.maxValue));
            
            // Update the parameter value
            param.currentValue = value;
            
            // Get the shader and update the uniform
            auto shader = styleShader->getShader(currentStyle);
            if (shader) {
                shader->use();
                switch (param.type) {
                    case StyleParameter::FLOAT:
                        shader->setFloat(paramName, value);
                        break;
                    case StyleParameter::INT:
                        shader->setInt(paramName, static_cast<int>(value));
                        break;
                    case StyleParameter::BOOL:
                        shader->setInt(paramName, value > 0.5f ? 1 : 0);
                        break;
                }
            }
            
            return true;
        }
    }
    
    return false;
}

void StyleShaderManager::resetStyleParameters() {
    auto it = styleParameters.find(currentStyle);
    if (it == styleParameters.end()) {
        return;
    }
    
    // Reset each parameter to its default value
    for (auto& param : it->second) {
        setStyleParameter(param.name, param.defaultValue);
    }
}

std::shared_ptr<Shader> StyleShaderManager::getCurrentShader() const {
    return styleShader->getShader(currentStyle);
}

void StyleShaderManager::initializeStyleParameters() {
    // Initialize parameters for each style
    initAnimeShaderParameters();
    initPixelArtParameters();
    initIllustrationParameters();
    initToonParameters();
    initWatercolorParameters();
    initSketchParameters();
}

void StyleShaderManager::initAnimeShaderParameters() {
    std::vector<StyleParameter> params;
    
    // Outline thickness
    StyleParameter outlineThickness;
    outlineThickness.name = "outlineThickness";
    outlineThickness.displayName = "Outline Thickness";
    outlineThickness.description = "Controls the thickness of cel-shaded outlines";
    outlineThickness.minValue = 0.0f;
    outlineThickness.maxValue = 1.0f;
    outlineThickness.defaultValue = 0.3f;
    outlineThickness.currentValue = 0.3f;
    outlineThickness.type = StyleParameter::FLOAT;
    params.push_back(outlineThickness);
    
    // Cel shading levels
    StyleParameter celLevels;
    celLevels.name = "celLevels";
    celLevels.displayName = "Cel Shading Levels";
    celLevels.description = "Number of discrete shading bands for the cel effect";
    celLevels.minValue = 1.0f;
    celLevels.maxValue = 10.0f;
    celLevels.defaultValue = 3.0f;
    celLevels.currentValue = 3.0f;
    celLevels.type = StyleParameter::INT;
    params.push_back(celLevels);
    
    // Specular highlight intensity
    StyleParameter specular;
    specular.name = "specularIntensity";
    specular.displayName = "Specular Intensity";
    specular.description = "Controls the brightness of specular highlights";
    specular.minValue = 0.0f;
    specular.maxValue = 2.0f;
    specular.defaultValue = 0.7f;
    specular.currentValue = 0.7f;
    specular.type = StyleParameter::FLOAT;
    params.push_back(specular);
    
    styleParameters[StyleShader::Style::ANIME] = params;
}

void StyleShaderManager::initPixelArtParameters() {
    std::vector<StyleParameter> params;
    
    // Pixel size
    StyleParameter pixelSize;
    pixelSize.name = "pixelSize";
    pixelSize.displayName = "Pixel Size";
    pixelSize.description = "Controls the size of virtual pixels";
    pixelSize.minValue = 0.01f;
    pixelSize.maxValue = 0.2f;
    pixelSize.defaultValue = 0.05f;
    pixelSize.currentValue = 0.05f;
    pixelSize.type = StyleParameter::FLOAT;
    params.push_back(pixelSize);
    
    // Color levels
    StyleParameter colorLevels;
    colorLevels.name = "colorLevels";
    colorLevels.displayName = "Color Depth";
    colorLevels.description = "Number of color levels per channel (e.g., 4 for 4-bit style)";
    colorLevels.minValue = 2.0f;
    colorLevels.maxValue = 16.0f;
    colorLevels.defaultValue = 8.0f;
    colorLevels.currentValue = 8.0f;
    colorLevels.type = StyleParameter::INT;
    params.push_back(colorLevels);
    
    // Dithering
    StyleParameter dithering;
    dithering.name = "dithering";
    dithering.displayName = "Dithering";
    dithering.description = "Enable dithering effect for smoother color transitions";
    dithering.minValue = 0.0f;
    dithering.maxValue = 1.0f;
    dithering.defaultValue = 0.0f;
    dithering.currentValue = 0.0f;
    dithering.type = StyleParameter::BOOL;
    params.push_back(dithering);
    
    styleParameters[StyleShader::Style::PIXEL_ART] = params;
}

void StyleShaderManager::initIllustrationParameters() {
    std::vector<StyleParameter> params;
    
    // Brush strength
    StyleParameter brushStrength;
    brushStrength.name = "brushStrength";
    brushStrength.displayName = "Brush Intensity";
    brushStrength.description = "Controls how prominent the brush strokes appear";
    brushStrength.minValue = 0.0f;
    brushStrength.maxValue = 1.0f;
    brushStrength.defaultValue = 0.4f;
    brushStrength.currentValue = 0.4f;
    brushStrength.type = StyleParameter::FLOAT;
    params.push_back(brushStrength);
    
    // Color blending
    StyleParameter colorBlending;
    colorBlending.name = "colorBlending";
    colorBlending.displayName = "Color Blending";
    colorBlending.description = "Controls how much colors blend together";
    colorBlending.minValue = 0.0f;
    colorBlending.maxValue = 1.0f;
    colorBlending.defaultValue = 0.6f;
    colorBlending.currentValue = 0.6f;
    colorBlending.type = StyleParameter::FLOAT;
    params.push_back(colorBlending);
    
    // Edge softness
    StyleParameter edgeSoftness;
    edgeSoftness.name = "edgeSoftness";
    edgeSoftness.displayName = "Edge Softness";
    edgeSoftness.description = "Controls the softness of edges in the illustration";
    edgeSoftness.minValue = 0.0f;
    edgeSoftness.maxValue = 1.0f;
    edgeSoftness.defaultValue = 0.5f;
    edgeSoftness.currentValue = 0.5f;
    edgeSoftness.type = StyleParameter::FLOAT;
    params.push_back(edgeSoftness);
    
    styleParameters[StyleShader::Style::ILLUSTRATION] = params;
}

void StyleShaderManager::initToonParameters() {
    std::vector<StyleParameter> params;
    
    // Toon levels
    StyleParameter toonLevels;
    toonLevels.name = "toonLevels";
    toonLevels.displayName = "Shading Levels";
    toonLevels.description = "Number of discrete shading levels";
    toonLevels.minValue = 1.0f;
    toonLevels.maxValue = 8.0f;
    toonLevels.defaultValue = 4.0f;
    toonLevels.currentValue = 4.0f;
    toonLevels.type = StyleParameter::INT;
    params.push_back(toonLevels);
    
    // Edge detection
    StyleParameter edgeThickness;
    edgeThickness.name = "edgeThickness";
    edgeThickness.displayName = "Edge Thickness";
    edgeThickness.description = "Thickness of cartoon outlines";
    edgeThickness.minValue = 0.0f;
    edgeThickness.maxValue = 1.0f;
    edgeThickness.defaultValue = 0.5f;
    edgeThickness.currentValue = 0.5f;
    edgeThickness.type = StyleParameter::FLOAT;
    params.push_back(edgeThickness);
    
    styleParameters[StyleShader::Style::TOON] = params;
}

void StyleShaderManager::initWatercolorParameters() {
    std::vector<StyleParameter> params;
    
    // Wetness
    StyleParameter wetness;
    wetness.name = "wetness";
    wetness.displayName = "Wetness";
    wetness.description = "Controls edge bleeding and color diffusion";
    wetness.minValue = 0.0f;
    wetness.maxValue = 1.0f;
    wetness.defaultValue = 0.7f;
    wetness.currentValue = 0.7f;
    wetness.type = StyleParameter::FLOAT;
    params.push_back(wetness);
    
    // Color saturation
    StyleParameter colorSaturation;
    colorSaturation.name = "colorSaturation";
    colorSaturation.displayName = "Color Saturation";
    colorSaturation.description = "Controls the intensity of colors";
    colorSaturation.minValue = 0.0f;
    colorSaturation.maxValue = 2.0f;
    colorSaturation.defaultValue = 0.8f;
    colorSaturation.currentValue = 0.8f;
    colorSaturation.type = StyleParameter::FLOAT;
    params.push_back(colorSaturation);
    
    // Paper texture amount
    StyleParameter paperTexture;
    paperTexture.name = "paperTextureAmount";
    paperTexture.displayName = "Paper Texture";
    paperTexture.description = "Amount of paper texture visible in the rendering";
    paperTexture.minValue = 0.0f;
    paperTexture.maxValue = 1.0f;
    paperTexture.defaultValue = 0.3f;
    paperTexture.currentValue = 0.3f;
    paperTexture.type = StyleParameter::FLOAT;
    params.push_back(paperTexture);
    
    styleParameters[StyleShader::Style::WATERCOLOR] = params;
}

void StyleShaderManager::initSketchParameters() {
    std::vector<StyleParameter> params;
    
    // Stroke density
    StyleParameter strokeDensity;
    strokeDensity.name = "strokeDensity";
    strokeDensity.displayName = "Stroke Density";
    strokeDensity.description = "Controls the density of sketch strokes";
    strokeDensity.minValue = 0.0f;
    strokeDensity.maxValue = 1.0f;
    strokeDensity.defaultValue = 0.8f;
    strokeDensity.currentValue = 0.8f;
    strokeDensity.type = StyleParameter::FLOAT;
    params.push_back(strokeDensity);
    
    // Stroke width
    StyleParameter strokeWidth;
    strokeWidth.name = "strokeWidth";
    strokeWidth.displayName = "Stroke Width";
    strokeWidth.description = "Controls the width of sketch strokes";
    strokeWidth.minValue = 0.1f;
    strokeWidth.maxValue = 1.0f;
    strokeWidth.defaultValue = 0.6f;
    strokeWidth.currentValue = 0.6f;
    strokeWidth.type = StyleParameter::FLOAT;
    params.push_back(strokeWidth);
    
    // Pencil darkness
    StyleParameter pencilDarkness;
    pencilDarkness.name = "pencilDarkness";
    pencilDarkness.displayName = "Pencil Darkness";
    pencilDarkness.description = "Controls the darkness of pencil strokes";
    pencilDarkness.minValue = 0.0f;
    pencilDarkness.maxValue = 1.0f;
    pencilDarkness.defaultValue = 0.7f;
    pencilDarkness.currentValue = 0.7f;
    pencilDarkness.type = StyleParameter::FLOAT;
    params.push_back(pencilDarkness);
    
    styleParameters[StyleShader::Style::SKETCH] = params;
}
