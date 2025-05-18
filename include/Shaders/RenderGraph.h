#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <set>
#include <algorithm>

namespace ElementalRenderer {

/**
 * @brief Represents a single render pass in the render graph
 */
class RenderPass {
public:
    /**
     * @brief Constructor
     * @param name Name of the render pass
     * @param executionFunc Function to execute for this render pass
     */
    RenderPass(const std::string& name, std::function<void()> executionFunc);
    
    /**
     * @brief Get the name of the render pass
     * @return Name of the render pass
     */
    const std::string& getName() const;
    
    /**
     * @brief Execute the render pass
     */
    void execute() const;
    
    /**
     * @brief Add a resource that this pass reads from
     * @param resourceName Name of the resource
     */
    void addReadResource(const std::string& resourceName);
    
    /**
     * @brief Add a resource that this pass writes to
     * @param resourceName Name of the resource
     */
    void addWriteResource(const std::string& resourceName);
    
    /**
     * @brief Get resources that this pass reads from
     * @return Set of resource names
     */
    const std::set<std::string>& getReadResources() const;
    
    /**
     * @brief Get resources that this pass writes to
     * @return Set of resource names
     */
    const std::set<std::string>& getWriteResources() const;

private:
    std::string m_name;
    std::function<void()> m_executionFunc;
    std::set<std::string> m_readResources;
    std::set<std::string> m_writeResources;
};

/**
 * @brief Heuristic Render Graph system that automatically builds an optimized dependency graph of render passes
 */
class RenderGraph {
public:
    /**
     * @brief Constructor
     * @param name Name of the render graph
     */
    RenderGraph(const std::string& name = "Default Render Graph");
    
    /**
     * @brief Destructor
     */
    ~RenderGraph() = default;
    
    /**
     * @brief Get the name of the render graph
     * @return Name of the render graph
     */
    const std::string& getName() const;
    
    /**
     * @brief Set the name of the render graph
     * @param name New name
     */
    void setName(const std::string& name);
    
    /**
     * @brief Add a render pass to the graph
     * @param pass Render pass to add
     * @return true if the pass was added successfully
     */
    bool addPass(std::shared_ptr<RenderPass> pass);
    
    /**
     * @brief Remove a render pass from the graph
     * @param passName Name of the pass to remove
     * @return true if the pass was removed successfully
     */
    bool removePass(const std::string& passName);
    
    /**
     * @brief Get a render pass by name
     * @param passName Name of the pass to get
     * @return Shared pointer to the pass, or nullptr if not found
     */
    std::shared_ptr<RenderPass> getPass(const std::string& passName) const;
    
    /**
     * @brief Get all render passes in the graph
     * @return Vector of render passes
     */
    const std::vector<std::shared_ptr<RenderPass>>& getPasses() const;
    
    /**
     * @brief Build the dependency graph based on resource read/write relationships
     * @return true if the graph was built successfully
     */
    bool buildDependencyGraph();
    
    /**
     * @brief Execute all render passes in the optimal order
     */
    void execute();
    
    /**
     * @brief Create a default render graph with common passes
     * @return Shared pointer to the created graph
     */
    static std::shared_ptr<RenderGraph> createDefault();

private:
    std::string m_name;
    std::vector<std::shared_ptr<RenderPass>> m_passes;
    std::unordered_map<std::string, std::shared_ptr<RenderPass>> m_passMap;
    std::vector<std::shared_ptr<RenderPass>> m_sortedPasses;
    std::unordered_map<std::string, std::vector<std::string>> m_dependencies;
    
    /**
     * @brief Perform topological sort on the dependency graph
     * @return true if sort was successful (no cycles)
     */
    bool topologicalSort();
    
    /**
     * @brief Recursive helper for topological sort
     * @param node Current node
     * @param visited Set of visited nodes
     * @param temp Set of temporarily visited nodes (for cycle detection)
     * @param result Vector to store the sorted result
     * @return true if no cycles were detected
     */
    bool topologicalSortUtil(const std::string& node, 
                            std::set<std::string>& visited, 
                            std::set<std::string>& temp, 
                            std::vector<std::string>& result);
};

} // namespace ElementalRenderer