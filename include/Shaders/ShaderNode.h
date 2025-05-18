/**
 * @file ShaderNode.h
 * @brief Node-based shader system for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_SHADER_NODE_H
#define ELEMENTAL_RENDERER_SHADER_NODE_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <glm/glm.hpp>

namespace ElementalRenderer {

// Forward declarations
class ShaderGraph;
class ShaderNode;

/**
 * @brief Represents a connection between shader nodes
 */
struct NodeConnection {
    std::shared_ptr<ShaderNode> sourceNode;
    int sourceOutputIndex;
    std::shared_ptr<ShaderNode> targetNode;
    int targetInputIndex;
};

/**
 * @brief Represents a pin (input or output) on a shader node
 */
struct NodePin {
    enum class Type {
        FLOAT,
        VEC2,
        VEC3,
        VEC4,
        INT,
        BOOL,
        SAMPLER2D,
        MATRIX
    };

    std::string name;
    Type type;
    std::string defaultValue;
    bool isConnected;
};

/**
 * @brief Base class for all shader nodes
 */
class ShaderNode {
public:
    /**
     * @brief Constructor
     * @param name Node name
     * @param category Node category
     */
    ShaderNode(const std::string& name, const std::string& category);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~ShaderNode() = default;
    
    /**
     * @brief Get the node name
     * @return Node name
     */
    const std::string& getName() const;
    
    /**
     * @brief Get the node category
     * @return Node category
     */
    const std::string& getCategory() const;
    
    /**
     * @brief Get the node position in the editor
     * @return Node position
     */
    const glm::vec2& getPosition() const;
    
    /**
     * @brief Set the node position in the editor
     * @param position New position
     */
    void setPosition(const glm::vec2& position);
    
    /**
     * @brief Get the node size in the editor
     * @return Node size
     */
    const glm::vec2& getSize() const;
    
    /**
     * @brief Set the node size in the editor
     * @param size New size
     */
    void setSize(const glm::vec2& size);
    
    /**
     * @brief Get the input pins
     * @return Vector of input pins
     */
    const std::vector<NodePin>& getInputPins() const;
    
    /**
     * @brief Get the output pins
     * @return Vector of output pins
     */
    const std::vector<NodePin>& getOutputPins() const;
    
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
    
    /**
     * @brief Set the connection state of an input pin
     * @param index Pin index
     * @param connected Connection state
     */
    void setInputConnected(int index, bool connected);
    
    /**
     * @brief Set the connection state of an output pin
     * @param index Pin index
     * @param connected Connection state
     */
    void setOutputConnected(int index, bool connected);
    
    /**
     * @brief Generate HLSL code for this node
     * @param graph The shader graph this node belongs to
     * @param code Output string to append code to
     * @param outputVariables Map of output variable names
     * @return true if code generation was successful
     */
    virtual bool generateCode(const ShaderGraph* graph, std::string& code, 
                             std::unordered_map<int, std::string>& outputVariables) = 0;
    
    /**
     * @brief Get a unique ID for this node
     * @return Node ID
     */
    uint32_t getId() const;

protected:
    std::string m_name;
    std::string m_category;
    glm::vec2 m_position;
    glm::vec2 m_size;
    std::vector<NodePin> m_inputPins;
    std::vector<NodePin> m_outputPins;
    uint32_t m_id;
    
    static uint32_t s_nextId;
};

/**
 * @brief Node for mathematical operations
 */
class MathNode : public ShaderNode {
public:
    enum class Operation {
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        DOT,
        CROSS,
        NORMALIZE,
        LENGTH,
        POWER,
        MIN,
        MAX,
        ABS,
        SIN,
        COS,
        TAN
    };
    
    /**
     * @brief Constructor
     * @param operation Math operation
     */
    MathNode(Operation operation);
    
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
     * @brief Get the operation
     * @return Operation
     */
    Operation getOperation() const;

private:
    Operation m_operation;
};

/**
 * @brief Node for texture sampling
 */
class TextureSampleNode : public ShaderNode {
public:
    /**
     * @brief Constructor
     */
    TextureSampleNode();
    
    /**
     * @brief Generate HLSL code for this node
     * @param graph The shader graph this node belongs to
     * @param code Output string to append code to
     * @param outputVariables Map of output variable names
     * @return true if code generation was successful
     */
    bool generateCode(const ShaderGraph* graph, std::string& code, 
                     std::unordered_map<int, std::string>& outputVariables) override;
};

/**
 * @brief Node for vector operations
 */
class VectorNode : public ShaderNode {
public:
    /**
     * @brief Constructor
     * @param components Number of components (2, 3, or 4)
     */
    VectorNode(int components);
    
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
     * @brief Get the number of components
     * @return Number of components
     */
    int getComponents() const;

private:
    int m_components;
};

/**
 * @brief Node for input parameters
 */
class InputNode : public ShaderNode {
public:
    enum class InputType {
        POSITION,
        NORMAL,
        UV,
        COLOR,
        TANGENT,
        BITANGENT,
        TIME,
        CAMERA_POSITION,
        CUSTOM
    };
    
    /**
     * @brief Constructor
     * @param inputType Input type
     * @param customName Custom name (for CUSTOM type)
     */
    InputNode(InputType inputType, const std::string& customName = "");
    
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
     * @brief Get the input type
     * @return Input type
     */
    InputType getInputType() const;
    
    /**
     * @brief Get the custom name
     * @return Custom name
     */
    const std::string& getCustomName() const;

private:
    InputType m_inputType;
    std::string m_customName;
};

/**
 * @brief Node for output parameters
 */
class OutputNode : public ShaderNode {
public:
    enum class OutputType {
        COLOR,
        NORMAL,
        EMISSION,
        METALLIC,
        ROUGHNESS,
        AMBIENT_OCCLUSION,
        CUSTOM
    };
    
    /**
     * @brief Constructor
     * @param outputType Output type
     * @param customName Custom name (for CUSTOM type)
     */
    OutputNode(OutputType outputType, const std::string& customName = "");
    
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
     * @brief Get the output type
     * @return Output type
     */
    OutputType getOutputType() const;
    
    /**
     * @brief Get the custom name
     * @return Custom name
     */
    const std::string& getCustomName() const;

private:
    OutputType m_outputType;
    std::string m_customName;
};

/**
 * @brief Factory for creating shader nodes
 */
class ShaderNodeFactory {
public:
    /**
     * @brief Create a math node
     * @param operation Math operation
     * @return Shared pointer to the created node
     */
    static std::shared_ptr<MathNode> createMathNode(MathNode::Operation operation);
    
    /**
     * @brief Create a texture sample node
     * @return Shared pointer to the created node
     */
    static std::shared_ptr<TextureSampleNode> createTextureSampleNode();
    
    /**
     * @brief Create a vector node
     * @param components Number of components
     * @return Shared pointer to the created node
     */
    static std::shared_ptr<VectorNode> createVectorNode(int components);
    
    /**
     * @brief Create an input node
     * @param inputType Input type
     * @param customName Custom name (for CUSTOM type)
     * @return Shared pointer to the created node
     */
    static std::shared_ptr<InputNode> createInputNode(InputNode::InputType inputType, const std::string& customName = "");
    
    /**
     * @brief Create an output node
     * @param outputType Output type
     * @param customName Custom name (for CUSTOM type)
     * @return Shared pointer to the created node
     */
    static std::shared_ptr<OutputNode> createOutputNode(OutputNode::OutputType outputType, const std::string& customName = "");
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_SHADER_NODE_H