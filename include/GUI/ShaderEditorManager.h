/**
 * @file ShaderEditorManager.h
 * @brief Manager for the node-based shader editor
 */

#ifndef ELEMENTAL_RENDERER_SHADER_EDITOR_MANAGER_H
#define ELEMENTAL_RENDERER_SHADER_EDITOR_MANAGER_H

#include "ShaderEditorUI.h"
#include "../Shader.h"
#include <memory>
#include <string>
#include <functional>

namespace ElementalRenderer {

/**
 * @brief Manager class for the shader editor
 * 
 * This class serves as the entry point for the shader editor functionality.
 * It manages the shader editor UI and handles interactions with the rest of the application.
 */
class ShaderEditorManager {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the singleton instance
     */
    static ShaderEditorManager& getInstance();
    
    /**
     * @brief Initialize the shader editor
     * @param width Width of the editor window
     * @param height Height of the editor window
     * @return true if initialization was successful
     */
    bool initialize(int width, int height);
    
    /**
     * @brief Shutdown the shader editor
     */
    void shutdown();
    
    /**
     * @brief Show the shader editor
     * @param shader Shader to edit (optional)
     */
    void showEditor(std::shared_ptr<Shader> shader = nullptr);
    
    /**
     * @brief Hide the shader editor
     */
    void hideEditor();
    
    /**
     * @brief Check if the editor is visible
     * @return true if the editor is visible
     */
    bool isEditorVisible() const;
    
    /**
     * @brief Render the shader editor
     */
    void render();
    
    /**
     * @brief Process mouse input for the editor
     * @param mousePos Mouse position
     * @param button Mouse button
     * @param action Mouse button action (press, release)
     * @return true if the input was handled by the editor
     */
    bool processMouseInput(const glm::vec2& mousePos, int button, int action);
    
    /**
     * @brief Process key input for the editor
     * @param key Key code
     * @param action Key action (press, release)
     * @return true if the input was handled by the editor
     */
    bool processKeyInput(int key, int action);
    
    /**
     * @brief Process character input for the editor
     * @param codepoint Unicode codepoint
     * @return true if the input was handled by the editor
     */
    bool processCharInput(unsigned int codepoint);
    
    /**
     * @brief Create a new shader from the current graph
     * @return Shared pointer to the created shader, or nullptr if creation failed
     */
    std::shared_ptr<Shader> createShaderFromGraph();
    
    /**
     * @brief Load a shader graph from a file
     * @param filePath Path to the file
     * @return true if the graph was loaded successfully
     */
    bool loadGraphFromFile(const std::string& filePath);
    
    /**
     * @brief Save the current shader graph to a file
     * @param filePath Path to save the file
     * @return true if the graph was saved successfully
     */
    bool saveGraphToFile(const std::string& filePath);
    
    /**
     * @brief Set the callback for when a shader is created
     * @param callback Function to call when a shader is created
     */
    void setOnShaderCreatedCallback(std::function<void(std::shared_ptr<Shader>)> callback);

private:
    /**
     * @brief Private constructor (singleton)
     */
    ShaderEditorManager();
    
    /**
     * @brief Private destructor (singleton)
     */
    ~ShaderEditorManager();
    
    /**
     * @brief Deleted copy constructor (singleton)
     */
    ShaderEditorManager(const ShaderEditorManager&) = delete;
    
    /**
     * @brief Deleted assignment operator (singleton)
     */
    ShaderEditorManager& operator=(const ShaderEditorManager&) = delete;
    
    /**
     * @brief Handle shader compilation result
     * @param success true if compilation was successful
     * @param message Compilation message
     */
    void handleShaderCompiled(bool success, const std::string& message);
    
    /**
     * @brief Create a shader from vertex and fragment code
     * @param vertexCode Vertex shader code
     * @param fragmentCode Fragment shader code
     * @return Shared pointer to the created shader, or nullptr if creation failed
     */
    std::shared_ptr<Shader> createShaderFromCode(const std::string& vertexCode, const std::string& fragmentCode);
    
    // Shader editor UI
    std::unique_ptr<ShaderEditorUI> m_editorUI;
    
    // Editor state
    bool m_isVisible;
    
    // Callbacks
    std::function<void(std::shared_ptr<Shader>)> m_onShaderCreated;
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_SHADER_EDITOR_MANAGER_H