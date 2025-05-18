#pragma once

#include "../Renderer.h"
#include <memory>
#include <string>
#include <functional>

/**
 * RenderingSettingsUI provides a user interface for selecting and adjusting
 * artistic shader styles and their parameters
 */
class RenderingSettingsUI {
public:
    RenderingSettingsUI();
    ~RenderingSettingsUI();
    
    // Initialize the UI
    bool initialize();
    
    // Set the renderer to control
    void setRenderer(std::shared_ptr<Renderer> renderer);
    
    // Render the UI
    void render();
    
    // Handle UI events
    void handleEvents();
    
    // Set callback for style changes
    void setOnStyleChangedCallback(std::function<void(const std::string&)> callback);
    
    // Set callback for parameter changes
    void setOnParameterChangedCallback(std::function<void(const std::string&, float)> callback);
    
private:
    // Reference to the renderer
    std::shared_ptr<Renderer> renderer;
    
    // Selected style index
    int selectedStyleIndex;
    
    // UI state
    bool showStyleSelector;
    bool showParameterAdjustments;
    
    // Callbacks
    std::function<void(const std::string&)> onStyleChanged;
    std::function<void(const std::string&, float)> onParameterChanged;
    
    // UI sections
    void renderStyleSelector();
    void renderParameterAdjustments();
    void renderPresetSelector();
    
    // Style management
    void applySelectedStyle();
};
