/**
 * @file ShaderNode.cpp
 * @brief Implementation of the shader node system
 */

#include "Shaders/ShaderNode.h"
#include "Shaders/ShaderGraph.h"
#include <sstream>
#include <iostream>

namespace ElementalRenderer {

// Initialize static member
uint32_t ShaderNode::s_nextId = 1;

// ShaderNode implementation
ShaderNode::ShaderNode(const std::string& name, const std::string& category)
    : m_name(name), m_category(category), m_position(0.0f, 0.0f), m_size(200.0f, 150.0f), m_id(s_nextId++) {
}

const std::string& ShaderNode::getName() const {
    return m_name;
}

const std::string& ShaderNode::getCategory() const {
    return m_category;
}

const glm::vec2& ShaderNode::getPosition() const {
    return m_position;
}

void ShaderNode::setPosition(const glm::vec2& position) {
    m_position = position;
}

const glm::vec2& ShaderNode::getSize() const {
    return m_size;
}

void ShaderNode::setSize(const glm::vec2& size) {
    m_size = size;
}

const std::vector<NodePin>& ShaderNode::getInputPins() const {
    return m_inputPins;
}

const std::vector<NodePin>& ShaderNode::getOutputPins() const {
    return m_outputPins;
}

void ShaderNode::addInputPin(const std::string& name, NodePin::Type type, const std::string& defaultValue) {
    NodePin pin;
    pin.name = name;
    pin.type = type;
    pin.defaultValue = defaultValue;
    pin.isConnected = false;
    m_inputPins.push_back(pin);
}

void ShaderNode::addOutputPin(const std::string& name, NodePin::Type type) {
    NodePin pin;
    pin.name = name;
    pin.type = type;
    pin.defaultValue = "";
    pin.isConnected = false;
    m_outputPins.push_back(pin);
}

void ShaderNode::setInputConnected(int index, bool connected) {
    if (index >= 0 && index < m_inputPins.size()) {
        m_inputPins[index].isConnected = connected;
    }
}

void ShaderNode::setOutputConnected(int index, bool connected) {
    if (index >= 0 && index < m_outputPins.size()) {
        m_outputPins[index].isConnected = connected;
    }
}

uint32_t ShaderNode::getId() const {
    return m_id;
}

// MathNode implementation
MathNode::MathNode(Operation operation)
    : ShaderNode("Math", "Math"), m_operation(operation) {
    
    // Configure node based on operation
    switch (operation) {
        case Operation::ADD:
            m_name = "Add";
            addInputPin("A", NodePin::Type::FLOAT, "0.0");
            addInputPin("B", NodePin::Type::FLOAT, "0.0");
            addOutputPin("Result", NodePin::Type::FLOAT);
            break;
        case Operation::SUBTRACT:
            m_name = "Subtract";
            addInputPin("A", NodePin::Type::FLOAT, "0.0");
            addInputPin("B", NodePin::Type::FLOAT, "0.0");
            addOutputPin("Result", NodePin::Type::FLOAT);
            break;
        case Operation::MULTIPLY:
            m_name = "Multiply";
            addInputPin("A", NodePin::Type::FLOAT, "0.0");
            addInputPin("B", NodePin::Type::FLOAT, "0.0");
            addOutputPin("Result", NodePin::Type::FLOAT);
            break;
        case Operation::DIVIDE:
            m_name = "Divide";
            addInputPin("A", NodePin::Type::FLOAT, "0.0");
            addInputPin("B", NodePin::Type::FLOAT, "1.0");
            addOutputPin("Result", NodePin::Type::FLOAT);
            break;
        case Operation::DOT:
            m_name = "Dot Product";
            addInputPin("A", NodePin::Type::VEC3, "float3(0,0,0)");
            addInputPin("B", NodePin::Type::VEC3, "float3(0,0,0)");
            addOutputPin("Result", NodePin::Type::FLOAT);
            break;
        case Operation::CROSS:
            m_name = "Cross Product";
            addInputPin("A", NodePin::Type::VEC3, "float3(0,0,0)");
            addInputPin("B", NodePin::Type::VEC3, "float3(0,0,0)");
            addOutputPin("Result", NodePin::Type::VEC3);
            break;
        case Operation::NORMALIZE:
            m_name = "Normalize";
            addInputPin("Vector", NodePin::Type::VEC3, "float3(0,0,0)");
            addOutputPin("Result", NodePin::Type::VEC3);
            break;
        case Operation::LENGTH:
            m_name = "Length";
            addInputPin("Vector", NodePin::Type::VEC3, "float3(0,0,0)");
            addOutputPin("Result", NodePin::Type::FLOAT);
            break;
        case Operation::POWER:
            m_name = "Power";
            addInputPin("Base", NodePin::Type::FLOAT, "0.0");
            addInputPin("Exponent", NodePin::Type::FLOAT, "1.0");
            addOutputPin("Result", NodePin::Type::FLOAT);
            break;
        case Operation::MIN:
            m_name = "Min";
            addInputPin("A", NodePin::Type::FLOAT, "0.0");
            addInputPin("B", NodePin::Type::FLOAT, "0.0");
            addOutputPin("Result", NodePin::Type::FLOAT);
            break;
        case Operation::MAX:
            m_name = "Max";
            addInputPin("A", NodePin::Type::FLOAT, "0.0");
            addInputPin("B", NodePin::Type::FLOAT, "0.0");
            addOutputPin("Result", NodePin::Type::FLOAT);
            break;
        case Operation::ABS:
            m_name = "Absolute";
            addInputPin("Value", NodePin::Type::FLOAT, "0.0");
            addOutputPin("Result", NodePin::Type::FLOAT);
            break;
        case Operation::SIN:
            m_name = "Sine";
            addInputPin("Value", NodePin::Type::FLOAT, "0.0");
            addOutputPin("Result", NodePin::Type::FLOAT);
            break;
        case Operation::COS:
            m_name = "Cosine";
            addInputPin("Value", NodePin::Type::FLOAT, "0.0");
            addOutputPin("Result", NodePin::Type::FLOAT);
            break;
        case Operation::TAN:
            m_name = "Tangent";
            addInputPin("Value", NodePin::Type::FLOAT, "0.0");
            addOutputPin("Result", NodePin::Type::FLOAT);
            break;
    }
}

bool MathNode::generateCode(const ShaderGraph* graph, std::string& code, 
                           std::unordered_map<int, std::string>& outputVariables) {
    std::stringstream ss;
    std::string varName = "math_" + std::to_string(m_id);

    std::vector<std::string> inputVars;
    for (int i = 0; i < m_inputPins.size(); ++i) {
        std::shared_ptr<ShaderNode> sourceNode;
        int sourceOutputIndex;
        
        if (m_inputPins[i].isConnected && graph->getConnectionSource(
            std::dynamic_pointer_cast<ShaderNode>(shared_from_this()), i, sourceNode, sourceOutputIndex)) {
            inputVars.push_back(outputVariables[sourceNode->getId() * 1000 + sourceOutputIndex]);
        } else {
            inputVars.push_back(m_inputPins[i].defaultValue);
        }
    }

    switch (m_operation) {
        case Operation::ADD:
            ss << "float " << varName << " = " << inputVars[0] << " + " << inputVars[1] << ";\n";
            break;
        case Operation::SUBTRACT:
            ss << "float " << varName << " = " << inputVars[0] << " - " << inputVars[1] << ";\n";
            break;
        case Operation::MULTIPLY:
            ss << "float " << varName << " = " << inputVars[0] << " * " << inputVars[1] << ";\n";
            break;
        case Operation::DIVIDE:
            ss << "float " << varName << " = " << inputVars[0] << " / " << inputVars[1] << ";\n";
            break;
        case Operation::DOT:
            ss << "float " << varName << " = dot(" << inputVars[0] << ", " << inputVars[1] << ");\n";
            break;
        case Operation::CROSS:
            ss << "float3 " << varName << " = cross(" << inputVars[0] << ", " << inputVars[1] << ");\n";
            break;
        case Operation::NORMALIZE:
            ss << "float3 " << varName << " = normalize(" << inputVars[0] << ");\n";
            break;
        case Operation::LENGTH:
            ss << "float " << varName << " = length(" << inputVars[0] << ");\n";
            break;
        case Operation::POWER:
            ss << "float " << varName << " = pow(" << inputVars[0] << ", " << inputVars[1] << ");\n";
            break;
        case Operation::MIN:
            ss << "float " << varName << " = min(" << inputVars[0] << ", " << inputVars[1] << ");\n";
            break;
        case Operation::MAX:
            ss << "float " << varName << " = max(" << inputVars[0] << ", " << inputVars[1] << ");\n";
            break;
        case Operation::ABS:
            ss << "float " << varName << " = abs(" << inputVars[0] << ");\n";
            break;
        case Operation::SIN:
            ss << "float " << varName << " = sin(" << inputVars[0] << ");\n";
            break;
        case Operation::COS:
            ss << "float " << varName << " = cos(" << inputVars[0] << ");\n";
            break;
        case Operation::TAN:
            ss << "float " << varName << " = tan(" << inputVars[0] << ");\n";
            break;
    }

    outputVariables[m_id * 1000] = varName;
    
    code += ss.str();
    return true;
}

MathNode::Operation MathNode::getOperation() const {
    return m_operation;
}

TextureSampleNode::TextureSampleNode()
    : ShaderNode("Texture Sample", "Texture") {
    
    addInputPin("Texture", NodePin::Type::SAMPLER2D, "");
    addInputPin("UV", NodePin::Type::VEC2, "input.texCoord");
    addOutputPin("RGBA", NodePin::Type::VEC4);
    addOutputPin("RGB", NodePin::Type::VEC3);
    addOutputPin("R", NodePin::Type::FLOAT);
    addOutputPin("G", NodePin::Type::FLOAT);
    addOutputPin("B", NodePin::Type::FLOAT);
    addOutputPin("A", NodePin::Type::FLOAT);
}

bool TextureSampleNode::generateCode(const ShaderGraph* graph, std::string& code, 
                                    std::unordered_map<int, std::string>& outputVariables) {
    std::stringstream ss;
    std::string varName = "texSample_" + std::to_string(m_id);

    std::vector<std::string> inputVars;
    for (int i = 0; i < m_inputPins.size(); ++i) {
        std::shared_ptr<ShaderNode> sourceNode;
        int sourceOutputIndex;
        
        if (m_inputPins[i].isConnected && graph->getConnectionSource(
            std::dynamic_pointer_cast<ShaderNode>(shared_from_this()), i, sourceNode, sourceOutputIndex)) {

            inputVars.push_back(outputVariables[sourceNode->getId() * 1000 + sourceOutputIndex]);
        } else {

            inputVars.push_back(m_inputPins[i].defaultValue);
        }
    }

    ss << "float4 " << varName << " = " << inputVars[0] << ".Sample(" << inputVars[0] << "Sampler, " << inputVars[1] << ");\n";

    outputVariables[m_id * 1000 + 0] = varName;
    outputVariables[m_id * 1000 + 1] = varName + ".rgb";
    outputVariables[m_id * 1000 + 2] = varName + ".r";
    outputVariables[m_id * 1000 + 3] = varName + ".g";
    outputVariables[m_id * 1000 + 4] = varName + ".b";
    outputVariables[m_id * 1000 + 5] = varName + ".a";
    
    code += ss.str();
    return true;
}

VectorNode::VectorNode(int components)
    : ShaderNode("Vector", "Math"), m_components(components) {
    
    m_name = "Vector" + std::to_string(components);

    for (int i = 0; i < components; ++i) {
        std::string pinName(1, 'X' + i); // X, Y, Z, W
        addInputPin(pinName, NodePin::Type::FLOAT, "0.0");
    }

    NodePin::Type outputType;
    switch (components) {
        case 2: outputType = NodePin::Type::VEC2; break;
        case 3: outputType = NodePin::Type::VEC3; break;
        case 4: outputType = NodePin::Type::VEC4; break;
        default: outputType = NodePin::Type::FLOAT; break;
    }
    
    addOutputPin("Vector", outputType);
}

bool VectorNode::generateCode(const ShaderGraph* graph, std::string& code, 
                             std::unordered_map<int, std::string>& outputVariables) {
    std::stringstream ss;
    std::string varName = "vector_" + std::to_string(m_id);

    std::vector<std::string> inputVars;
    for (int i = 0; i < m_inputPins.size(); ++i) {
        std::shared_ptr<ShaderNode> sourceNode;
        int sourceOutputIndex;
        
        if (m_inputPins[i].isConnected && graph->getConnectionSource(
            std::dynamic_pointer_cast<ShaderNode>(shared_from_this()), i, sourceNode, sourceOutputIndex)) {

            inputVars.push_back(outputVariables[sourceNode->getId() * 1000 + sourceOutputIndex]);
        } else {

            inputVars.push_back(m_inputPins[i].defaultValue);
        }
    }

    ss << "float" << m_components << " " << varName << " = float" << m_components << "(";
    for (int i = 0; i < m_components; ++i) {
        ss << inputVars[i];
        if (i < m_components - 1) {
            ss << ", ";
        }
    }
    ss << ");\n";

    outputVariables[m_id * 1000] = varName;
    
    code += ss.str();
    return true;
}

int VectorNode::getComponents() const {
    return m_components;
}

InputNode::InputNode(InputType inputType, const std::string& customName)
    : ShaderNode("Input", "Input"), m_inputType(inputType), m_customName(customName) {

    switch (inputType) {
        case InputType::POSITION:
            m_name = "Position";
            addOutputPin("Position", NodePin::Type::VEC3);
            break;
        case InputType::NORMAL:
            m_name = "Normal";
            addOutputPin("Normal", NodePin::Type::VEC3);
            break;
        case InputType::UV:
            m_name = "UV";
            addOutputPin("UV", NodePin::Type::VEC2);
            break;
        case InputType::COLOR:
            m_name = "Vertex Color";
            addOutputPin("Color", NodePin::Type::VEC4);
            break;
        case InputType::TANGENT:
            m_name = "Tangent";
            addOutputPin("Tangent", NodePin::Type::VEC3);
            break;
        case InputType::BITANGENT:
            m_name = "Bitangent";
            addOutputPin("Bitangent", NodePin::Type::VEC3);
            break;
        case InputType::TIME:
            m_name = "Time";
            addOutputPin("Time", NodePin::Type::FLOAT);
            break;
        case InputType::CAMERA_POSITION:
            m_name = "Camera Position";
            addOutputPin("Position", NodePin::Type::VEC3);
            break;
        case InputType::CUSTOM:
            m_name = customName;
            addOutputPin(customName, NodePin::Type::FLOAT);
            break;
    }
}

bool InputNode::generateCode(const ShaderGraph* graph, std::string& code, 
                            std::unordered_map<int, std::string>& outputVariables) {
    std::string varName;

    switch (m_inputType) {
        case InputType::POSITION:
            varName = "input.position";
            break;
        case InputType::NORMAL:
            varName = "input.normal";
            break;
        case InputType::UV:
            varName = "input.texCoord";
            break;
        case InputType::COLOR:
            varName = "input.color";
            break;
        case InputType::TANGENT:
            varName = "input.tangent";
            break;
        case InputType::BITANGENT:
            varName = "input.bitangent";
            break;
        case InputType::TIME:
            varName = "Time";
            break;
        case InputType::CAMERA_POSITION:
            varName = "CameraPosition";
            break;
        case InputType::CUSTOM:
            varName = m_customName;
            break;
    }

    outputVariables[m_id * 1000] = varName;
    
    return true;
}

InputNode::InputType InputNode::getInputType() const {
    return m_inputType;
}

const std::string& InputNode::getCustomName() const {
    return m_customName;
}

OutputNode::OutputNode(OutputType outputType, const std::string& customName)
    : ShaderNode("Output", "Output"), m_outputType(outputType), m_customName(customName) {

    switch (outputType) {
        case OutputType::COLOR:
            m_name = "Color Output";
            addInputPin("Color", NodePin::Type::VEC3, "float3(0,0,0)");
            break;
        case OutputType::NORMAL:
            m_name = "Normal Output";
            addInputPin("Normal", NodePin::Type::VEC3, "float3(0,0,1)");
            break;
        case OutputType::EMISSION:
            m_name = "Emission Output";
            addInputPin("Emission", NodePin::Type::VEC3, "float3(0,0,0)");
            break;
        case OutputType::METALLIC:
            m_name = "Metallic Output";
            addInputPin("Metallic", NodePin::Type::FLOAT, "0.0");
            break;
        case OutputType::ROUGHNESS:
            m_name = "Roughness Output";
            addInputPin("Roughness", NodePin::Type::FLOAT, "0.5");
            break;
        case OutputType::AMBIENT_OCCLUSION:
            m_name = "AO Output";
            addInputPin("AO", NodePin::Type::FLOAT, "1.0");
            break;
        case OutputType::CUSTOM:
            m_name = customName + " Output";
            addInputPin(customName, NodePin::Type::FLOAT, "0.0");
            break;
    }
}

bool OutputNode::generateCode(const ShaderGraph* graph, std::string& code, 
                             std::unordered_map<int, std::string>& outputVariables) {
    std::stringstream ss;

    std::string inputVar;
    if (m_inputPins[0].isConnected) {
        std::shared_ptr<ShaderNode> sourceNode;
        int sourceOutputIndex;
        
        if (graph->getConnectionSource(
            std::dynamic_pointer_cast<ShaderNode>(shared_from_this()), 0, sourceNode, sourceOutputIndex)) {
            inputVar = outputVariables[sourceNode->getId() * 1000 + sourceOutputIndex];
        }
    } else {
        inputVar = m_inputPins[0].defaultValue;
    }

    switch (m_outputType) {
        case OutputType::COLOR:
            ss << "output.color = " << inputVar << ";\n";
            break;
        case OutputType::NORMAL:
            ss << "output.normal = " << inputVar << ";\n";
            break;
        case OutputType::EMISSION:
            ss << "output.emission = " << inputVar << ";\n";
            break;
        case OutputType::METALLIC:
            ss << "output.metallic = " << inputVar << ";\n";
            break;
        case OutputType::ROUGHNESS:
            ss << "output.roughness = " << inputVar << ";\n";
            break;
        case OutputType::AMBIENT_OCCLUSION:
            ss << "output.ao = " << inputVar << ";\n";
            break;
        case OutputType::CUSTOM:
            ss << "output." << m_customName << " = " << inputVar << ";\n";
            break;
    }
    
    code += ss.str();
    return true;
}

OutputNode::OutputType OutputNode::getOutputType() const {
    return m_outputType;
}

const std::string& OutputNode::getCustomName() const {
    return m_customName;
}

std::shared_ptr<MathNode> ShaderNodeFactory::createMathNode(MathNode::Operation operation) {
    return std::make_shared<MathNode>(operation);
}

std::shared_ptr<TextureSampleNode> ShaderNodeFactory::createTextureSampleNode() {
    return std::make_shared<TextureSampleNode>();
}

std::shared_ptr<VectorNode> ShaderNodeFactory::createVectorNode(int components) {
    return std::make_shared<VectorNode>(components);
}

std::shared_ptr<InputNode> ShaderNodeFactory::createInputNode(InputNode::InputType inputType, const std::string& customName) {
    return std::make_shared<InputNode>(inputType, customName);
}

std::shared_ptr<OutputNode> ShaderNodeFactory::createOutputNode(OutputNode::OutputType outputType, const std::string& customName) {
    return std::make_shared<OutputNode>(outputType, customName);
}

} // namespace ElementalRenderer