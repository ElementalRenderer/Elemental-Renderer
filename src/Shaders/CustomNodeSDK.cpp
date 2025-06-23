/**
 * @file CustomNodeSDK.cpp
 * @brief Implementation of the Custom Node SDK for ElementalRenderer
 */

#include "Shaders/CustomNodeSDK.h"
#include "Shaders/ShaderGraph.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace ElementalRenderer {

// CustomShaderNode implementation
CustomShaderNode::CustomShaderNode(std::shared_ptr<ICustomNodeDefinition> definition)
    : ShaderNode(definition->getName(), definition->getCategory())
    , m_definition(definition)
{
    // Add input pins from definition
    for (const auto& pin : definition->getInputPins()) {
        addInputPin(pin.name, pin.type, pin.defaultValue);
    }
    
    // Add output pins from definition
    for (const auto& pin : definition->getOutputPins()) {
        addOutputPin(pin.name, pin.type);
    }
}

bool CustomShaderNode::generateCode(const ShaderGraph* graph, std::string& code,
                                  std::unordered_map<int, std::string>& outputVariables)
{
    // Collect input variable names
    std::unordered_map<int, std::string> inputVariables;
    for (int i = 0; i < m_inputPins.size(); ++i) {
        std::shared_ptr<ShaderNode> sourceNode;
        int sourceOutputIndex;
        
        if (graph->getConnectionSource(shared_from_this(), i, sourceNode, sourceOutputIndex)) {
            // Input is connected, get the variable name from the source node
            auto it = outputVariables.find(sourceNode->getId() * 1000 + sourceOutputIndex);
            if (it != outputVariables.end()) {
                inputVariables[i] = it->second;
            } else {
                // Source node hasn't generated its output variable yet
                return false;
            }
        } else {
            // Input is not connected, use default value
            inputVariables[i] = m_inputPins[i].defaultValue;
        }
    }
    
    // Generate code using the definition
    return m_definition->generateCode(graph, code, outputVariables, inputVariables);
}

std::shared_ptr<ICustomNodeDefinition> CustomShaderNode::getDefinition() const
{
    return m_definition;
}

// CustomNodeDefinition implementation
CustomNodeDefinition::CustomNodeDefinition(const std::string& name, const std::string& category, const std::string& description)
    : m_name(name)
    , m_category(category)
    , m_description(description)
{
}

std::string CustomNodeDefinition::getName() const
{
    return m_name;
}

std::string CustomNodeDefinition::getCategory() const
{
    return m_category;
}

std::string CustomNodeDefinition::getDescription() const
{
    return m_description;
}

std::vector<NodePin> CustomNodeDefinition::getInputPins() const
{
    return m_inputPins;
}

std::vector<NodePin> CustomNodeDefinition::getOutputPins() const
{
    return m_outputPins;
}

void CustomNodeDefinition::addInputPin(const std::string& name, NodePin::Type type, const std::string& defaultValue)
{
    NodePin pin;
    pin.name = name;
    pin.type = type;
    pin.defaultValue = defaultValue;
    pin.isConnected = false;
    m_inputPins.push_back(pin);
}

void CustomNodeDefinition::addOutputPin(const std::string& name, NodePin::Type type)
{
    NodePin pin;
    pin.name = name;
    pin.type = type;
    pin.isConnected = false;
    m_outputPins.push_back(pin);
}

// JsonNodeDefinition implementation
JsonNodeDefinition::JsonNodeDefinition(const std::string& jsonPath)
    : m_jsonPath(jsonPath)
{
    // Load and parse the JSON file
    std::ifstream file(jsonPath);
    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        
        parseJson(buffer.str());
        
        // Store the last modified time
        m_lastModified = std::filesystem::last_write_time(jsonPath);
    } else {
        std::cerr << "Failed to open JSON file: " << jsonPath << std::endl;
    }
}

std::string JsonNodeDefinition::getName() const
{
    return m_name;
}

std::string JsonNodeDefinition::getCategory() const
{
    return m_category;
}

std::string JsonNodeDefinition::getDescription() const
{
    return m_description;
}

std::vector<NodePin> JsonNodeDefinition::getInputPins() const
{
    return m_inputPins;
}

std::vector<NodePin> JsonNodeDefinition::getOutputPins() const
{
    return m_outputPins;
}

