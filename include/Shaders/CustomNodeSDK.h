/**
 * @file CustomNodeSDK.h
 * @brief SDK for creating custom shader nodes in ElementalRenderer
 */

#ifndef ELEMENTAL_RENDERER_CUSTOM_NODE_SDK_H
#define ELEMENTAL_RENDERER_CUSTOM_NODE_SDK_H

#include "ShaderNode.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <any>
#include <filesystem>

namespace ElementalRenderer {

/**
 * @brief Interface for custom shader node definitions
 */
class ICustomNodeDefinition {
public:
    virtual ~ICustomNodeDefinition() = default;
    
    /**
     * @brief Get the name of the node
     * @return Node name
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief Get the category of the node
     * @return Node category
     */
    virtual std::string getCategory() const = 0;
    
    /**
     * @brief Get the description of the node
     * @return Node description
     */
    virtual std::string getDescription() const = 0;
    
    /**
     * @brief Get the input pins of the node
     * @return Vector of input pins
     */
    virtual std::vector<NodePin> getInputPins() const = 0;
    
    /**
     * @brief Get the output pins of the node
     * @return Vector of output pins
     */
    virtual std::vector<NodePin> getOutputPins() const = 0;
    
    /**
     * @brief Generate HLSL code for this node
     * @param graph The shader graph this node belongs to
     * @param code Output string to append code to
     * @param outputVariables Map of output variable names
     * @param inputVariables Map of input variable names
     * @return true if code generation was successful
     */
    virtual bool generateCode(const ShaderGraph* graph, std::string& code,
                             std::unordered_map<int, std::string>& outputVariables,
                             const std::unordered_map<int, std::string>& inputVariables) const = 0;
};

/**
 * @brief Custom shader node implementation
 */
class CustomShaderNode : public ShaderNode {
public:
    /**
     * @brief Constructor
     * @param definition Custom node definition
     */
    CustomShaderNode(std::shared_ptr<ICustomNodeDefinition> definition);
    
    /**
     * @brief Generate HLSL code for this node
     * @param graph The shader graph this node belongs to
     * @param code Output string to append code to
     * @param outputVariables Map of output variable names
     * @return true if code generation was successful
     */
    bool generateCode(const ShaderGraph* graph, std::string& code,
                     std::unordered_map<int, std::string>& outputVariables) override;
    
    /**
     * @brief Get the custom node definition
     * @return Custom node definition
     */
    std::shared_ptr<ICustomNodeDefinition> getDefinition() const;

private:
    std::shared_ptr<ICustomNodeDefinition> m_definition;
};

/**
 * @brief Base class for C++ custom node definitions
 */
class CustomNodeDefinition : public ICustomNodeDefinition {
public:
    /**
     * @brief Constructor
     * @param name Node name
     * @param category Node category
     * @param description Node description
     */
    CustomNodeDefinition(const std::string& name, const std::string& category, const std::string& description);
    
    /**
     * @brief Get the name of the node
     * @return Node name
     */
    std::string getName() const override;
    
    /**
     * @brief Get the category of the node
     * @return Node category
     */
    std::string getCategory() const override;
    
    /**
     * @brief Get the description of the node
     * @return Node description
     */
    std::string getDescription() const override;
    
    /**
     * @brief Get the input pins of the node
     * @return Vector of input pins
     */
    std::vector<NodePin> getInputPins() const override;
    
    /**
     * @brief Get the output pins of the node
     * @return Vector of output pins
     */
    std::vector<NodePin> getOutputPins() const override;
    
    /**
     * @brief Add an input pin
     * @param name Pin name
     * @param type Pin type
     * @param defaultValue Default value
     */
    void addInputPin(const std::string& name, NodePin::Type type, const std::string& defaultValue = "");
    
    /**
     * @brief Add an output pin
     * @param name Pin name
     * @param type Pin type
     */
    void addOutputPin(const std::string& name, NodePin::Type type);

protected:
    std::string m_name;
    std::string m_category;
    std::string m_description;
    std::vector<NodePin> m_inputPins;
    std::vector<NodePin> m_outputPins;
};

/**
 * @brief JSON-based custom node definition
 */
class JsonNodeDefinition : public ICustomNodeDefinition {
public:
    /**
     * @brief Constructor
     * @param jsonPath Path to JSON file
     */
    JsonNodeDefinition(const std::string& jsonPath);
    
    /**
     * @brief Get the name of the node
     * @return Node name
     */
    std::string getName() const override;
    
    /**
     * @brief Get the category of the node
     * @return Node category
     */
    std::string getCategory() const override;
    
    /**
     * @brief Get the description of the node
     * @return Node description
     */
    std::string getDescription() const override;
    
