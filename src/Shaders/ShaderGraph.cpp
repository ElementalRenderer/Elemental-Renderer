/**
 * @file ShaderGraph.cpp
 * @brief Implementation of the shader graph system
 */

#include "Shaders/ShaderGraph.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <json/json.hpp> // Using nlohmann/json for serialization

namespace ElementalRenderer {

// Constructor
ShaderGraph::ShaderGraph(const std::string& name)
    : m_name(name) {
}

// Getters and setters
const std::string& ShaderGraph::getName() const {
    return m_name;
}

void ShaderGraph::setName(const std::string& name) {
    m_name = name;
}

// Node management
bool ShaderGraph::addNode(std::shared_ptr<ShaderNode> node) {
    if (!node) {
        return false;
    }
    
    // Check if node already exists
    for (const auto& existingNode : m_nodes) {
        if (existingNode->getId() == node->getId()) {
            return false;
        }
    }
    
    m_nodes.push_back(node);
    return true;
}

bool ShaderGraph::removeNode(std::shared_ptr<ShaderNode> node) {
    if (!node) {
        return false;
    }
    
    return removeNodeById(node->getId());
}

bool ShaderGraph::removeNodeById(uint32_t nodeId) {
    // Remove connections involving this node
    auto connectionIt = m_connections.begin();
    while (connectionIt != m_connections.end()) {
        if (connectionIt->sourceNode->getId() == nodeId || connectionIt->targetNode->getId() == nodeId) {
            // Update connection state on pins
            if (connectionIt->sourceNode->getId() == nodeId) {
                connectionIt->targetNode->setInputConnected(connectionIt->targetInputIndex, false);
            } else {
                connectionIt->sourceNode->setOutputConnected(connectionIt->sourceOutputIndex, false);
            }
            
            connectionIt = m_connections.erase(connectionIt);
        } else {
            ++connectionIt;
        }
    }
    
    // Remove the node
    auto nodeIt = std::find_if(m_nodes.begin(), m_nodes.end(), 
                              [nodeId](const std::shared_ptr<ShaderNode>& node) {
                                  return node->getId() == nodeId;
                              });
    
    if (nodeIt != m_nodes.end()) {
        m_nodes.erase(nodeIt);
        return true;
    }
    
    return false;
}

std::shared_ptr<ShaderNode> ShaderGraph::getNodeById(uint32_t nodeId) const {
    auto it = std::find_if(m_nodes.begin(), m_nodes.end(), 
                          [nodeId](const std::shared_ptr<ShaderNode>& node) {
                              return node->getId() == nodeId;
                          });
    
    if (it != m_nodes.end()) {
        return *it;
    }
    
    return nullptr;
}

const std::vector<std::shared_ptr<ShaderNode>>& ShaderGraph::getNodes() const {
    return m_nodes;
}

// Connection management
bool ShaderGraph::addConnection(std::shared_ptr<ShaderNode> sourceNode, int sourceOutputIndex,
                               std::shared_ptr<ShaderNode> targetNode, int targetInputIndex) {
    if (!sourceNode || !targetNode) {
        return false;
    }
    
    // Check if indices are valid
    if (sourceOutputIndex < 0 || sourceOutputIndex >= sourceNode->getOutputPins().size() ||
        targetInputIndex < 0 || targetInputIndex >= targetNode->getInputPins().size()) {
        return false;
    }
    
    // Check if target input is already connected
    for (const auto& connection : m_connections) {
        if (connection.targetNode->getId() == targetNode->getId() && 
            connection.targetInputIndex == targetInputIndex) {
            return false;
        }
    }
    
    // Create the connection
    NodeConnection connection;
    connection.sourceNode = sourceNode;
    connection.sourceOutputIndex = sourceOutputIndex;
    connection.targetNode = targetNode;
    connection.targetInputIndex = targetInputIndex;
    
    // Update connection state on pins
    sourceNode->setOutputConnected(sourceOutputIndex, true);
    targetNode->setInputConnected(targetInputIndex, true);
    
    m_connections.push_back(connection);
    return true;
}

bool ShaderGraph::removeConnection(std::shared_ptr<ShaderNode> targetNode, int targetInputIndex) {
    if (!targetNode) {
        return false;
    }
    
    auto it = std::find_if(m_connections.begin(), m_connections.end(),
                          [targetNode, targetInputIndex](const NodeConnection& connection) {
                              return connection.targetNode->getId() == targetNode->getId() &&
                                     connection.targetInputIndex == targetInputIndex;
                          });
    
    if (it != m_connections.end()) {
        // Update connection state on pins
        it->sourceNode->setOutputConnected(it->sourceOutputIndex, false);
        it->targetNode->setInputConnected(it->targetInputIndex, false);
        
        m_connections.erase(it);
        return true;
    }
    
    return false;
}

const std::vector<NodeConnection>& ShaderGraph::getConnections() const {
    return m_connections;
}

bool ShaderGraph::getConnectionSource(std::shared_ptr<ShaderNode> targetNode, int targetInputIndex,
                                     std::shared_ptr<ShaderNode>& sourceNode, int& sourceOutputIndex) const {
    if (!targetNode) {
        return false;
    }
    
    auto it = std::find_if(m_connections.begin(), m_connections.end(),
                          [targetNode, targetInputIndex](const NodeConnection& connection) {
                              return connection.targetNode->getId() == targetNode->getId() &&
                                     connection.targetInputIndex == targetInputIndex;
                          });
    
    if (it != m_connections.end()) {
        sourceNode = it->sourceNode;
        sourceOutputIndex = it->sourceOutputIndex;
        return true;
    }
    
    return false;
}

// Code generation
std::string ShaderGraph::generateVertexShaderCode() const {
    std::stringstream ss;
    
    // Generate common code
    ss << generateCommonCode();
    
    // Generate vertex shader structures
    ss << generateVertexStructures();
    
    // Generate vertex shader main function
    ss << "VertexOutput VSMain(VertexInput input) {\n";
    ss << "    VertexOutput output;\n";
    ss << "    output.position = mul(float4(input.position, 1.0), WorldViewProjection);\n";
    ss << "    output.worldPos = mul(float4(input.position, 1.0), World).xyz;\n";
    ss << "    output.normal = normalize(mul(input.normal, (float3x3)WorldInverseTranspose));\n";
    ss << "    output.texCoord = input.texCoord;\n";
    
    // Add any additional vertex shader code here
    
    ss << "    return output;\n";
    ss << "}\n";
    
    return ss.str();
}

std::string ShaderGraph::generateFragmentShaderCode() const {
    std::stringstream ss;
    
    // Generate fragment shader structures
    ss << generateFragmentStructures();
    
    // Generate fragment shader main function
    ss << "float4 PSMain(VertexOutput input) : SV_TARGET {\n";
    ss << "    FragmentOutput output;\n";
    ss << "    output.color = float3(0, 0, 0);\n";
    ss << "    output.normal = input.normal;\n";
    ss << "    output.emission = float3(0, 0, 0);\n";
    ss << "    output.metallic = 0.0;\n";
    ss << "    output.roughness = 0.5;\n";
    ss << "    output.ao = 1.0;\n\n";
    
    // Generate code for all output nodes and their dependencies
    std::unordered_map<int, std::string> outputVariables;
    std::unordered_map<uint32_t, bool> processedNodes;
    
    auto outputNodes = findOutputNodes();
    for (const auto& outputNode : outputNodes) {
        generateNodeCode(outputNode, ss.str(), outputVariables, processedNodes);
    }
    
    // Return final color
    ss << "    return float4(output.color, 1.0);\n";
    ss << "}\n";
    
    return ss.str();
}

bool ShaderGraph::saveToFile(const std::string& filePath) const {
    try {
        // Create JSON object
        nlohmann::json j;
        
        // Save graph properties
        j["name"] = m_name;
        
        // Save nodes
        nlohmann::json nodesArray = nlohmann::json::array();
        for (const auto& node : m_nodes) {
            nlohmann::json nodeObj;
            nodeObj["id"] = node->getId();
            nodeObj["name"] = node->getName();
            nodeObj["category"] = node->getCategory();
            nodeObj["position"] = {
                {"x", node->getPosition().x},
                {"y", node->getPosition().y}
            };
            nodeObj["size"] = {
                {"width", node->getSize().x},
                {"height", node->getSize().y}
            };
            
            // Save node type-specific data
            if (auto mathNode = std::dynamic_pointer_cast<MathNode>(node)) {
                nodeObj["type"] = "MathNode";
                nodeObj["operation"] = static_cast<int>(mathNode->getOperation());
            } else if (auto textureSampleNode = std::dynamic_pointer_cast<TextureSampleNode>(node)) {
                nodeObj["type"] = "TextureSampleNode";
            } else if (auto vectorNode = std::dynamic_pointer_cast<VectorNode>(node)) {
                nodeObj["type"] = "VectorNode";
                nodeObj["components"] = vectorNode->getComponents();
            } else if (auto inputNode = std::dynamic_pointer_cast<InputNode>(node)) {
                nodeObj["type"] = "InputNode";
                nodeObj["inputType"] = static_cast<int>(inputNode->getInputType());
                nodeObj["customName"] = inputNode->getCustomName();
            } else if (auto outputNode = std::dynamic_pointer_cast<OutputNode>(node)) {
                nodeObj["type"] = "OutputNode";
                nodeObj["outputType"] = static_cast<int>(outputNode->getOutputType());
                nodeObj["customName"] = outputNode->getCustomName();
            }
            
            nodesArray.push_back(nodeObj);
        }
        j["nodes"] = nodesArray;
        
        // Save connections
        nlohmann::json connectionsArray = nlohmann::json::array();
        for (const auto& connection : m_connections) {
            nlohmann::json connectionObj;
            connectionObj["sourceNodeId"] = connection.sourceNode->getId();
            connectionObj["sourceOutputIndex"] = connection.sourceOutputIndex;
            connectionObj["targetNodeId"] = connection.targetNode->getId();
            connectionObj["targetInputIndex"] = connection.targetInputIndex;
            connectionsArray.push_back(connectionObj);
        }
        j["connections"] = connectionsArray;
        
        // Write to file
        std::ofstream file(filePath);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(4); // Pretty print with 4-space indentation
        file.close();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving shader graph: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<ShaderGraph> ShaderGraph::loadFromFile(const std::string& filePath) {
    try {
        // Read JSON from file
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return nullptr;
        }
        
        nlohmann::json j;
        file >> j;
        file.close();
        
        // Create graph
        auto graph = std::make_shared<ShaderGraph>(j["name"]);
        
        // Create nodes
        std::unordered_map<uint32_t, std::shared_ptr<ShaderNode>> nodesMap;
        for (const auto& nodeObj : j["nodes"]) {
            std::shared_ptr<ShaderNode> node;
            
            // Create node based on type
            std::string type = nodeObj["type"];
            if (type == "MathNode") {
                auto operation = static_cast<MathNode::Operation>(nodeObj["operation"]);
                node = ShaderNodeFactory::createMathNode(operation);
            } else if (type == "TextureSampleNode") {
                node = ShaderNodeFactory::createTextureSampleNode();
            } else if (type == "VectorNode") {
                int components = nodeObj["components"];
                node = ShaderNodeFactory::createVectorNode(components);
            } else if (type == "InputNode") {
                auto inputType = static_cast<InputNode::InputType>(nodeObj["inputType"]);
                std::string customName = nodeObj["customName"];
                node = ShaderNodeFactory::createInputNode(inputType, customName);
            } else if (type == "OutputNode") {
                auto outputType = static_cast<OutputNode::OutputType>(nodeObj["outputType"]);
                std::string customName = nodeObj["customName"];
                node = ShaderNodeFactory::createOutputNode(outputType, customName);
            }
            
            if (node) {
                // Set node properties
                node->setPosition({nodeObj["position"]["x"], nodeObj["position"]["y"]});
                node->setSize({nodeObj["size"]["width"], nodeObj["size"]["height"]});
                
                // Add node to graph
                graph->addNode(node);
                
                // Store node in map for connection creation
                nodesMap[nodeObj["id"]] = node;
            }
        }
        
        // Create connections
        for (const auto& connectionObj : j["connections"]) {
            uint32_t sourceNodeId = connectionObj["sourceNodeId"];
            int sourceOutputIndex = connectionObj["sourceOutputIndex"];
            uint32_t targetNodeId = connectionObj["targetNodeId"];
            int targetInputIndex = connectionObj["targetInputIndex"];
            
            auto sourceNode = nodesMap[sourceNodeId];
            auto targetNode = nodesMap[targetNodeId];
            
            if (sourceNode && targetNode) {
                graph->addConnection(sourceNode, sourceOutputIndex, targetNode, targetInputIndex);
            }
        }
        
        return graph;
    } catch (const std::exception& e) {
        std::cerr << "Error loading shader graph: " << e.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<ShaderGraph> ShaderGraph::create(const std::string& name) {
    return std::make_shared<ShaderGraph>(name);
}

std::shared_ptr<ShaderGraph> ShaderGraph::createDefaultPBRGraph() {
    auto graph = std::make_shared<ShaderGraph>("Default PBR");
    
    // Create nodes
    auto albedoInput = ShaderNodeFactory::createInputNode(InputNode::InputType::CUSTOM, "Albedo");
    albedoInput->setPosition({100, 100});
    
    auto metallicInput = ShaderNodeFactory::createInputNode(InputNode::InputType::CUSTOM, "Metallic");
    metallicInput->setPosition({100, 200});
    
    auto roughnessInput = ShaderNodeFactory::createInputNode(InputNode::InputType::CUSTOM, "Roughness");
    roughnessInput->setPosition({100, 300});
    
    auto aoInput = ShaderNodeFactory::createInputNode(InputNode::InputType::CUSTOM, "AO");
    aoInput->setPosition({100, 400});
    
    auto colorOutput = ShaderNodeFactory::createOutputNode(OutputNode::OutputType::COLOR);
    colorOutput->setPosition({500, 100});
    
    auto metallicOutput = ShaderNodeFactory::createOutputNode(OutputNode::OutputType::METALLIC);
    metallicOutput->setPosition({500, 200});
    
    auto roughnessOutput = ShaderNodeFactory::createOutputNode(OutputNode::OutputType::ROUGHNESS);
    roughnessOutput->setPosition({500, 300});
    
    auto aoOutput = ShaderNodeFactory::createOutputNode(OutputNode::OutputType::AMBIENT_OCCLUSION);
    aoOutput->setPosition({500, 400});
    
    // Add nodes to graph
    graph->addNode(albedoInput);
    graph->addNode(metallicInput);
    graph->addNode(roughnessInput);
    graph->addNode(aoInput);
    graph->addNode(colorOutput);
    graph->addNode(metallicOutput);
    graph->addNode(roughnessOutput);
    graph->addNode(aoOutput);
    
    // Create connections
    graph->addConnection(albedoInput, 0, colorOutput, 0);
    graph->addConnection(metallicInput, 0, metallicOutput, 0);
    graph->addConnection(roughnessInput, 0, roughnessOutput, 0);
    graph->addConnection(aoInput, 0, aoOutput, 0);
    
    return graph;
}

// Private helper methods
bool ShaderGraph::generateNodeCode(std::shared_ptr<ShaderNode> node, std::string& code,
                                  std::unordered_map<int, std::string>& outputVariables,
                                  std::unordered_map<uint32_t, bool>& processedNodes) const {
    // Check if node has already been processed
    if (processedNodes[node->getId()]) {
        return true;
    }
    
    // Process dependencies first
    for (int i = 0; i < node->getInputPins().size(); ++i) {
        if (node->getInputPins()[i].isConnected) {
            std::shared_ptr<ShaderNode> sourceNode;
            int sourceOutputIndex;
            
            if (getConnectionSource(node, i, sourceNode, sourceOutputIndex)) {
                if (!generateNodeCode(sourceNode, code, outputVariables, processedNodes)) {
                    return false;
                }
            }
        }
    }
    
    // Generate code for this node
    bool result = node->generateCode(this, code, outputVariables);
    
    // Mark node as processed
    processedNodes[node->getId()] = true;
    
    return result;
}

std::vector<std::shared_ptr<OutputNode>> ShaderGraph::findOutputNodes() const {
    std::vector<std::shared_ptr<OutputNode>> outputNodes;
    
    for (const auto& node : m_nodes) {
        if (auto outputNode = std::dynamic_pointer_cast<OutputNode>(node)) {
            outputNodes.push_back(outputNode);
        }
    }
    
    return outputNodes;
}

std::string ShaderGraph::generateCommonCode() const {
    std::stringstream ss;
    
    // Add common uniforms
    ss << "// Common uniforms\n";
    ss << "cbuffer PerFrameConstants : register(b0) {\n";
    ss << "    float4x4 World;\n";
    ss << "    float4x4 View;\n";
    ss << "    float4x4 Projection;\n";
    ss << "    float4x4 WorldViewProjection;\n";
    ss << "    float4x4 WorldInverseTranspose;\n";
    ss << "    float3 CameraPosition;\n";
    ss << "    float Time;\n";
    ss << "}\n\n";
    
    return ss.str();
}

std::string ShaderGraph::generateVertexStructures() const {
    std::stringstream ss;
    
    // Vertex input structure
    ss << "// Vertex shader input/output structures\n";
    ss << "struct VertexInput {\n";
    ss << "    float3 position : POSITION;\n";
    ss << "    float3 normal : NORMAL;\n";
    ss << "    float2 texCoord : TEXCOORD0;\n";
    ss << "    float4 color : COLOR0;\n";
    ss << "    float3 tangent : TANGENT;\n";
    ss << "    float3 bitangent : BINORMAL;\n";
    ss << "};\n\n";
    
    // Vertex output structure
    ss << "struct VertexOutput {\n";
    ss << "    float4 position : SV_POSITION;\n";
    ss << "    float3 worldPos : POSITION;\n";
    ss << "    float3 normal : NORMAL;\n";
    ss << "    float2 texCoord : TEXCOORD0;\n";
    ss << "    float4 color : COLOR0;\n";
    ss << "};\n\n";
    
    return ss.str();
}

std::string ShaderGraph::generateFragmentStructures() const {
    std::stringstream ss;
    
    // Fragment output structure
    ss << "// Fragment shader output structure\n";
    ss << "struct FragmentOutput {\n";
    ss << "    float3 color;\n";
    ss << "    float3 normal;\n";
    ss << "    float3 emission;\n";
    ss << "    float metallic;\n";
    ss << "    float roughness;\n";
    ss << "    float ao;\n";
    ss << "};\n\n";
    
    return ss.str();
}

} // namespace ElementalRenderer