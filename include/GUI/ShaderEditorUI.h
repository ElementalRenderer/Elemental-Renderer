/**
 * @file ShaderEditorUI.h
 * @brief Node-based shader editor UI for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_SHADER_EDITOR_UI_H
#define ELEMENTAL_RENDERER_SHADER_EDITOR_UI_H

#include "ElementalGUI.h"
#include "../Shaders/ShaderGraph.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace ElementalRenderer {

/**
 * @brief UI class for the node-based shader editor
 */
class ShaderEditorUI {
public:
    /**
     * @brief Constructor
     */
    ShaderEditorUI();
    
    /**
     * @brief Destructor
     */
    ~ShaderEditorUI();
    
    /**
     * @brief Initialize the shader editor UI
     * @param width Width of the editor window
     * @param height Height of the editor window
     * @return true if initialization was successful
     */
    bool initialize(int width, int height);
    
    /**
     * @brief Render the shader editor UI
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
     * @brief Set the current shader graph
     * @param graph Shader graph to edit
     */
    void setShaderGraph(std::shared_ptr<ShaderGraph> graph);
    
    /**
     * @brief Get the current shader graph
     * @return Current shader graph
     */
    std::shared_ptr<ShaderGraph> getShaderGraph() const;
    
    /**
     * @brief Create a new shader graph
     * @param name Name of the new graph
     */
    void createNewGraph(const std::string& name);
    
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
     * @brief Generate shader code from the current graph
     * @param vertexCode Output parameter for vertex shader code
     * @param fragmentCode Output parameter for fragment shader code
     * @return true if code generation was successful
     */
    bool generateShaderCode(std::string& vertexCode, std::string& fragmentCode);
    
    /**
     * @brief Compile and test the current shader
     * @return true if compilation was successful
     */
    bool compileAndTestShader();
    
    /**
     * @brief Set the callback for when a shader is compiled
     * @param callback Function to call when a shader is compiled
     */
    void setOnShaderCompiledCallback(std::function<void(bool, const std::string&)> callback);

private:
    // UI elements
    std::shared_ptr<Panel> m_mainPanel;
    std::shared_ptr<Panel> m_nodePanel;
    std::shared_ptr<Panel> m_propertiesPanel;
    std::shared_ptr<Panel> m_codePanel;
    std::shared_ptr<TextInput> m_codeEditor;
    std::shared_ptr<Button> m_compileButton;
    std::shared_ptr<Button> m_saveButton;
    std::shared_ptr<Button> m_loadButton;
    std::shared_ptr<Button> m_newButton;
    std::shared_ptr<Checkbox> m_showCodeCheckbox;
    
    // Node visualization
    struct NodeUI {
        std::shared_ptr<Panel> panel;
        std::vector<std::shared_ptr<GUIElement>> inputPins;
        std::vector<std::shared_ptr<GUIElement>> outputPins;
        uint32_t nodeId;
    };
    
    std::unordered_map<uint32_t, NodeUI> m_nodeUIs;
    
    // Connection visualization
    struct ConnectionUI {
        uint32_t sourceNodeId;
        int sourceOutputIndex;
        uint32_t targetNodeId;
        int targetInputIndex;
    };
    
    std::vector<ConnectionUI> m_connectionUIs;
    
    // Dragging state
    bool m_draggingNode;
    uint32_t m_draggedNodeId;
    glm::vec2 m_dragOffset;
    
    // Connection creation state
    bool m_creatingConnection;
    uint32_t m_connectionSourceNodeId;
    int m_connectionSourceOutputIndex;
    glm::vec2 m_connectionEndPoint;
    
    // Current shader graph
    std::shared_ptr<ShaderGraph> m_shaderGraph;
    
    // Selected node
    uint32_t m_selectedNodeId;
    
    // Code view/edit mode
    bool m_codeViewMode;
    
    // Callbacks
    std::function<void(bool, const std::string&)> m_onShaderCompiled;
    
    // Helper methods
    void createMainUI();
    void updateNodeUI();
    void renderConnections();
    void renderNodeCreationMenu();
    void renderPropertiesPanel();
    void updateCodeEditor();
    
    NodeUI createNodeUI(std::shared_ptr<ShaderNode> node);
    void updateNodeUIPositions();
    void handleNodeSelection(uint32_t nodeId);
    void handlePinClick(uint32_t nodeId, int pinIndex, bool isOutput);
    void completeConnectionCreation(uint32_t targetNodeId, int targetInputIndex);
    void cancelConnectionCreation();
    
    void addNode(const std::string& nodeType, const glm::vec2& position);
    void removeSelectedNode();
    void duplicateSelectedNode();
    
    std::string getNodeTypeDisplayName(const std::string& nodeType);
    glm::vec4 getNodeColor(const std::string& category);
    glm::vec4 getPinColor(NodePin::Type type);
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_SHADER_EDITOR_UI_H