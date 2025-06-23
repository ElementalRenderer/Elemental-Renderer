#include <ElementalRenderer.h>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @file PostProcessingDemo.cpp
 * @brief Demonstrates post-processing effects with ElementalRenderer
 */

int main() {
    // Initialize the renderer
    ElementalRenderer::RendererOptions options;
    options.windowWidth = 1280;
    options.windowHeight = 720;
    options.windowTitle = "Post-Processing Demo";
    
    if (!ElementalRenderer::Renderer::initialize(options)) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return -1;
    }
    
    // Create a scene
    auto scene = std::make_shared<ElementalRenderer::Scene>("Post-Processing Scene");
    
    // Add a ground plane
    auto groundMesh = ElementalRenderer::Mesh::createPlane(20.0f, 20.0f);
    auto groundMaterial = std::make_shared<ElementalRenderer::Material>();
    groundMaterial->setDiffuseColor(glm::vec3(0.5f, 0.5f, 0.5f));
    groundMaterial->setSpecularColor(glm::vec3(0.2f, 0.2f, 0.2f));
    groundMaterial->setRoughness(0.8f);
    groundMesh->setMaterial(groundMaterial);
    groundMesh->setPosition(glm::vec3(0.0f, -2.0f, 0.0f));
    scene->addMesh(groundMesh, "Ground");
    
    // Add some colorful objects
    // Red cube
    auto redCubeMesh = ElementalRenderer::Mesh::createCube(1.0f);
    auto redMaterial = std::make_shared<ElementalRenderer::Material>();
    redMaterial->setDiffuseColor(glm::vec3(1.0f, 0.2f, 0.2f));
    redMaterial->setSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
    redMaterial->setRoughness(0.3f);
    redMaterial->setEmissiveColor(glm::vec3(0.8f, 0.0f, 0.0f));
    redMaterial->setEmissiveIntensity(0.5f);
    redCubeMesh->setMaterial(redMaterial);
    redCubeMesh->setPosition(glm::vec3(-3.0f, 0.0f, 0.0f));
    scene->addMesh(redCubeMesh, "RedCube");
    
    // Green sphere
    auto greenSphereMesh = ElementalRenderer::Mesh::createSphere(1.0f, 32, 32);
    auto greenMaterial = std::make_shared<ElementalRenderer::Material>();
    greenMaterial->setDiffuseColor(glm::vec3(0.2f, 1.0f, 0.2f));
    greenMaterial->setSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
    greenMaterial->setRoughness(0.1f);
    greenMaterial->setEmissiveColor(glm::vec3(0.0f, 0.8f, 0.0f));
    greenMaterial->setEmissiveIntensity(0.5f);
    greenSphereMesh->setMaterial(greenMaterial);
    greenSphereMesh->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    scene->addMesh(greenSphereMesh, "GreenSphere");
    
    // Blue cylinder
    auto blueCylinderMesh = ElementalRenderer::Mesh::createCylinder(0.5f, 2.0f, 32);
    auto blueMaterial = std::make_shared<ElementalRenderer::Material>();
    blueMaterial->setDiffuseColor(glm::vec3(0.2f, 0.2f, 1.0f));
    blueMaterial->setSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
    blueMaterial->setRoughness(0.2f);
    blueMaterial->setEmissiveColor(glm::vec3(0.0f, 0.0f, 0.8f));
    blueMaterial->setEmissiveIntensity(0.5f);
    blueCylinderMesh->setMaterial(blueMaterial);
    blueCylinderMesh->setPosition(glm::vec3(3.0f, 0.0f, 0.0f));
    scene->addMesh(blueCylinderMesh, "BlueCylinder");
    
    // Add lights
    auto directionalLight = std::make_shared<ElementalRenderer::Light>(
        ElementalRenderer::Light::Type::DIRECTIONAL,
        glm::vec3(1.0f, 1.0f, 1.0f),  // Color
        1.0f                          // Intensity
    );
    directionalLight->setDirection(glm::vec3(-0.5f, -1.0f, -0.3f));
    scene->addLight(directionalLight, "DirectionalLight");
    
    // Create a camera
    ElementalRenderer::Camera camera;
    camera.setPosition(glm::vec3(0.0f, 3.0f, 8.0f));
    camera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    camera.setPerspective(45.0f, 16.0f/9.0f, 0.1f, 100.0f);
    
    // Create post-processing effects
    auto bloomEffect = std::make_shared<ElementalRenderer::ShaderEffect>("Bloom");
    auto dofEffect = std::make_shared<ElementalRenderer::ShaderEffect>("DepthOfField");
    auto colorGradingEffect = std::make_shared<ElementalRenderer::ShaderEffect>("ColorGrading");
    auto vignetteEffect = std::make_shared<ElementalRenderer::ShaderEffect>("Vignette");
    auto filmGrainEffect = std::make_shared<ElementalRenderer::ShaderEffect>("FilmGrain");
    
    // Configure bloom effect
    bloomEffect->setParameter("threshold", 0.8f);
    bloomEffect->setParameter("intensity", 1.0f);
    
    // Configure depth of field effect
    dofEffect->setParameter("focusDistance", 8.0f);
    dofEffect->setParameter("focusRange", 4.0f);
    dofEffect->setParameter("blurAmount", 1.0f);
    
    // Configure color grading effect
    colorGradingEffect->setParameter("contrast", 1.1f);
    colorGradingEffect->setParameter("brightness", 1.0f);
    colorGradingEffect->setParameter("saturation", 1.2f);
    colorGradingEffect->setParameter("temperature", 0.0f);
    colorGradingEffect->setParameter("tint", 0.0f);
    
    // Configure vignette effect
    vignetteEffect->setParameter("intensity", 0.5f);
    vignetteEffect->setParameter("smoothness", 0.5f);
    
    // Configure film grain effect
    filmGrainEffect->setParameter("amount", 0.1f);
    
    // Create a post-processing shader
    auto postProcessShader = std::make_shared<ElementalRenderer::PostProcessShader>();
    postProcessShader->addEffect(bloomEffect);
    postProcessShader->addEffect(dofEffect);
    postProcessShader->addEffect(colorGradingEffect);
    postProcessShader->addEffect(vignetteEffect);
    postProcessShader->addEffect(filmGrainEffect);
    
    // Create a render graph
    auto renderGraph = std::make_shared<ElementalRenderer::RenderGraph>("PostProcessGraph");
    
    // Scene rendering pass
    auto scenePass = std::make_shared<ElementalRenderer::RenderPass>("ScenePass", [&]() {
        // Clear the framebuffer
        ElementalRenderer::Renderer::clear();
        
        // Render the scene
        ElementalRenderer::Renderer::renderScene(*scene, camera);
    });
    scenePass->addWriteResource("SceneColor");
    scenePass->addWriteResource("SceneDepth");
    renderGraph->addPass(scenePass);
    
    // Bloom pass
    auto bloomPass = std::make_shared<ElementalRenderer::RenderPass>("BloomPass", [&]() {
        bloomEffect->apply();
    });
    bloomPass->addReadResource("SceneColor");
    bloomPass->addWriteResource("BloomResult");
    renderGraph->addPass(bloomPass);
    
    // Depth of field pass
    auto dofPass = std::make_shared<ElementalRenderer::RenderPass>("DoFPass", [&]() {
        dofEffect->apply();
    });
    dofPass->addReadResource("BloomResult");
    dofPass->addReadResource("SceneDepth");
    dofPass->addWriteResource("DoFResult");
    renderGraph->addPass(dofPass);
    
    // Color grading pass
    auto colorGradingPass = std::make_shared<ElementalRenderer::RenderPass>("ColorGradingPass", [&]() {
        colorGradingEffect->apply();
    });
    colorGradingPass->addReadResource("DoFResult");
    colorGradingPass->addWriteResource("ColorGradingResult");
    renderGraph->addPass(colorGradingPass);
    
    // Final pass (vignette and film grain)
    auto finalPass = std::make_shared<ElementalRenderer::RenderPass>("FinalPass", [&]() {
        vignetteEffect->apply();
        filmGrainEffect->apply();
    });
    finalPass->addReadResource("ColorGradingResult");
    finalPass->addWriteResource("FinalResult");
    renderGraph->addPass(finalPass);
    
    // Build the render graph
    renderGraph->buildDependencyGraph();
    
    // Available post-processing effects
    std::vector<std::string> effectNames = {
        "None",
        "Bloom",
        "Depth of Field",
        "Color Grading",
        "Vignette",
        "Film Grain",
        "All Effects"
    };
    
    // Current effect index
    int currentEffectIndex = 6; // Start with all effects
    
    // Animation variables
    float time = 0.0f;
    
    // Main loop
    while (!ElementalRenderer::Renderer::shouldClose()) {
        // Update time
        time += 0.01f;
        
        // Animate objects
        redCubeMesh->setRotation(glm::vec3(time * 0.5f, time, 0.0f));
        greenSphereMesh->setPosition(glm::vec3(0.0f, sin(time) * 0.5f, 0.0f));
        blueCylinderMesh->setRotation(glm::vec3(0.0f, 0.0f, time * 0.7f));
        
        // Handle input for switching effects
        if (ElementalRenderer::Renderer::isKeyPressed('1')) {
            currentEffectIndex = 0; // None
        } else if (ElementalRenderer::Renderer::isKeyPressed('2')) {
            currentEffectIndex = 1; // Bloom
        } else if (ElementalRenderer::Renderer::isKeyPressed('3')) {
            currentEffectIndex = 2; // Depth of Field
        } else if (ElementalRenderer::Renderer::isKeyPressed('4')) {
            currentEffectIndex = 3; // Color Grading
        } else if (ElementalRenderer::Renderer::isKeyPressed('5')) {
            currentEffectIndex = 4; // Vignette
        } else if (ElementalRenderer::Renderer::isKeyPressed('6')) {
            currentEffectIndex = 5; // Film Grain
        } else if (ElementalRenderer::Renderer::isKeyPressed('7')) {
            currentEffectIndex = 6; // All Effects
        }
        
        // Update post-processing based on selected effect
        postProcessShader->clearEffects();
        
        if (currentEffectIndex == 1 || currentEffectIndex == 6) {
            postProcessShader->addEffect(bloomEffect);
        }
        if (currentEffectIndex == 2 || currentEffectIndex == 6) {
            postProcessShader->addEffect(dofEffect);
        }
        if (currentEffectIndex == 3 || currentEffectIndex == 6) {
            postProcessShader->addEffect(colorGradingEffect);
        }
        if (currentEffectIndex == 4 || currentEffectIndex == 6) {
            postProcessShader->addEffect(vignetteEffect);
        }
        if (currentEffectIndex == 5 || currentEffectIndex == 6) {
            postProcessShader->addEffect(filmGrainEffect);
        }
        
        // Execute the render graph
        renderGraph->execute();
        
        // Display current effect name
        ElementalRenderer::Renderer::drawText("Current Effect: " + effectNames[currentEffectIndex], 10, 30, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        ElementalRenderer::Renderer::drawText("Press 1-7 to switch effects", 10, 60, 0.4f, glm::vec3(0.8f, 0.8f, 0.8f));
        
        // Swap buffers and poll events
        ElementalRenderer::Renderer::swapBuffers();
        ElementalRenderer::Renderer::pollEvents();
    }
    
    // Clean up
    ElementalRenderer::Renderer::shutdown();
    
    return 0;
}