#include <ElementalRenderer.h>
#include <iostream>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @file PBRDemo.cpp
 * @brief Demonstrates Physically Based Rendering with ElementalRenderer
 */

int main() {
    // Initialize the renderer
    ElementalRenderer::RendererOptions options;
    options.windowWidth = 1280;
    options.windowHeight = 720;
    options.windowTitle = "PBR Demo";
    
    if (!ElementalRenderer::Renderer::initialize(options)) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return -1;
    }
    
    // Create a scene
    auto scene = std::make_shared<ElementalRenderer::Scene>("PBR Scene");
    
    // Add a ground plane
    auto groundMesh = ElementalRenderer::Mesh::createPlane(20.0f, 20.0f);
    auto groundMaterial = std::make_shared<ElementalRenderer::Material>();
    groundMaterial->setDiffuseColor(glm::vec3(0.5f, 0.5f, 0.5f));
    groundMaterial->setRoughness(0.9f);
    groundMaterial->setMetallic(0.0f);
    groundMesh->setMaterial(groundMaterial);
    groundMesh->setPosition(glm::vec3(0.0f, -2.0f, 0.0f));
    scene->addMesh(groundMesh, "Ground");
    
    // Create a grid of spheres with varying roughness and metallic properties
    const int gridSize = 7;
    const float spacing = 2.0f;
    const float startX = -((gridSize - 1) * spacing) / 2.0f;
    const float startZ = -((gridSize - 1) * spacing) / 2.0f;
    
    std::vector<std::shared_ptr<ElementalRenderer::Mesh>> spheres;
    
    for (int x = 0; x < gridSize; ++x) {
        for (int z = 0; z < gridSize; ++z) {
            auto sphereMesh = ElementalRenderer::Mesh::createSphere(0.5f, 32, 32);
            auto material = std::make_shared<ElementalRenderer::Material>();
            
            // Base color (albedo)
            material->setDiffuseColor(glm::vec3(0.9f, 0.9f, 0.9f));
            
            // Vary roughness along X axis (0.0 to 1.0)
            float roughness = static_cast<float>(x) / static_cast<float>(gridSize - 1);
            material->setRoughness(roughness);
            
            // Vary metallic along Z axis (0.0 to 1.0)
            float metallic = static_cast<float>(z) / static_cast<float>(gridSize - 1);
            material->setMetallic(metallic);
            
            sphereMesh->setMaterial(material);
            sphereMesh->setPosition(glm::vec3(
                startX + x * spacing,
                0.0f,
                startZ + z * spacing
            ));
            
            std::string name = "Sphere_" + std::to_string(x) + "_" + std::to_string(z);
            scene->addMesh(sphereMesh, name);
            spheres.push_back(sphereMesh);
        }
    }
    
    // Add lights
    // Main directional light (sun)
    auto sunLight = std::make_shared<ElementalRenderer::Light>(
        ElementalRenderer::Light::Type::DIRECTIONAL,
        glm::vec3(1.0f, 0.9f, 0.8f),  // Warm sunlight color
        3.0f                          // Intensity
    );
    sunLight->setDirection(glm::vec3(-0.5f, -1.0f, -0.3f));
    scene->addLight(sunLight, "SunLight");
    
    // Add some point lights with different colors
    auto redLight = std::make_shared<ElementalRenderer::Light>(
        ElementalRenderer::Light::Type::POINT,
        glm::vec3(1.0f, 0.2f, 0.2f),  // Red
        2.0f                          // Intensity
    );
    redLight->setPosition(glm::vec3(-5.0f, 2.0f, -5.0f));
    redLight->setRange(10.0f);
    scene->addLight(redLight, "RedLight");
    
    auto greenLight = std::make_shared<ElementalRenderer::Light>(
        ElementalRenderer::Light::Type::POINT,
        glm::vec3(0.2f, 1.0f, 0.2f),  // Green
        2.0f                          // Intensity
    );
    greenLight->setPosition(glm::vec3(5.0f, 2.0f, -5.0f));
    greenLight->setRange(10.0f);
    scene->addLight(greenLight, "GreenLight");
    
    auto blueLight = std::make_shared<ElementalRenderer::Light>(
        ElementalRenderer::Light::Type::POINT,
        glm::vec3(0.2f, 0.2f, 1.0f),  // Blue
        2.0f                          // Intensity
    );
    blueLight->setPosition(glm::vec3(0.0f, 2.0f, 5.0f));
    blueLight->setRange(10.0f);
    scene->addLight(blueLight, "BlueLight");
    
    // Create a camera
    ElementalRenderer::Camera camera;
    camera.setPosition(glm::vec3(0.0f, 5.0f, 10.0f));
    camera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    camera.setPerspective(45.0f, 16.0f/9.0f, 0.1f, 100.0f);
    
    // Set up PBR rendering
    auto& lightingManager = ElementalRenderer::LightingModelManager::getInstance();
    lightingManager.setActiveLightingModel("PBR");
    
    // Create a render graph
    auto renderGraph = std::make_shared<ElementalRenderer::RenderGraph>("PBRGraph");
    
    // Main rendering pass
    auto mainPass = std::make_shared<ElementalRenderer::RenderPass>("MainPass", [&]() {
        // Clear the framebuffer
        ElementalRenderer::Renderer::clear();
        
        // Get the PBR shader
        auto pbrShader = ElementalRenderer::PBRShader::getInstance();
        pbrShader->bind();
        
        // Set camera parameters
        pbrShader->setViewMatrix(camera.getViewMatrix());
        pbrShader->setProjectionMatrix(camera.getProjectionMatrix());
        pbrShader->setCameraPosition(camera.getPosition());
        
        // Set environment parameters (if using IBL)
        // pbrShader->setEnvironmentMap(environmentMap);
        // pbrShader->setIrradianceMap(irradianceMap);
        // pbrShader->setPrefilterMap(prefilterMap);
        // pbrShader->setBRDFLUT(brdfLUT);
        
        // Set lights
        pbrShader->setLightCount(scene->getLights().size());
        for (size_t i = 0; i < scene->getLights().size(); ++i) {
            const auto& light = scene->getLights()[i];
            pbrShader->setLightType(i, static_cast<int>(light->getType()));
            pbrShader->setLightPosition(i, light->getPosition());
            pbrShader->setLightDirection(i, light->getDirection());
            pbrShader->setLightColor(i, light->getColor() * light->getIntensity());
            pbrShader->setLightRange(i, light->getRange());
        }
        
        // Render the scene
        for (const auto& mesh : scene->getMeshes()) {
            pbrShader->setModelMatrix(mesh->getTransformMatrix());
            pbrShader->setMaterial(mesh->getMaterial());
            mesh->draw();
        }
    });
    renderGraph->addPass(mainPass);
    
    // Build the render graph
    renderGraph->buildDependencyGraph();
    
    // Animation variables
    float time = 0.0f;
    
    // Main loop
    while (!ElementalRenderer::Renderer::shouldClose()) {
        // Update time
        time += 0.01f;
        
        // Animate lights
        redLight->setPosition(glm::vec3(
            -5.0f + 2.0f * sin(time * 0.5f),
            2.0f + sin(time) * 0.5f,
            -5.0f + 2.0f * cos(time * 0.7f)
        ));
        
        greenLight->setPosition(glm::vec3(
            5.0f + 2.0f * sin(time * 0.6f),
            2.0f + sin(time * 1.1f) * 0.5f,
            -5.0f + 2.0f * cos(time * 0.8f)
        ));
        
        blueLight->setPosition(glm::vec3(
            2.0f * sin(time * 0.9f),
            2.0f + sin(time * 1.2f) * 0.5f,
            5.0f + 2.0f * cos(time * 0.5f)
        ));
        
        // Rotate camera around the scene
        float cameraX = 10.0f * sin(time * 0.1f);
        float cameraZ = 10.0f * cos(time * 0.1f);
        camera.setPosition(glm::vec3(cameraX, 5.0f, cameraZ));
        camera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
        
        // Execute the render graph
        renderGraph->execute();
        
        // Swap buffers and poll events
        ElementalRenderer::Renderer::swapBuffers();
        ElementalRenderer::Renderer::pollEvents();
    }
    
    // Clean up
    ElementalRenderer::Renderer::shutdown();
    
    return 0;
}