bool JsonNodeDefinition::generateCode(const ShaderGraph* graph, std::string& code,
                                    std::unordered_map<int, std::string>& outputVariables,
                                    const std::unordered_map<int, std::string>& inputVariables) const
{
    // Start with the code template
    std::string generatedCode = m_codeTemplate;
    
    // Replace input variable placeholders
    for (const auto& [index, varName] : inputVariables) {
        if (index < m_inputPins.size()) {
            std::string placeholder = "{{" + m_inputPins[index].name + "}}";
            size_t pos = generatedCode.find(placeholder);
            while (pos != std::string::npos) {
                generatedCode.replace(pos, placeholder.length(), varName);
                pos = generatedCode.find(placeholder, pos + varName.length());
            }
        }
    }
    
    // Generate output variable names
    for (int i = 0; i < m_outputPins.size(); ++i) {
        std::string varName = "customNode_" + std::to_string(graph->getNodeById(getId())->getId()) + "_out" + std::to_string(i);
        
        // Replace output variable placeholders
        std::string placeholder = "{{" + m_outputPins[i].name + "_out}}";
        size_t pos = generatedCode.find(placeholder);
        while (pos != std::string::npos) {
            generatedCode.replace(pos, placeholder.length(), varName);
            pos = generatedCode.find(placeholder, pos + varName.length());
        }
        
        // Store output variable name
        outputVariables[getId() * 1000 + i] = varName;
    }
    
    // Add the generated code
    code += generatedCode;
    
    return true;
}

bool JsonNodeDefinition::hasFileChanged() const
{
    if (!std::filesystem::exists(m_jsonPath)) {
        return false;
    }
    
    auto lastModified = std::filesystem::last_write_time(m_jsonPath);
    return lastModified != m_lastModified;
}

