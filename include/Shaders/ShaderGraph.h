/**
 * @file ShaderGraph.h
 * @brief Shader graph system for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_SHADER_GRAPH_H
#define ELEMENTAL_RENDERER_SHADER_GRAPH_H

#include "ShaderNode.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace ElementalRenderer {

/**
 * @brief Class for managing a shader graph
 */
class ShaderGraph {
public:
    /**
     * @brief Constructor
     * @param name Graph name
     */
    ShaderGraph(const std::string& name);
    
    /**
     * @brief Destructor
     */
    ~ShaderGraph() = default;
    
    /**
     * @brief Get the graph name
     * @return Graph name
     */
    const std::string& getName() const;
    
    /**
     * @brief Set the graph name
     * @param name New name
     */
    void setName(const std::string& name);
    
    /**
     * @brief Add a node to the graph
     * @param node Node to add
     * @return true if the node was added successfully
     */
    bool addNode(std::shared_ptr<ShaderNode> node);
    
    /**
     * @brief Remove a node from the graph
     * @param node Node to remove
     * @return true if the node was removed successfully
     */
    bool removeNode(std::shared_ptr<ShaderNode> node);
    
    /**
     * @brief Remove a node from the graph by ID
     * @param nodeId ID of the node to remove
     * @return true if the node was removed successfully
     */
    bool removeNodeById(uint32_t nodeId);
    
    /**
     * @brief Get a node by ID
     * @param nodeId ID of the node to get
     * @return Shared pointer to the node, or nullptr if not found
     */
    std::shared_ptr<ShaderNode> getNodeById(uint32_t nodeId) const;
    
    /**
     * @brief Get all nodes in the graph
     * @return Vector of nodes
     */
    const std::vector<std::shared_ptr<ShaderNode>>& getNodes() const;
    
    /**
     * @brief Add a connection between nodes
     * @param sourceNode Source node
     * @param sourceOutputIndex Source output pin index
     * @param targetNode Target node
     * @param targetInputIndex Target input pin index
     * @return true if the connection was added successfully
     */
    bool addConnection(std::shared_ptr<ShaderNode> sourceNode, int sourceOutputIndex,
                      std::shared_ptr<ShaderNode> targetNode, int targetInputIndex);
    
    /**
     * @brief Remove a connection
     * @param targetNode Target node
     * @param targetInputIndex Target input pin index
     * @return true if the connection was removed successfully
     */
    bool removeConnection(std::shared_ptr<ShaderNode> targetNode, int targetInputIndex);
    
    /**
     * @brief Get all connections in the graph
     * @return Vector of connections
     */
    const std::vector<NodeConnection>& getConnections() const;
    
    /**
     * @brief Get the source node and output index for a given target node and input index
     * @param targetNode Target node
     * @param targetInputIndex Target input pin index
     * @param sourceNode Output parameter for the source node
     * @param sourceOutputIndex Output parameter for the source output pin index
     * @return true if a connection was found
     */
    bool getConnectionSource(std::shared_ptr<ShaderNode> targetNode, int targetInputIndex,
                            std::shared_ptr<ShaderNode>& sourceNode, int& sourceOutputIndex) const;
    
    /**
     * @brief Generate HLSL vertex shader code from the graph
     * @return Generated vertex shader code
     */
    std::string generateVertexShaderCode() const;
    
    /**
     * @brief Generate HLSL fragment shader code from the graph
     * @return Generated fragment shader code
     */
    std::string generateFragmentShaderCode() const;
    
    /**
     * @brief Save the graph to a file
     * @param filePath Path to save the file
     * @return true if the graph was saved successfully
     */
    bool saveToFile(const std::string& filePath) const;
    
    /**
     * @brief Load a graph from a file
     * @param filePath Path to the file
     * @return Shared pointer to the loaded graph, or nullptr if loading failed
     */
    static std::shared_ptr<ShaderGraph> loadFromFile(const std::string& filePath);
    
    /**
     * @brief Create a new empty graph
     * @param name Graph name
     * @return Shared pointer to the created graph
     */
    static std::shared_ptr<ShaderGraph> create(const std::string& name);
    
    /**
     * @brief Create a default PBR shader graph
     * @return Shared pointer to the created graph
     */
    static std::shared_ptr<ShaderGraph> createDefaultPBRGraph();

private:
    std::string m_name;
    std::vector<std::shared_ptr<ShaderNode>> m_nodes;
    std::vector<NodeConnection> m_connections;
    
    /**
     * @brief Generate code for a node and its dependencies
     * @param node Node to generate code for
     * @param code Output string to append code to
     * @param outputVariables Map of output variable names
     * @param processedNodes Set of nodes that have already been processed
     * @return true if code generation was successful
     */
    bool generateNodeCode(std::shared_ptr<ShaderNode> node, std::string& code,
                         std::unordered_map<int, std::string>& outputVariables,
                         std::unordered_map<uint32_t, bool>& processedNodes) const;
    
    /**
     * @brief Find all output nodes in the graph
     * @return Vector of output nodes
     */
    std::vector<std::shared_ptr<OutputNode>> findOutputNodes() const;
    
    /**
     * @brief Generate common shader code (uniforms, structures, etc.)
     * @return Generated common code
     */
    std::string generateCommonCode() const;
    
    /**
     * @brief Generate vertex shader input/output structures
     * @return Generated vertex shader structures
     */
    std::string generateVertexStructures() const;
    
    /**
     * @brief Generate fragment shader input/output structures
     * @return Generated fragment shader structures
     */
    std::string generateFragmentStructures() const;
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_SHADER_GRAPH_H