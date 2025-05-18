/**
 * @file ShaderEditorManager.cpp
 * @brief Implementation of the shader editor manager
 */

#include "GUI/ShaderEditorManager.h"
#include <iostream>

namespace ElementalRenderer {

ShaderEditorManager& ShaderEditorManager::getInstance() {
    static ShaderEditorManager instance;
    return instance;
}

ShaderEditorManager::ShaderEditorManager()
    : m_isVisible(false) {
}

ShaderEditorManager::~ShaderEditorManager() {
    shutdown();
}

bool ShaderEditorManager::initialize(int width, int height) {

    m_editorUI = std::make_unique<ShaderEditorUI>();

    if (!m_editorUI->initialize(width, height)) {
        std::cerr << "Failed to initialize shader editor UI" << std::endl;
        return false;
    }

    m_editorUI->setOnShaderCompiledCallback([this](bool success, const std::string& message) {
        handleShaderCompiled(success, message);
    });
    
    return true;
}

void ShaderEditorManager::shutdown() {
    m_editorUI.reset();
    m_isVisible = false;
}

void ShaderEditorManager::showEditor(std::shared_ptr<Shader> shader) {
    if (!m_editorUI) {
        return;
    }
    
    m_isVisible = true;

    if (shader) {

        m_editorUI->createNewGraph("Shader Graph");
    }
}

void ShaderEditorManager::hideEditor() {
    m_isVisible = false;
}

bool ShaderEditorManager::isEditorVisible() const {
    return m_isVisible;
}

void ShaderEditorManager::render() {
    if (!m_isVisible || !m_editorUI) {
        return;
    }
    
    m_editorUI->render();
}

bool ShaderEditorManager::processMouseInput(const glm::vec2& mousePos, int button, int action) {
    if (!m_isVisible || !m_editorUI) {
        return false;
    }
    
    return m_editorUI->processMouseInput(mousePos, button, action);
}

bool ShaderEditorManager::processKeyInput(int key, int action) {
    if (!m_isVisible || !m_editorUI) {
        return false;
    }
    
    return m_editorUI->processKeyInput(key, action);
}

bool ShaderEditorManager::processCharInput(unsigned int codepoint) {
    if (!m_isVisible || !m_editorUI) {
        return false;
    }
    
    return m_editorUI->processCharInput(codepoint);
}

std::shared_ptr<Shader> ShaderEditorManager::createShaderFromGraph() {
    if (!m_editorUI) {
        return nullptr;
    }

    std::string vertexCode, fragmentCode;
    if (!m_editorUI->generateShaderCode(vertexCode, fragmentCode)) {
        std::cerr << "Failed to generate shader code" << std::endl;
        return nullptr;
    }

    return createShaderFromCode(vertexCode, fragmentCode);
}

bool ShaderEditorManager::loadGraphFromFile(const std::string& filePath) {
    if (!m_editorUI) {
        return false;
    }
    
    return m_editorUI->loadGraphFromFile(filePath);
}

bool ShaderEditorManager::saveGraphToFile(const std::string& filePath) {
    if (!m_editorUI) {
        return false;
    }
    
    return m_editorUI->saveGraphToFile(filePath);
}

void ShaderEditorManager::setOnShaderCreatedCallback(std::function<void(std::shared_ptr<Shader>)> callback) {
    m_onShaderCreated = callback;
}

void ShaderEditorManager::handleShaderCompiled(bool success, const std::string& message) {
    if (success) {
        std::cout << "Shader compiled successfully: " << message << std::endl;
        auto shader = createShaderFromGraph();
        if (shader && m_onShaderCreated) {
            m_onShaderCreated(shader);
        }
    } else {
        std::cerr << "Shader compilation failed: " << message << std::endl;
    }
}

std::shared_ptr<Shader> ShaderEditorManager::createShaderFromCode(const std::string& vertexCode, const std::string& fragmentCode) {
    auto shader = Shader::createFromSource(vertexCode, fragmentCode);
    
    if (!shader) {
        std::cerr << "Failed to create shader from generated code" << std::endl;
        return nullptr;
    }
    
    return shader;
}

} // namespace ElementalRenderer