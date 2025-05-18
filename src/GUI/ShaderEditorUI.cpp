/**
 * @file ShaderEditorUI.cpp
 * @brief Implementation of the shader editor UI
 */

#include "GUI/ShaderEditorUI.h"
#include <sstream>
#include <iostream>
#include <algorithm>

namespace ElementalRenderer {

ShaderEditorUI::ShaderEditorUI()
    : m_draggingNode(false), m_draggedNodeId(0), m_creatingConnection(false),
      m_connectionSourceNodeId(0), m_connectionSourceOutputIndex(0),
      m_selectedNodeId(0), m_codeViewMode(false) {
}

ShaderEditorUI::~ShaderEditorUI() {
}

bool ShaderEditorUI::initialize(int width, int height) {

    createMainUI();

    createNewGraph("New Shader");

    return true;
}

void ShaderEditorUI::render() {
    if (!m_mainPanel) {
        return;
    }

    updateNodeUI();

    m_mainPanel->render();
    renderConnections();
    renderNodeCreationMenu();

    if (m_selectedNodeId != 0) {
        renderPropertiesPanel();
    }

    if (m_codeViewMode) {
        updateCodeEditor();
    }
}

bool ShaderEditorUI::processMouseInput(const glm::vec2& mousePos, int button, int action) {
    if (!m_mainPanel) {
        return false;
    }

    if (m_draggingNode) {
        if (button == 0 && action == 0) {
            m_draggingNode = false;
            return true;
        }

        auto node = m_shaderGraph->getNodeById(m_draggedNodeId);
        if (node) {
            node->setPosition(mousePos - m_dragOffset);
            return true;
        }
    }

    if (m_creatingConnection) {
        if (button == 0 && action == 0) {
            m_creatingConnection = false;

            for (const auto& nodeUIEntry : m_nodeUIs) {
                auto nodeId = nodeUIEntry.first;
                const auto& nodeUI = nodeUIEntry.second;

                for (int i = 0; i < nodeUI.inputPins.size(); ++i) {
                    const auto& pin = nodeUI.inputPins[i];
                    glm::vec2 pinPos = pin->getPosition();
                    glm::vec2 pinSize = pin->getSize();

                    if (mousePos.x >= pinPos.x && mousePos.x <= pinPos.x + pinSize.x &&
                        mousePos.y >= pinPos.y && mousePos.y <= pinPos.y + pinSize.y) {
                        completeConnectionCreation(nodeId, i);
                        return true;
                    }
                }
            }

            cancelConnectionCreation();
            return true;
        }

        m_connectionEndPoint = mousePos;
        return true;
    }

    if (button == 0 && action == 1) {
        for (const auto& nodeUIEntry : m_nodeUIs) {
            auto nodeId = nodeUIEntry.first;
            const auto& nodeUI = nodeUIEntry.second;

            glm::vec2 panelPos = nodeUI.panel->getPosition();
            glm::vec2 panelSize = nodeUI.panel->getSize();

            if (mousePos.x >= panelPos.x && mousePos.x <= panelPos.x + panelSize.x &&
                mousePos.y >= panelPos.y && mousePos.y <= panelPos.y + panelSize.y) {

                handleNodeSelection(nodeId);

                m_draggingNode = true;
                m_draggedNodeId = nodeId;
                m_dragOffset = mousePos - panelPos;
                return true;
            }

            for (int i = 0; i < nodeUI.outputPins.size(); ++i) {
                const auto& pin = nodeUI.outputPins[i];
                glm::vec2 pinPos = pin->getPosition();
                glm::vec2 pinSize = pin->getSize();

                if (mousePos.x >= pinPos.x && mousePos.x <= pinPos.x + pinSize.x &&
                    mousePos.y >= pinPos.y && mousePos.y <= pinPos.y + pinSize.y) {
                    handlePinClick(nodeId, i, true);
                    return true;
                }
            }

            for (int i = 0; i < nodeUI.inputPins.size(); ++i) {
                const auto& pin = nodeUI.inputPins[i];
                glm::vec2 pinPos = pin->getPosition();
                glm::vec2 pinSize = pin->getSize();

                if (mousePos.x >= pinPos.x && mousePos.x <= pinPos.x + pinSize.x &&
                    mousePos.y >= pinPos.y && mousePos.y <= pinPos.y + pinSize.y) {
                    handlePinClick(nodeId, i, false);
                    return true;
                }
            }
        }
    }

    return m_mainPanel->processMouseInput(mousePos, button);
}

bool ShaderEditorUI::processKeyInput(int key, int action) {
    if (!m_mainPanel) {
        return false;
    }

    if (key == 261 && action == 1 && m_selectedNodeId != 0) {
        removeSelectedNode();
        return true;
    }

    if (key == 68 && action == 1 && m_selectedNodeId != 0) {
        duplicateSelectedNode();
        return true;
    }

    return m_mainPanel->processKeyInput(key, action);
}

bool ShaderEditorUI::processCharInput(unsigned int codepoint) {
    if (!m_mainPanel) {
        return false;
    }

    if (m_codeViewMode && m_codeEditor) {

        return true;
    }

    return false;
}

void ShaderEditorUI::setShaderGraph(std::shared_ptr<ShaderGraph> graph) {
    if (!graph) {
        return;
    }

    m_shaderGraph = graph;
    m_selectedNodeId = 0;
    m_nodeUIs.clear();
    m_connectionUIs.clear();

    for (const auto& node : m_shaderGraph->getNodes()) {
        m_nodeUIs[node->getId()] = createNodeUI(node);
    }

    for (const auto& connection : m_shaderGraph->getConnections()) {
        ConnectionUI connectionUI;
        connectionUI.sourceNodeId = connection.sourceNode->getId();
        connectionUI.sourceOutputIndex = connection.sourceOutputIndex;
        connectionUI.targetNodeId = connection.targetNode->getId();
        connectionUI.targetInputIndex = connection.targetInputIndex;
        m_connectionUIs.push_back(connectionUI);
    }

    if (m_codeViewMode) {
        updateCodeEditor();
    }
}

std::shared_ptr<ShaderGraph> ShaderEditorUI::getShaderGraph() const {
    return m_shaderGraph;
}

void ShaderEditorUI::createNewGraph(const std::string& name) {
    m_shaderGraph = ShaderGraph::create(name);
    m_selectedNodeId = 0;
    m_nodeUIs.clear();
    m_connectionUIs.clear();

    auto positionNode = ShaderNodeFactory::createInputNode(InputNode::InputType::POSITION);
    positionNode->setPosition({100, 100});

    auto colorOutput = ShaderNodeFactory::createOutputNode(OutputNode::OutputType::COLOR);
    colorOutput->setPosition({500, 100});

    m_shaderGraph->addNode(positionNode);
    m_shaderGraph->addNode(colorOutput);

    m_nodeUIs[positionNode->getId()] = createNodeUI(positionNode);
    m_nodeUIs[colorOutput->getId()] = createNodeUI(colorOutput);

    if (m_codeViewMode) {
        updateCodeEditor();
    }
}

bool ShaderEditorUI::loadGraphFromFile(const std::string& filePath) {
    auto graph = ShaderGraph::loadFromFile(filePath);
    if (!graph) {
        return false;
    }

    setShaderGraph(graph);
    return true;
}

bool ShaderEditorUI::saveGraphToFile(const std::string& filePath) {
    if (!m_shaderGraph) {
        return false;
    }

    return m_shaderGraph->saveToFile(filePath);
}

bool ShaderEditorUI::generateShaderCode(std::string& vertexCode, std::string& fragmentCode) {
    if (!m_shaderGraph) {
        return false;
    }

    vertexCode = m_shaderGraph->generateVertexShaderCode();
    fragmentCode = m_shaderGraph->generateFragmentShaderCode();

    return true;
}

bool ShaderEditorUI::compileAndTestShader() {
    if (!m_shaderGraph) {
        return false;
    }

    std::string vertexCode, fragmentCode;
    if (!generateShaderCode(vertexCode, fragmentCode)) {
        if (m_onShaderCompiled) {
            m_onShaderCompiled(false, "Failed to generate shader code");
        }
        return false;
    }

    if (m_onShaderCompiled) {
        m_onShaderCompiled(true, "Shader compiled successfully");
    }

    return true;
}

void ShaderEditorUI::setOnShaderCompiledCallback(std::function<void(bool, const std::string&)> callback) {
    m_onShaderCompiled = callback;
}

void ShaderEditorUI::createMainUI() {

    m_mainPanel = ElementalGUI::createPanel({0, 0}, {1280, 720}, "Shader Editor");

    auto toolbar = ElementalGUI::createPanel({0, 0}, {1280, 40}, "Toolbar");
    toolbar->setBackgroundColor({0.2f, 0.2f, 0.2f, 1.0f});

    m_newButton = ElementalGUI::createButton({10, 5}, {80, 30}, "New");
    m_newButton->setOnClickCallback([this]() {
        createNewGraph("New Shader");
    });

    m_loadButton = ElementalGUI::createButton({100, 5}, {80, 30}, "Load");
    m_loadButton->setOnClickCallback([this]() {

        loadGraphFromFile("shader.json");
    });

    m_saveButton = ElementalGUI::createButton({190, 5}, {80, 30}, "Save");
    m_saveButton->setOnClickCallback([this]() {

        saveGraphToFile("shader.json");
    });

    m_compileButton = ElementalGUI::createButton({280, 5}, {80, 30}, "Compile");
    m_compileButton->setOnClickCallback([this]() {
        compileAndTestShader();
    });

    m_showCodeCheckbox = ElementalGUI::createCheckbox({370, 5}, {30, 30}, "Show Code", m_codeViewMode);
    m_showCodeCheckbox->setOnChangeCallback([this](bool checked) {
        m_codeViewMode = checked;
        if (m_codeViewMode) {
            updateCodeEditor();
        }
    });

    toolbar->addElement(m_newButton);
    toolbar->addElement(m_loadButton);
    toolbar->addElement(m_saveButton);
    toolbar->addElement(m_compileButton);
    toolbar->addElement(m_showCodeCheckbox);
    toolbar->addElement(ElementalGUI::createLabel({410, 10}, "Show Code"));

    m_nodePanel = ElementalGUI::createPanel({0, 40}, {1280, 680}, "Node Editor");
    m_nodePanel->setBackgroundColor({0.15f, 0.15f, 0.15f, 1.0f});

    m_propertiesPanel = ElementalGUI::createPanel({1000, 40}, {280, 340}, "Properties");
    m_propertiesPanel->setBackgroundColor({0.2f, 0.2f, 0.2f, 1.0f});
    m_propertiesPanel->setVisible(false);

    m_codePanel = ElementalGUI::createPanel({1000, 380}, {280, 340}, "Code");
    m_codePanel->setBackgroundColor({0.2f, 0.2f, 0.2f, 1.0f});
    m_codePanel->setVisible(false);

    m_codeEditor = ElementalGUI::createTextInput({10, 40}, {260, 290}, "// Shader code will appear here");
    m_codePanel->addElement(m_codeEditor);

    m_mainPanel->addElement(toolbar);
    m_mainPanel->addElement(m_nodePanel);
    m_mainPanel->addElement(m_propertiesPanel);
    m_mainPanel->addElement(m_codePanel);
}

void ShaderEditorUI::updateNodeUI() {
    if (!m_shaderGraph) {
        return;
    }

    updateNodeUIPositions();

    m_codePanel->setVisible(m_codeViewMode);
}

void ShaderEditorUI::renderConnections() {

    if (m_creatingConnection) {

    }
}

void ShaderEditorUI::renderNodeCreationMenu() {

}

void ShaderEditorUI::renderPropertiesPanel() {
    if (!m_shaderGraph || m_selectedNodeId == 0) {
        m_propertiesPanel->setVisible(false);
        return;
    }

    auto node = m_shaderGraph->getNodeById(m_selectedNodeId);
    if (!node) {
        m_propertiesPanel->setVisible(false);
        return;
    }

    m_propertiesPanel->setVisible(true);

}

void ShaderEditorUI::updateCodeEditor() {
    if (!m_shaderGraph || !m_codeEditor) {
        return;
    }

    std::string vertexCode, fragmentCode;
    if (generateShaderCode(vertexCode, fragmentCode)) {
        m_codeEditor->setText(fragmentCode);
    } else {
        m_codeEditor->setText("// Failed to generate shader code");
    }

    m_codePanel->setVisible(true);
}

ShaderEditorUI::NodeUI ShaderEditorUI::createNodeUI(std::shared_ptr<ShaderNode> node) {
    NodeUI nodeUI;
    nodeUI.nodeId = node->getId();

    glm::vec4 nodeColor = getNodeColor(node->getCategory());
    nodeUI.panel = ElementalGUI::createPanel(node->getPosition(), node->getSize(), node->getName());
    nodeUI.panel->setBackgroundColor(nodeColor);

    const auto& inputPins = node->getInputPins();
    for (int i = 0; i < inputPins.size(); ++i) {
        const auto& pin = inputPins[i];
        glm::vec2 pinPos = {5, 30 + i * 20};
        glm::vec2 pinSize = {15, 15};

        auto pinElement = ElementalGUI::createPanel(pinPos, pinSize, "");
        pinElement->setBackgroundColor(getPinColor(pin.type));

        auto pinLabel = ElementalGUI::createLabel({25, 30 + i * 20}, pin.name);

        nodeUI.panel->addElement(pinElement);
        nodeUI.panel->addElement(pinLabel);
        nodeUI.inputPins.push_back(pinElement);
    }

    const auto& outputPins = node->getOutputPins();
    for (int i = 0; i < outputPins.size(); ++i) {
        const auto& pin = outputPins[i];
        glm::vec2 pinPos = {node->getSize().x - 20, 30 + i * 20};
        glm::vec2 pinSize = {15, 15};

        auto pinElement = ElementalGUI::createPanel(pinPos, pinSize, "");
        pinElement->setBackgroundColor(getPinColor(pin.type));

        auto pinLabel = ElementalGUI::createLabel({node->getSize().x - 80, 30 + i * 20}, pin.name);

        nodeUI.panel->addElement(pinElement);
        nodeUI.panel->addElement(pinLabel);
        nodeUI.outputPins.push_back(pinElement);
    }

    m_nodePanel->addElement(nodeUI.panel);

    return nodeUI;
}

void ShaderEditorUI::updateNodeUIPositions() {
    if (!m_shaderGraph) {
        return;
    }

    for (const auto& node : m_shaderGraph->getNodes()) {
        auto it = m_nodeUIs.find(node->getId());
        if (it != m_nodeUIs.end()) {
            it->second.panel->setPosition(node->getPosition());
        }
    }
}

void ShaderEditorUI::handleNodeSelection(uint32_t nodeId) {
    m_selectedNodeId = nodeId;

    renderPropertiesPanel();
}

void ShaderEditorUI::handlePinClick(uint32_t nodeId, int pinIndex, bool isOutput) {
    auto node = m_shaderGraph->getNodeById(nodeId);
    if (!node) {
        return;
    }

    if (isOutput) {
        m_creatingConnection = true;
        m_connectionSourceNodeId = nodeId;
        m_connectionSourceOutputIndex = pinIndex;

        auto it = m_nodeUIs.find(nodeId);
        if (it != m_nodeUIs.end() && pinIndex < it->second.outputPins.size()) {
            glm::vec2 pinPos = it->second.outputPins[pinIndex]->getPosition();
            glm::vec2 pinSize = it->second.outputPins[pinIndex]->getSize();
            glm::vec2 nodePos = it->second.panel->getPosition();

            m_connectionEndPoint = nodePos + pinPos + pinSize / 2.0f;
        }
    } else {

        m_shaderGraph->removeConnection(node, pinIndex);

        auto it = std::find_if(m_connectionUIs.begin(), m_connectionUIs.end(),
                              [nodeId, pinIndex](const ConnectionUI& connection) {
                                  return connection.targetNodeId == nodeId &&
                                         connection.targetInputIndex == pinIndex;
                              });

        if (it != m_connectionUIs.end()) {
            m_connectionUIs.erase(it);
        }
    }
}

void ShaderEditorUI::completeConnectionCreation(uint32_t targetNodeId, int targetInputIndex) {
    auto sourceNode = m_shaderGraph->getNodeById(m_connectionSourceNodeId);
    auto targetNode = m_shaderGraph->getNodeById(targetNodeId);

    if (!sourceNode || !targetNode) {
        return;
    }

    if (m_shaderGraph->addConnection(sourceNode, m_connectionSourceOutputIndex, targetNode, targetInputIndex)) {

        ConnectionUI connectionUI;
        connectionUI.sourceNodeId = m_connectionSourceNodeId;
        connectionUI.sourceOutputIndex = m_connectionSourceOutputIndex;
        connectionUI.targetNodeId = targetNodeId;
        connectionUI.targetInputIndex = targetInputIndex;
        m_connectionUIs.push_back(connectionUI);

    }
}

void ShaderEditorUI::cancelConnectionCreation() {
    m_creatingConnection = false;
}

void ShaderEditorUI::addNode(const std::string& nodeType, const glm::vec2& position) {
    if (!m_shaderGraph) {
        return;
    }

    std::shared_ptr<ShaderNode> node;

    if (nodeType == "MathAdd") {
        node = ShaderNodeFactory::createMathNode(MathNode::Operation::ADD);
    } else if (nodeType == "MathMultiply") {
        node = ShaderNodeFactory::createMathNode(MathNode::Operation::MULTIPLY);
    } else if (nodeType == "TextureSample") {
        node = ShaderNodeFactory::createTextureSampleNode();
    } else if (nodeType == "Vector3") {
        node = ShaderNodeFactory::createVectorNode(3);
    } else if (nodeType == "InputPosition") {
        node = ShaderNodeFactory::createInputNode(InputNode::InputType::POSITION);
    } else if (nodeType == "OutputColor") {
        node = ShaderNodeFactory::createOutputNode(OutputNode::OutputType::COLOR);
    }

    if (node) {
        node->setPosition(position);

        if (m_shaderGraph->addNode(node)) {
            m_nodeUIs[node->getId()] = createNodeUI(node);
            handleNodeSelection(node->getId());
        }
    }
}

void ShaderEditorUI::removeSelectedNode() {
    if (!m_shaderGraph || m_selectedNodeId == 0) {
        return;
    }

    if (m_shaderGraph->removeNodeById(m_selectedNodeId)) {
        auto it = m_nodeUIs.find(m_selectedNodeId);
        if (it != m_nodeUIs.end()) {
            m_nodeUIs.erase(it);
        }

        auto connectionIt = m_connectionUIs.begin();
        while (connectionIt != m_connectionUIs.end()) {
            if (connectionIt->sourceNodeId == m_selectedNodeId || connectionIt->targetNodeId == m_selectedNodeId) {
                connectionIt = m_connectionUIs.erase(connectionIt);
            } else {
                ++connectionIt;
            }
        }

        m_selectedNodeId = 0;
        m_propertiesPanel->setVisible(false);
    }
}

void ShaderEditorUI::duplicateSelectedNode() {
    if (!m_shaderGraph || m_selectedNodeId == 0) {
        return;
    }

    auto node = m_shaderGraph->getNodeById(m_selectedNodeId);
    if (!node) {
        return;
    }

    std::shared_ptr<ShaderNode> newNode;

    if (auto mathNode = std::dynamic_pointer_cast<MathNode>(node)) {
        newNode = ShaderNodeFactory::createMathNode(mathNode->getOperation());
    } else if (auto textureSampleNode = std::dynamic_pointer_cast<TextureSampleNode>(node)) {
        newNode = ShaderNodeFactory::createTextureSampleNode();
    } else if (auto vectorNode = std::dynamic_pointer_cast<VectorNode>(node)) {
        newNode = ShaderNodeFactory::createVectorNode(vectorNode->getComponents());
    } else if (auto inputNode = std::dynamic_pointer_cast<InputNode>(node)) {
        newNode = ShaderNodeFactory::createInputNode(inputNode->getInputType(), inputNode->getCustomName());
    } else if (auto outputNode = std::dynamic_pointer_cast<OutputNode>(node)) {
        newNode = ShaderNodeFactory::createOutputNode(outputNode->getOutputType(), outputNode->getCustomName());
    }

    if (newNode) {

        newNode->setPosition(node->getPosition() + glm::vec2(20, 20));

        if (m_shaderGraph->addNode(newNode)) {

            m_nodeUIs[newNode->getId()] = createNodeUI(newNode);

            handleNodeSelection(newNode->getId());
        }
    }
}

std::string ShaderEditorUI::getNodeTypeDisplayName(const std::string& nodeType) {
    if (nodeType == "MathAdd") return "Add";
    if (nodeType == "MathMultiply") return "Multiply";
    if (nodeType == "TextureSample") return "Texture Sample";
    if (nodeType == "Vector3") return "Vector3";
    if (nodeType == "InputPosition") return "Position";
    if (nodeType == "OutputColor") return "Color Output";

    return nodeType;
}

glm::vec4 ShaderEditorUI::getNodeColor(const std::string& category) {
    if (category == "Math") return {0.2f, 0.4f, 0.6f, 1.0f};
    if (category == "Texture") return {0.6f, 0.4f, 0.2f, 1.0f};
    if (category == "Input") return {0.2f, 0.6f, 0.4f, 1.0f};
    if (category == "Output") return {0.6f, 0.2f, 0.4f, 1.0f};

    return {0.4f, 0.4f, 0.4f, 1.0f};
}

glm::vec4 ShaderEditorUI::getPinColor(NodePin::Type type) {
    switch (type) {
        case NodePin::Type::FLOAT:
            return {0.9f, 0.5f, 0.1f, 1.0f}; // Orange
        case NodePin::Type::VEC2:
            return {0.1f, 0.5f, 0.9f, 1.0f}; // Blue
        case NodePin::Type::VEC3:
            return {0.1f, 0.9f, 0.5f, 1.0f}; // Green
        case NodePin::Type::VEC4:
            return {0.9f, 0.1f, 0.5f, 1.0f}; // Pink
        case NodePin::Type::INT:
            return {0.5f, 0.9f, 0.1f, 1.0f}; // Lime
        case NodePin::Type::BOOL:
            return {0.9f, 0.9f, 0.1f, 1.0f}; // Yellow
        case NodePin::Type::SAMPLER2D:
            return {0.5f, 0.1f, 0.9f, 1.0f}; // Purple
        case NodePin::Type::MATRIX:
            return {0.1f, 0.9f, 0.9f, 1.0f}; // Cyan
        default:
            return {0.7f, 0.7f, 0.7f, 1.0f}; // Gray
    }
}

} // namespace ElementalRenderer