bool JsonNodeDefinition::reload()
{
    if (!std::filesystem::exists(m_jsonPath)) {
        return false;
    }
    
    std::ifstream file(m_jsonPath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    bool result = parseJson(buffer.str());
    if (result) {
        m_lastModified = std::filesystem::last_write_time(m_jsonPath);
    }
    
    return result;
}

bool JsonNodeDefinition::parseJson(const std::string& jsonContent)
{
    try {
        nlohmann::json json = nlohmann::json::parse(jsonContent);
        
        // Parse basic node information
        m_name = json["name"].get<std::string>();
        m_category = json["category"].get<std::string>();
        m_description = json["description"].get<std::string>();
        
        // Parse input pins
        m_inputPins.clear();
        for (const auto& pinJson : json["inputs"]) {
            NodePin pin;
            pin.name = pinJson["name"].get<std::string>();
            
            // Parse pin type
            std::string typeStr = pinJson["type"].get<std::string>();
            if (typeStr == "float") pin.type = NodePin::Type::FLOAT;
            else if (typeStr == "vec2") pin.type = NodePin::Type::VEC2;
            else if (typeStr == "vec3") pin.type = NodePin::Type::VEC3;
            else if (typeStr == "vec4") pin.type = NodePin::Type::VEC4;
            else if (typeStr == "int") pin.type = NodePin::Type::INT;
            else if (typeStr == "bool") pin.type = NodePin::Type::BOOL;
            else if (typeStr == "sampler2D") pin.type = NodePin::Type::SAMPLER2D;
            else if (typeStr == "matrix") pin.type = NodePin::Type::MATRIX;
            else {
                std::cerr << "Unknown pin type: " << typeStr << std::endl;
                return false;
            }
            
            // Parse default value (if present)
            if (pinJson.contains("defaultValue")) {
                pin.defaultValue = pinJson["defaultValue"].get<std::string>();
            } else {
                pin.defaultValue = "";
            }
            
            pin.isConnected = false;
            m_inputPins.push_back(pin);
        }
        
        // Parse output pins
        m_outputPins.clear();
        for (const auto& pinJson : json["outputs"]) {
            NodePin pin;
            pin.name = pinJson["name"].get<std::string>();
            
            // Parse pin type
            std::string typeStr = pinJson["type"].get<std::string>();
            if (typeStr == "float") pin.type = NodePin::Type::FLOAT;
            else if (typeStr == "vec2") pin.type = NodePin::Type::VEC2;
            else if (typeStr == "vec3") pin.type = NodePin::Type::VEC3;
            else if (typeStr == "vec4") pin.type = NodePin::Type::VEC4;
            else if (typeStr == "int") pin.type = NodePin::Type::INT;
            else if (typeStr == "bool") pin.type = NodePin::Type::BOOL;
            else if (typeStr == "sampler2D") pin.type = NodePin::Type::SAMPLER2D;
            else if (typeStr == "matrix") pin.type = NodePin::Type::MATRIX;
            else {
                std::cerr << "Unknown pin type: " << typeStr << std::endl;
                return false;
            }
            
            pin.isConnected = false;
            m_outputPins.push_back(pin);
        }
        
        // Parse code template
        m_codeTemplate = json["code"].get<std::string>();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        return false;
    }
}

// CustomNodeManager implementation
static CustomNodeManager* s_instance = nullptr;

CustomNodeManager& CustomNodeManager::getInstance()
{
    if (!s_instance) {
        s_instance = new CustomNodeManager();
    }
    return *s_instance;
}

CustomNodeManager::CustomNodeManager()
    : m_hotReloadingEnabled(false)
{
}

CustomNodeManager::~CustomNodeManager()
{
    m_nodeDefinitions.clear();
}

bool CustomNodeManager::registerNodeDefinition(std::shared_ptr<ICustomNodeDefinition> definition)
{
    if (!definition) {
        return false;
    }
    
    std::string name = definition->getName();
    if (m_nodeDefinitions.find(name) != m_nodeDefinitions.end()) {
        std::cerr << "Node definition with name '" << name << "' already exists" << std::endl;
        return false;
    }
    
    m_nodeDefinitions[name] = definition;
    return true;
}

bool CustomNodeManager::unregisterNodeDefinition(const std::string& name)
{
    auto it = m_nodeDefinitions.find(name);
    if (it == m_nodeDefinitions.end()) {
        return false;
    }
    
    m_nodeDefinitions.erase(it);
    return true;
}

std::shared_ptr<ICustomNodeDefinition> CustomNodeManager::getNodeDefinition(const std::string& name) const
{
    auto it = m_nodeDefinitions.find(name);
    if (it == m_nodeDefinitions.end()) {
        return nullptr;
    }
    
    return it->second;
}

const std::unordered_map<std::string, std::shared_ptr<ICustomNodeDefinition>>& CustomNodeManager::getNodeDefinitions() const
{
    return m_nodeDefinitions;
}

std::shared_ptr<CustomShaderNode> CustomNodeManager::createNode(const std::string& name) const
{
    auto definition = getNodeDefinition(name);
    if (!definition) {
        return nullptr;
    }
    
    return std::make_shared<CustomShaderNode>(definition);
}

int CustomNodeManager::loadFromDirectory(const std::string& directory, bool recursive)
{
    int count = 0;
    
    try {
        std::filesystem::path dirPath(directory);
        if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath)) {
            std::cerr << "Directory does not exist: " << directory << std::endl;
            return 0;
        }
        
        std::filesystem::directory_iterator end;
        std::filesystem::directory_iterator it(dirPath);
        
        if (recursive) {
            for (auto& entry : std::filesystem::recursive_directory_iterator(dirPath)) {
                if (entry.is_regular_file() && entry.path().extension() == ".json") {
                    auto definition = std::make_shared<JsonNodeDefinition>(entry.path().string());
                    if (registerNodeDefinition(definition)) {
                        count++;
                    }
                }
            }
        } else {
            for (auto& entry : std::filesystem::directory_iterator(dirPath)) {
                if (entry.is_regular_file() && entry.path().extension() == ".json") {
                    auto definition = std::make_shared<JsonNodeDefinition>(entry.path().string());
                    if (registerNodeDefinition(definition)) {
                        count++;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading node definitions from directory: " << e.what() << std::endl;
    }
    
    return count;
}

int CustomNodeManager::checkForChanges()
{
    if (!m_hotReloadingEnabled) {
        return 0;
    }
    
    int count = 0;
    
    for (auto& [name, definition] : m_nodeDefinitions) {
        auto jsonDef = std::dynamic_pointer_cast<JsonNodeDefinition>(definition);
        if (jsonDef && jsonDef->hasFileChanged()) {
            if (jsonDef->reload()) {
                count++;
            }
        }
    }
    
    return count;
}

void CustomNodeManager::setHotReloadingEnabled(bool enable)
{
    m_hotReloadingEnabled = enable;
}

bool CustomNodeManager::isHotReloadingEnabled() const
{
    return m_hotReloadingEnabled;
}

} // namespace ElementalRenderer