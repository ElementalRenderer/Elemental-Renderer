#include "../../include/GUI/RenderingSettingsUI.h"
#include <imgui.h>
#include <iostream>

RenderingSettingsUI::RenderingSettingsUI()
    : renderer(nullptr), 
      selectedStyleIndex(0), 
      showStyleSelector(true), 
      showParameterAdjustments(true) {
}

RenderingSettingsUI::~RenderingSettingsUI() {
}

bool RenderingSettingsUI::initialize() {
    return true;
}

void RenderingSettingsUI::setRenderer(std::shared_ptr<Renderer> renderer) {
    this->renderer = renderer;
    if (renderer) {
        std::string currentStyle = renderer->getCurrentStyleName();
        auto styleNames = renderer->getAvailableStyles();
        
        for (size_t i = 0; i < styleNames.size(); i++) {
            if (styleNames[i] == currentStyle) {
                selectedStyleIndex = static_cast<int>(i);
                break;
            }
        }
    }
}

void RenderingSettingsUI::render() {
    if (!renderer) {
        return;
    }
    ImGui::Begin("Rendering Settings");

    if (ImGui::CollapsingHeader("Shader Style", ImGuiTreeNodeFlags_DefaultOpen)) {
        renderStyleSelector();
    }

    if (ImGui::CollapsingHeader("Style Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
        renderParameterAdjustments();
    }

    if (ImGui::CollapsingHeader("Style Presets", ImGuiTreeNodeFlags_DefaultOpen)) {
        renderPresetSelector();
    }
    
    ImGui::End();
}

void RenderingSettingsUI::handleEvents() {
}

void RenderingSettingsUI::setOnStyleChangedCallback(std::function<void(const std::string&)> callback) {
    onStyleChanged = callback;
}

void RenderingSettingsUI::setOnParameterChangedCallback(std::function<void(const std::string&, float)> callback) {
    onParameterChanged = callback;
}

void RenderingSettingsUI::renderStyleSelector() {
    auto styleNames = renderer->getAvailableStyles();
    auto styleDescriptions = renderer->getAvailableStyleDescriptions();

    if (ImGui::Combo("Style", &selectedStyleIndex, 
                   [](void* data, int idx, const char** out_text) {
                       auto* names = static_cast<std::vector<std::string>*>(data);
                       if (idx < 0 || idx >= static_cast<int>(names->size())) 
                           return false;
                       *out_text = (*names)[idx].c_str();
                       return true;
                   }, 
                   &styleNames, static_cast<int>(styleNames.size()))) {

        applySelectedStyle();
    }

    if (selectedStyleIndex >= 0 && selectedStyleIndex < static_cast<int>(styleDescriptions.size())) {
        ImGui::Spacing();
        ImGui::TextWrapped("%s", styleDescriptions[selectedStyleIndex].c_str());
    }

    ImGui::Spacing();
    if (ImGui::Button("Apply Style")) {
        applySelectedStyle();
    }
    
    ImGui::SameLine();

    if (ImGui::Button("Reset Parameters")) {
        renderer->resetStyleParameters();
    }
}

void RenderingSettingsUI::renderParameterAdjustments() {
    auto styleParams = renderer->getCurrentStyleParameters();
    
    if (styleParams.empty()) {
        ImGui::Text("No adjustable parameters for this style.");
        return;
    }

    for (const auto& param : styleParams) {
        float value = param.currentValue;
        bool changed = false;
        
        switch (param.type) {
            case StyleShaderManager::StyleParameter::FLOAT:
                changed = ImGui::SliderFloat(param.displayName.c_str(), &value, 
                                            param.minValue, param.maxValue, "%.2f");
                break;
                
            case StyleShaderManager::StyleParameter::INT:
                int intValue = static_cast<int>(value);
                if (ImGui::SliderInt(param.displayName.c_str(), &intValue, 
                                    static_cast<int>(param.minValue), 
                                    static_cast<int>(param.maxValue))) {
                    value = static_cast<float>(intValue);
                    changed = true;
                }
                break;
                
            case StyleShaderManager::StyleParameter::BOOL:
                bool boolValue = value > 0.5f;
                if (ImGui::Checkbox(param.displayName.c_str(), &boolValue)) {
                    value = boolValue ? 1.0f : 0.0f;
                    changed = true;
                }
                break;
        }

        if (changed) {
            renderer->setStyleParameter(param.name, value);

            if (onParameterChanged) {
                onParameterChanged(param.name, value);
            }
        }

        if (ImGui::IsItemHovered() && !param.description.empty()) {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(param.description.c_str());
            ImGui::EndTooltip();
        }
    }

    ImGui::Spacing();
    if (ImGui::Button("Reset All Parameters")) {
        renderer->resetStyleParameters();
    }
}

void RenderingSettingsUI::renderPresetSelector() {

    const char* presets[] = {
        "Default", "Vibrant", "Subtle", "Exaggerated", "Custom"
    };
    
    static int currentPreset = 0;
    if (ImGui::Combo("Preset", &currentPreset, presets, IM_ARRAYSIZE(presets))) {
        renderer->resetStyleParameters();
        
        auto styleParams = renderer->getCurrentStyleParameters();
        if (!styleParams.empty()) {
            switch (currentPreset) {
                case 1: // Vibrant

                    for (const auto& param : styleParams) {
                        if (param.name.find("Saturation") != std::string::npos ||
                            param.name.find("Intensity") != std::string::npos) {
                            renderer->setStyleParameter(param.name, 
                                                       std::min(param.maxValue, param.defaultValue * 1.5f));
                        }
                    }
                    break;
                    
                case 2: // Subtle

                    for (const auto& param : styleParams) {
                        if (param.name.find("Intensity") != std::string::npos ||
                            param.name.find("Strength") != std::string::npos) {
                            renderer->setStyleParameter(param.name, 
                                                       std::max(param.minValue, param.defaultValue * 0.5f));
                        }
                    }
                    break;
                    
                case 3:

                    for (const auto& param : styleParams) {
                        if (param.name.find("Thickness") != std::string::npos ||
                            param.name.find("Size") != std::string::npos) {
                            renderer->setStyleParameter(param.name, 
                                                       std::min(param.maxValue, param.defaultValue * 2.0f));
                        }
                    }
                    break;
                    
                case 4:
                    break;
                    
                default:
                    break;
            }
        }
    }

    ImGui::Spacing();
    if (ImGui::Button("Save Current as Custom")) {
        currentPreset = 4; // Select the "Custom" preset
    }
}

void RenderingSettingsUI::applySelectedStyle() {
    if (!renderer) {
        return;
    }

    if (renderer->setShaderStyleByIndex(selectedStyleIndex)) {
        std::string styleName = renderer->getCurrentStyleName();
        if (onStyleChanged) {
            onStyleChanged(styleName);
        }
    }
    else {
        std::cerr << "Failed to apply style index: " << selectedStyleIndex << std::endl;
    }
}