    /**
     * @brief Get the input pins of the node
     * @return Vector of input pins
     */
    std::vector<NodePin> getInputPins() const override;
    
    /**
     * @brief Get the output pins of the node
     * @return Vector of output pins
     */
    std::vector<NodePin> getOutputPins() const override;
    
    /**
     * @brief Generate HLSL code for this node
     * @param graph The shader graph this node belongs to
     * @param code Output string to append code to
     * @param outputVariables Map of output variable names
     * @param inputVariables Map of input variable names
     * @return true if code generation was successful
     */
    bool generateCode(const ShaderGraph* graph, std::string& code,
                     std::unordered_map<int, std::string>& outputVariables,
                     const std::unordered_map<int, std::string>& inputVariables) const override;
    
    /**
     * @brief Check if the JSON file has been modified
     * @return true if the file has been modified since last load
     */
    bool hasFileChanged() const;
    
    /**
     * @brief Reload the JSON file
     * @return true if reload was successful
     */
    bool reload();

private:
    std::string m_jsonPath;
    std::string m_name;
    std::string m_category;
    std::string m_description;
    std::vector<NodePin> m_inputPins;
    std::vector<NodePin> m_outputPins;
    std::string m_codeTemplate;
    std::filesystem::file_time_type m_lastModified;
    
    bool parseJson(const std::string& jsonContent);
};

/**
 * @brief Manager for custom shader nodes
 */
class CustomNodeManager {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the singleton instance
     */
    static CustomNodeManager& getInstance();
    
    /**
     * @brief Register a custom node definition
     * @param definition Custom node definition
     * @return true if registration was successful
     */
    bool registerNodeDefinition(std::shared_ptr<ICustomNodeDefinition> definition);
    
    /**
     * @brief Unregister a custom node definition
     * @param name Node name
     * @return true if unregistration was successful
     */
    bool unregisterNodeDefinition(const std::string& name);
    
    /**
     * @brief Get a custom node definition by name
     * @param name Node name
     * @return Custom node definition, or nullptr if not found
     */
    std::shared_ptr<ICustomNodeDefinition> getNodeDefinition(const std::string& name) const;
    
    /**
     * @brief Get all registered custom node definitions
     * @return Map of node names to definitions
     */
    const std::unordered_map<std::string, std::shared_ptr<ICustomNodeDefinition>>& getNodeDefinitions() const;
    
    /**
     * @brief Create a custom shader node
     * @param name Node name
     * @return Custom shader node, or nullptr if definition not found
     */
    std::shared_ptr<CustomShaderNode> createNode(const std::string& name) const;
    
    /**
     * @brief Load all JSON node definitions from a directory
     * @param directory Directory path
     * @param recursive Whether to search subdirectories
     * @return Number of definitions loaded
     */
    int loadFromDirectory(const std::string& directory, bool recursive = false);
    
    /**
     * @brief Check for changes in JSON node definitions and reload if necessary
     * @return Number of definitions reloaded
     */
    int checkForChanges();
    
    /**
     * @brief Enable or disable hot reloading
     * @param enable Whether to enable hot reloading
     */
    void setHotReloadingEnabled(bool enable);
    
    /**
     * @brief Check if hot reloading is enabled
     * @return true if hot reloading is enabled
     */
    bool isHotReloadingEnabled() const;

private:
    CustomNodeManager();
    ~CustomNodeManager();
    
    std::unordered_map<std::string, std::shared_ptr<ICustomNodeDefinition>> m_nodeDefinitions;
    bool m_hotReloadingEnabled;
};

/**
 * @brief Macro for defining a custom node class
 */
#define DEFINE_CUSTOM_NODE(NodeClass, NodeName, NodeCategory, NodeDescription) \
class NodeClass : public ElementalRenderer::CustomNodeDefinition { \
public: \
    NodeClass() : ElementalRenderer::CustomNodeDefinition(NodeName, NodeCategory, NodeDescription) { \
        setupNode(); \
    } \
    \
    bool generateCode(const ElementalRenderer::ShaderGraph* graph, std::string& code, \
                     std::unordered_map<int, std::string>& outputVariables, \
                     const std::unordered_map<int, std::string>& inputVariables) const override; \
    \
private: \
    void setupNode(); \
}; \
\
bool NodeClass::generateCode(const ElementalRenderer::ShaderGraph* graph, std::string& code, \
                           std::unordered_map<int, std::string>& outputVariables, \
                           const std::unordered_map<int, std::string>& inputVariables) const

/**
 * @brief Macro for registering a custom node
 */
#define REGISTER_CUSTOM_NODE(NodeClass) \
    ElementalRenderer::CustomNodeManager::getInstance().registerNodeDefinition(std::make_shared<NodeClass>())

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_CUSTOM_NODE_SDK_H