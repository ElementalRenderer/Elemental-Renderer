#include <ElementalRenderer.h>
#include <iostream>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @file NodeShaderDemo.cpp
 * @brief Demonstrates creating and using node-based shaders with ElementalRenderer
 */

int main() {
    // Initialize the renderer
    ElementalRenderer::RendererOptions options;
    options.windowWidth = 1280;
    options.windowHeight = 720;
    options.windowTitle = "Node Shader Demo";
    
    if (!ElementalRenderer::Renderer::initialize(options)) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return -1;
    }
    
    // Create a scene
    auto scene = std::make_shared<ElementalRenderer::Scene>("Node Shader Scene");
    
    // Add a ground plane
    auto groundMesh = ElementalRenderer::Mesh::createPlane(20.0f, 20.0f);
    auto groundMaterial = std::make_shared<ElementalRenderer::Material>();
    groundMaterial->setDiffuseColor(glm::vec3(0.5f, 0.5f, 0.5f));
    groundMaterial->setSpecularColor(glm::vec3(0.2f, 0.2f, 0.2f));
    groundMaterial->setRoughness(0.8f);
    groundMesh->setMaterial(groundMaterial);
    groundMesh->setPosition(glm::vec3(0.0f, -2.0f, 0.0f));
    scene->addMesh(groundMesh, "Ground");
    
    // Create a camera
    ElementalRenderer::Camera camera;
    camera.setPosition(glm::vec3(0.0f, 3.0f, 8.0f));
    camera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    camera.setPerspective(45.0f, 16.0f/9.0f, 0.1f, 100.0f);
    
    // Add a directional light
    auto directionalLight = std::make_shared<ElementalRenderer::Light>(
        ElementalRenderer::Light::Type::DIRECTIONAL,
        glm::vec3(1.0f, 1.0f, 1.0f),  // Color
        1.0f                          // Intensity
    );
    directionalLight->setDirection(glm::vec3(-0.5f, -1.0f, -0.3f));
    scene->addLight(directionalLight, "DirectionalLight");
    
    // Create different node-based shader examples
    
    // Example 1: Checkerboard pattern
    auto createCheckerboardShader = []() -> std::shared_ptr<ElementalRenderer::ShaderGraph> {
        auto graph = std::make_shared<ElementalRenderer::ShaderGraph>("CheckerboardShader");
        
        // Create nodes
        auto uvNode = ElementalRenderer::ShaderNodeFactory::createInputNode(
            ElementalRenderer::InputNode::InputType::UV);
        
        auto scaleNode = ElementalRenderer::ShaderNodeFactory::createMathNode(
            ElementalRenderer::MathNode::Operation::MULTIPLY);
        
        auto floorNode = ElementalRenderer::ShaderNodeFactory::createMathNode(
            ElementalRenderer::MathNode::Operation::FLOOR);
        
        auto modNode = ElementalRenderer::ShaderNodeFactory::createMathNode(
            ElementalRenderer::MathNode::Operation::MODULO);
        
        auto addNode = ElementalRenderer::ShaderNodeFactory::createMathNode(
            ElementalRenderer::MathNode::Operation::ADD);
        
        auto stepNode = ElementalRenderer::ShaderNodeFactory::createMathNode(
            ElementalRenderer::MathNode::Operation::STEP);
        
        auto colorMixNode = ElementalRenderer::ShaderNodeFactory::createMathNode(
            ElementalRenderer::MathNode::Operation::MIX);
        
        auto color1Node = ElementalRenderer::ShaderNodeFactory::createVectorNode(3);
        auto color2Node = ElementalRenderer::ShaderNodeFactory::createVectorNode(3);
        
        auto outputNode = ElementalRenderer::ShaderNodeFactory::createOutputNode(
            ElementalRenderer::OutputNode::OutputType::COLOR);
        
        // Set node positions for visualization
        uvNode->setPosition(glm::vec2(100, 100));
        scaleNode->setPosition(glm::vec2(300, 100));
        floorNode->setPosition(glm::vec2(500, 100));
        modNode->setPosition(glm::vec2(700, 100));
        addNode->setPosition(glm::vec2(900, 100));
        stepNode->setPosition(glm::vec2(1100, 100));
        colorMixNode->setPosition(glm::vec2(1300, 100));
        color1Node->setPosition(glm::vec2(1100, 200));
        color2Node->setPosition(glm::vec2(1100, 300));
        outputNode->setPosition(glm::vec2(1500, 100));
        
        // Set node parameters
        // Scale factor for checkerboard (higher = smaller checks)
        scaleNode->setParameter("B", "8.0");
        
        // Modulo value (2 for checkerboard)
        modNode->setParameter("B", "2.0");
        
        // Step threshold
        stepNode->setParameter("Edge", "1.0");
        
        // Colors for checkerboard
        color1Node->setParameter("X", "0.1");
        color1Node->setParameter("Y", "0.1");
        color1Node->setParameter("Z", "0.1");
        
        color2Node->setParameter("X", "0.9");
        color2Node->setParameter("Y", "0.9");
        color2Node->setParameter("Z", "0.9");
        
        // Add nodes to graph
        graph->addNode(uvNode);
        graph->addNode(scaleNode);
        graph->addNode(floorNode);
        graph->addNode(modNode);
        graph->addNode(addNode);
        graph->addNode(stepNode);
        graph->addNode(colorMixNode);
        graph->addNode(color1Node);
        graph->addNode(color2Node);
        graph->addNode(outputNode);
        
        // Connect nodes
        // UV -> Scale
        graph->addConnection(uvNode, 0, scaleNode, 0);
        
        // Scale -> Floor
        graph->addConnection(scaleNode, 0, floorNode, 0);
        
        // Floor -> Modulo
        graph->addConnection(floorNode, 0, modNode, 0);
        
        // Extract X and Y components from UV, floor them, mod them, and add them
        graph->addConnection(modNode, 0, addNode, 0);
        
        // Add -> Step
        graph->addConnection(addNode, 0, stepNode, 0);
        
        // Step -> Mix
        graph->addConnection(stepNode, 0, colorMixNode, 0);
        
        // Colors -> Mix
        graph->addConnection(color1Node, 0, colorMixNode, 1);
        graph->addConnection(color2Node, 0, colorMixNode, 2);
        
        // Mix -> Output
        graph->addConnection(colorMixNode, 0, outputNode, 0);
        
        return graph;
    };
    
    // Example 2: Procedural wood grain
    auto createWoodShader = []() -> std::shared_ptr<ElementalRenderer::ShaderGraph> {
        auto graph = std::make_shared<ElementalRenderer::ShaderGraph>("WoodShader");
        
        // Create nodes
        auto positionNode = ElementalRenderer::ShaderNodeFactory::createInputNode(
            ElementalRenderer::InputNode::InputType::POSITION);
        
        auto timeNode = ElementalRenderer::ShaderNodeFactory::createInputNode(
            ElementalRenderer::InputNode::InputType::TIME);
        
        auto noiseNode = ElementalRenderer::ShaderNodeFactory::createCustomNode("PerlinNoise");
        
        auto sinNode = ElementalRenderer::ShaderNodeFactory::createMathNode(
            ElementalRenderer::MathNode::Operation::SIN);
        
        auto absNode = ElementalRenderer::ShaderNodeFactory::createMathNode(
            ElementalRenderer::MathNode::Operation::ABS);
        
        auto powNode = ElementalRenderer::ShaderNodeFactory::createMathNode(
            ElementalRenderer::MathNode::Operation::POWER);
        
        auto colorMixNode = ElementalRenderer::ShaderNodeFactory::createMathNode(
            ElementalRenderer::MathNode::Operation::MIX);
        
        auto darkWoodNode = ElementalRenderer::ShaderNodeFactory::createVectorNode(3);
        auto lightWoodNode = ElementalRenderer::ShaderNodeFactory::createVectorNode(3);
        
        auto outputNode = ElementalRenderer::ShaderNodeFactory::createOutputNode(
            ElementalRenderer::OutputNode::OutputType::COLOR);
        
        // Set node parameters
        // Wood colors
        darkWoodNode->setParameter("X", "0.3");
        darkWoodNode->setParameter("Y", "0.2");
        darkWoodNode->setParameter("Z", "0.1");
        
        lightWoodNode->setParameter("X", "0.6");
        lightWoodNode->setParameter("Y", "0.4");
        lightWoodNode->setParameter("Z", "0.2");
        
        // Power for contrast
        powNode->setParameter("B", "1.5");
        
        // Add nodes to graph
        graph->addNode(positionNode);
        graph->addNode(timeNode);
        graph->addNode(noiseNode);
        graph->addNode(sinNode);
        graph->addNode(absNode);
        graph->addNode(powNode);
        graph->addNode(colorMixNode);
        graph->addNode(darkWoodNode);
        graph->addNode(lightWoodNode);
        graph->addNode(outputNode);
        
        // Connect nodes
        // Position -> Noise
        graph->addConnection(positionNode, 0, noiseNode, 0);
        
        // Time -> Noise (animation)
        graph->addConnection(timeNode, 0, noiseNode, 1);
        
        // Noise -> Sin
        graph->addConnection(noiseNode, 0, sinNode, 0);
        
        // Sin -> Abs
        graph->addConnection(sinNode, 0, absNode, 0);
        
        // Abs -> Pow
        graph->addConnection(absNode, 0, powNode, 0);
        
        // Pow -> Mix
        graph->addConnection(powNode, 0, colorMixNode, 0);
        
        // Colors -> Mix
        graph->addConnection(darkWoodNode, 0, colorMixNode, 1);
        graph->addConnection(lightWoodNode, 0, colorMixNode, 2);
        
        // Mix -> Output
        graph->addConnection(colorMixNode, 0, outputNode, 0);
        
        return graph;
    };
    
    // Example 3: Plasma effect
    auto createPlasmaShader = []() -> std::shared_ptr<ElementalRenderer::ShaderGraph> {
        auto graph = std::make_shared<ElementalRenderer::ShaderGraph>("PlasmaShader");
        
        // Create nodes
        auto uvNode = ElementalRenderer::ShaderNodeFactory::createInputNode(
            ElementalRenderer::InputNode::InputType::UV);
        
        auto timeNode = ElementalRenderer::ShaderNodeFactory::createInputNode(
            ElementalRenderer::InputNode::InputType::TIME);
        
        auto sinTime = ElementalRenderer::ShaderNodeFactory::createMathNode(
            ElementalRenderer::MathNode::Operation::SIN);
        
        auto cosTime = ElementalRenderer::ShaderNodeFactory::createMathNode(
            ElementalRenderer::MathNode::Operation::COS);
        
        auto sin1 = ElementalRenderer::ShaderNodeFactory::createMathNode(
            ElementalRenderer::MathNode::Operation::SIN);
        
        auto sin2 = ElementalRenderer::ShaderNodeFactory::createMathNode(
            ElementalRenderer::MathNode::Operation::SIN);
        
        auto sin3 = ElementalRenderer::ShaderNodeFactory::createMathNode(
            ElementalRenderer::MathNode::Operation::SIN);
        
        auto add1 = ElementalRenderer::ShaderNodeFactory::createMathNode(
            ElementalRenderer::MathNode::Operation::ADD);
        
        auto add2 = ElementalRenderer::ShaderNodeFactory::createMathNode(
            ElementalRenderer::MathNode::Operation::ADD);
        
        auto normalizeNode = ElementalRenderer::ShaderNodeFactory::createMathNode(
            ElementalRenderer::MathNode::Operation::NORMALIZE);
        
        auto hsvToRgbNode = ElementalRenderer::ShaderNodeFactory::createCustomNode("HSVToRGB");
        
        auto outputNode = ElementalRenderer::ShaderNodeFactory::createOutputNode(
            ElementalRenderer::OutputNode::OutputType::COLOR);
        
        // Add nodes to graph
        graph->addNode(uvNode);
        graph->addNode(timeNode);
        graph->addNode(sinTime);
        graph->addNode(cosTime);
        graph->addNode(sin1);
        graph->addNode(sin2);
        graph->addNode(sin3);
        graph->addNode(add1);
        graph->addNode(add2);
        graph->addNode(normalizeNode);
        graph->addNode(hsvToRgbNode);
        graph->addNode(outputNode);
        
        // Connect nodes
        // Time -> Sin/Cos
        graph->addConnection(timeNode, 0, sinTime, 0);
        graph->addConnection(timeNode, 0, cosTime, 0);
        
        // UV + Time -> Sin nodes
        graph->addConnection(uvNode, 0, sin1, 0);
        graph->addConnection(sinTime, 0, sin1, 1);
        
        graph->addConnection(uvNode, 1, sin2, 0);
        graph->addConnection(cosTime, 0, sin2, 1);
        
        // Add sin results
        graph->addConnection(sin1, 0, add1, 0);
        graph->addConnection(sin2, 0, add1, 1);
        
        // One more sin
        graph->addConnection(add1, 0, sin3, 0);
        
        // Add final sin
        graph->addConnection(sin3, 0, add2, 0);
        graph->addConnection(timeNode, 0, add2, 1);
        
        // Normalize to 0-1 range
        graph->addConnection(add2, 0, normalizeNode, 0);
        
        // Convert to RGB
        graph->addConnection(normalizeNode, 0, hsvToRgbNode, 0);
        
        // Output
        graph->addConnection(hsvToRgbNode, 0, outputNode, 0);
        
        return graph;
    };
    
    // Create shader graphs
    auto checkerboardShader = createCheckerboardShader();
    auto woodShader = createWoodShader();
    auto plasmaShader = createPlasmaShader();
    
    // Create materials with these shaders
    auto checkerboardMaterial = std::make_shared<ElementalRenderer::Material>();
    checkerboardMaterial->setShaderGraph(checkerboardShader);
    
    auto woodMaterial = std::make_shared<ElementalRenderer::Material>();
    woodMaterial->setShaderGraph(woodShader);
    
    auto plasmaMaterial = std::make_shared<ElementalRenderer::Material>();
    plasmaMaterial->setShaderGraph(plasmaShader);
    
    // Create objects with these materials
    auto sphereMesh = ElementalRenderer::Mesh::createSphere(1.0f, 32, 32);
    sphereMesh->setMaterial(checkerboardMaterial);
    sphereMesh->setPosition(glm::vec3(-3.0f, 0.0f, 0.0f));
    scene->addMesh(sphereMesh, "CheckerboardSphere");
    
    auto cubeMesh = ElementalRenderer::Mesh::createCube(1.5f);
    cubeMesh->setMaterial(woodMaterial);
    cubeMesh->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    scene->addMesh(cubeMesh, "WoodCube");
    
    auto torusMesh = ElementalRenderer::Mesh::createTorus(1.0f, 0.3f, 32, 32);
    torusMesh->setMaterial(plasmaMaterial);
    torusMesh->setPosition(glm::vec3(3.0f, 0.0f, 0.0f));
    scene->addMesh(torusMesh, "PlasmaTorus");
    
    // Get the shader editor manager
    auto& editorManager = ElementalRenderer::ShaderEditorManager::getInstance();
    
    // Initialize the editor
    editorManager.initialize(1280, 720);
    
    // Set the active shader graph for editing
    editorManager.setActiveShaderGraph(checkerboardShader);
    
    // Animation variables
    float time = 0.0f;
    bool showEditor = false;
    int activeShaderIndex = 0;
    
    // Main loop
    while (!ElementalRenderer::Renderer::shouldClose()) {
        // Update time
        time += 0.01f;
        
        // Rotate objects
        sphereMesh->setRotation(glm::vec3(0.0f, time * 0.2f, 0.0f));
        cubeMesh->setRotation(glm::vec3(time * 0.1f, time * 0.1f, 0.0f));
        torusMesh->setRotation(glm::vec3(time * 0.1f, 0.0f, time * 0.1f));
        
        // Handle input for showing/hiding editor
        if (ElementalRenderer::Renderer::isKeyPressed('E')) {
            showEditor = !showEditor;
            if (showEditor) {
                editorManager.showEditor();
            } else {
                editorManager.hideEditor();
            }
        }
        
        // Handle input for switching active shader
        if (ElementalRenderer::Renderer::isKeyPressed('1')) {
            activeShaderIndex = 0;
            editorManager.setActiveShaderGraph(checkerboardShader);
        } else if (ElementalRenderer::Renderer::isKeyPressed('2')) {
            activeShaderIndex = 1;
            editorManager.setActiveShaderGraph(woodShader);
        } else if (ElementalRenderer::Renderer::isKeyPressed('3')) {
            activeShaderIndex = 2;
            editorManager.setActiveShaderGraph(plasmaShader);
        }
        
        // Update shader parameters
        // Update time parameter for animated shaders
        woodMaterial->setParameter("Time", time);
        plasmaMaterial->setParameter("Time", time);
        
        // Render the scene
        ElementalRenderer::Renderer::clear();
        ElementalRenderer::Renderer::renderScene(*scene, camera);
        
        // Draw UI text
        ElementalRenderer::Renderer::drawText("Press E to toggle shader editor", 10, 30, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        ElementalRenderer::Renderer::drawText("Press 1-3 to switch active shader", 10, 60, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        
        std::string activeShaderName;
        switch (activeShaderIndex) {
            case 0: activeShaderName = "Checkerboard"; break;
            case 1: activeShaderName = "Wood"; break;
            case 2: activeShaderName = "Plasma"; break;
        }
        ElementalRenderer::Renderer::drawText("Active Shader: " + activeShaderName, 10, 90, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        
        // Update editor if visible
        if (showEditor) {
            editorManager.update();
        }
        
        // Swap buffers and poll events
        ElementalRenderer::Renderer::swapBuffers();
        ElementalRenderer::Renderer::pollEvents();
    }
    
    // Clean up
    ElementalRenderer::Renderer::shutdown();
    
    return 0;
}