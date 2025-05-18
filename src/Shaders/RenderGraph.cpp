#include "../../include/Shaders/RenderGraph.h"
#include <iostream>
#include <queue>

namespace ElementalRenderer {

// RenderPass implementation
RenderPass::RenderPass(const std::string& name, std::function<void()> executionFunc)
    : m_name(name), m_executionFunc(executionFunc) {
}

const std::string& RenderPass::getName() const {
    return m_name;
}

void RenderPass::execute() const {
    if (m_executionFunc) {
        m_executionFunc();
    }
}

void RenderPass::addReadResource(const std::string& resourceName) {
    m_readResources.insert(resourceName);
}

void RenderPass::addWriteResource(const std::string& resourceName) {
    m_writeResources.insert(resourceName);
}

const std::set<std::string>& RenderPass::getReadResources() const {
    return m_readResources;
}

const std::set<std::string>& RenderPass::getWriteResources() const {
    return m_writeResources;
}

// RenderGraph implementation
RenderGraph::RenderGraph(const std::string& name)
    : m_name(name) {
}

const std::string& RenderGraph::getName() const {
    return m_name;
}

void RenderGraph::setName(const std::string& name) {
    m_name = name;
}

bool RenderGraph::addPass(std::shared_ptr<RenderPass> pass) {
    if (!pass) {
        return false;
    }

    const std::string& passName = pass->getName();
    if (m_passMap.find(passName) != m_passMap.end()) {
        std::cerr << "RenderGraph: Pass with name '" << passName << "' already exists" << std::endl;
        return false;
    }

    m_passes.push_back(pass);
    m_passMap[passName] = pass;
    return true;
}

bool RenderGraph::removePass(const std::string& passName) {
    auto it = m_passMap.find(passName);
    if (it == m_passMap.end()) {
        return false;
    }

    auto pass = it->second;
    m_passMap.erase(it);

    auto vecIt = std::find(m_passes.begin(), m_passes.end(), pass);
    if (vecIt != m_passes.end()) {
        m_passes.erase(vecIt);
    }

    return true;
}

std::shared_ptr<RenderPass> RenderGraph::getPass(const std::string& passName) const {
    auto it = m_passMap.find(passName);
    if (it == m_passMap.end()) {
        return nullptr;
    }
    return it->second;
}

const std::vector<std::shared_ptr<RenderPass>>& RenderGraph::getPasses() const {
    return m_passes;
}

bool RenderGraph::buildDependencyGraph() {
    // Clear existing dependencies
    m_dependencies.clear();
    m_sortedPasses.clear();

    // Initialize dependency map for all passes
    for (const auto& pass : m_passes) {
        m_dependencies[pass->getName()] = std::vector<std::string>();
    }

    // Build dependencies based on resource usage
    std::unordered_map<std::string, std::string> lastWriter;

    // First pass: identify write-after-write and read-after-write dependencies
    for (const auto& pass : m_passes) {
        const std::string& passName = pass->getName();
        
        // Check read resources
        for (const auto& resource : pass->getReadResources()) {
            // If there's a previous writer for this resource, add a dependency
            if (lastWriter.find(resource) != lastWriter.end()) {
                m_dependencies[passName].push_back(lastWriter[resource]);
            }
        }
        
        // Check write resources
        for (const auto& resource : pass->getWriteResources()) {
            // If there's a previous writer for this resource, add a dependency
            if (lastWriter.find(resource) != lastWriter.end()) {
                m_dependencies[passName].push_back(lastWriter[resource]);
            }
            // Update last writer
            lastWriter[resource] = passName;
        }
    }

    // Second pass: identify write-after-read dependencies
    std::unordered_map<std::string, std::vector<std::string>> readers;
    
    // Collect all readers for each resource
    for (const auto& pass : m_passes) {
        const std::string& passName = pass->getName();
        for (const auto& resource : pass->getReadResources()) {
            readers[resource].push_back(passName);
        }
    }
    
    // Add dependencies for writers after readers
    for (const auto& pass : m_passes) {
        const std::string& passName = pass->getName();
        for (const auto& resource : pass->getWriteResources()) {
            for (const auto& reader : readers[resource]) {
                // Don't add self-dependencies
                if (reader != passName) {
                    // Add dependency from reader to writer
                    m_dependencies[passName].push_back(reader);
                }
            }
        }
    }

    // Perform topological sort
    return topologicalSort();
}

bool RenderGraph::topologicalSort() {
    std::set<std::string> visited;
    std::set<std::string> temp;
    std::vector<std::string> result;

    // Process all nodes
    for (const auto& pass : m_passes) {
        const std::string& node = pass->getName();
        if (visited.find(node) == visited.end()) {
            if (!topologicalSortUtil(node, visited, temp, result)) {
                std::cerr << "RenderGraph: Cycle detected in dependency graph" << std::endl;
                return false;
            }
        }
    }

    // Reverse the result to get the correct order
    std::reverse(result.begin(), result.end());

    // Build the sorted passes vector
    m_sortedPasses.clear();
    for (const auto& passName : result) {
        m_sortedPasses.push_back(m_passMap[passName]);
    }

    return true;
}

bool RenderGraph::topologicalSortUtil(const std::string& node, 
                                     std::set<std::string>& visited, 
                                     std::set<std::string>& temp, 
                                     std::vector<std::string>& result) {
    // If node is already in temp, we have a cycle
    if (temp.find(node) != temp.end()) {
        return false;
    }

    // If node is already visited, nothing to do
    if (visited.find(node) != visited.end()) {
        return true;
    }

    // Mark node as temporarily visited
    temp.insert(node);

    // Visit all dependencies
    for (const auto& dep : m_dependencies[node]) {
        if (!topologicalSortUtil(dep, visited, temp, result)) {
            return false;
        }
    }

    // Mark node as visited
    temp.erase(node);
    visited.insert(node);
    result.push_back(node);

    return true;
}

void RenderGraph::execute() {
    if (m_sortedPasses.empty()) {
        if (!buildDependencyGraph()) {
            std::cerr << "RenderGraph: Failed to build dependency graph" << std::endl;
            return;
        }
    }

    for (const auto& pass : m_sortedPasses) {
        pass->execute();
    }
}

std::shared_ptr<RenderGraph> RenderGraph::createDefault() {
    auto graph = std::make_shared<RenderGraph>("Default Render Graph");
    
    // Create default render passes
    // These would typically be created based on the specific rendering needs
    
    // Example: Shadow map pass
    auto shadowPass = std::make_shared<RenderPass>("ShadowPass", []() {
        // Shadow map rendering logic would go here
        std::cout << "Executing Shadow Pass" << std::endl;
    });
    shadowPass->addWriteResource("ShadowMap");
    graph->addPass(shadowPass);
    
    // Example: Geometry pass
    auto geometryPass = std::make_shared<RenderPass>("GeometryPass", []() {
        // Geometry rendering logic would go here
        std::cout << "Executing Geometry Pass" << std::endl;
    });
    geometryPass->addReadResource("ShadowMap");
    geometryPass->addWriteResource("GBuffer");
    graph->addPass(geometryPass);
    
    // Example: Lighting pass
    auto lightingPass = std::make_shared<RenderPass>("LightingPass", []() {
        // Lighting calculation logic would go here
        std::cout << "Executing Lighting Pass" << std::endl;
    });
    lightingPass->addReadResource("GBuffer");
    lightingPass->addReadResource("ShadowMap");
    lightingPass->addWriteResource("LightBuffer");
    graph->addPass(lightingPass);
    
    // Example: Post-processing pass
    auto postProcessPass = std::make_shared<RenderPass>("PostProcessPass", []() {
        // Post-processing logic would go here
        std::cout << "Executing Post-Process Pass" << std::endl;
    });
    postProcessPass->addReadResource("LightBuffer");
    postProcessPass->addWriteResource("FinalImage");
    graph->addPass(postProcessPass);
    
    // Build the dependency graph
    graph->buildDependencyGraph();
    
    return graph;
}

} // namespace ElementalRenderer