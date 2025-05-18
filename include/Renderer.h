#pragma once

#include "Camera.h"
#include "Shaders/StyleShaderManager.h"
#include <memory>
#include <string>
#include <vector>

/**
 * Renderer class manages the rendering pipeline and scene visualization
 */
class Renderer {
public:
    Renderer();
    ~Renderer();
    
    // Initialize the renderer
    bool initialize(int width, int height);
    
    // Resize the renderer viewport
    void resize(int width, int height);
    
    // Set active camera
    void setCamera(std::shared_ptr<Camera> camera);
    
    // Render the scene
    void render();
    
    // Set clear color
    void setClearColor(float r, float g, float b, float a = 1.0f);
    
    // Style shader management
    bool setShaderStyle(const std::string& styleName);
    bool setShaderStyleByIndex(int index);
    std::string getCurrentStyleName() const;
    std::vector<std::string> getAvailableStyles() const;
    std::vector<std::string> getAvailableStyleDescriptions() const;
    
    // Style parameter management
    std::vector<StyleShaderManager::StyleParameter> getCurrentStyleParameters() const;
    bool setStyleParameter(const std::string& paramName, float value);
    void resetStyleParameters();
    
private:
    // Viewport dimensions
    int viewportWidth;
    int viewportHeight;
    
    // Clear color
    float clearColor[4];
    
    // Active camera
    std::shared_ptr<Camera> camera;
    
    // Style shader manager
    std::unique_ptr<StyleShaderManager> styleShaderManager;
    
    // Internal rendering methods
    void setupRenderState();
    void renderScene();
    void applyPostProcessing();
};
