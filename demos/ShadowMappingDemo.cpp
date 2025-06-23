#include <ElementalRenderer.h>
#include <iostream>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @file ShadowMappingDemo.cpp
 * @brief Demonstrates shadow mapping with ElementalRenderer
 */

int main() {
    // Initialize the renderer
    ElementalRenderer::RendererOptions options;
    options.windowWidth = 1280;
    options.windowHeight = 720;
    options.windowTitle = "Shadow Mapping Demo";
    
    if (!ElementalRenderer::Renderer::initialize(options)) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return -1;
    }
    
    // Create a scene
    auto scene = std::make_shared<ElementalRenderer::Scene>("Shadow Mapping Scene");
    
    // Add a ground plane
    auto groundMesh = ElementalRenderer::Mesh::createPlane(20.0f, 20.0f);
    auto groundMaterial = std::make_shared<ElementalRenderer::Material>();
    groundMaterial->setDiffuseColor(glm::vec3(0.5f, 0.5f, 0.5f));
    groundMaterial->setSpecularColor(glm::vec3(0.2f, 0.2f, 0.2f));
    groundMaterial->setRoughness(0.8f);
    groundMesh->setMaterial(groundMaterial);
    groundMesh->setPosition(glm::vec3(0.0f, -2.0f, 0.0f));
    scene->addMesh(groundMesh, "Ground");
    
    // Add some objects that will cast shadows
    auto cubeMesh = ElementalRenderer::Mesh::createCube(1.0f);
    auto cubeMaterial = std::make_shared<ElementalRenderer::Material>();
    cubeMaterial->setDiffuseColor(glm::vec3(0.8f, 0.2f, 0.2f));
    cubeMaterial->setSpecularColor(glm::vec3(0.8f, 0.8f, 0.8f));
    cubeMaterial->setRoughness(0.3f);
    cubeMesh->setMaterial(cubeMaterial);
    cubeMesh->setPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
    scene->addMesh(cubeMesh, "Cube");
    
    auto sphereMesh = ElementalRenderer::Mesh::createSphere(1.0f, 32, 32);
    auto sphereMaterial = std::make_shared<ElementalRenderer::Material>();
    sphereMaterial->setDiffuseColor(glm::vec3(0.2f, 0.8f, 0.2f));
    sphereMaterial->setSpecularColor(glm::vec3(0.8f, 0.8f, 0.8f));
    sphereMaterial->setRoughness(0.3f);
    sphereMesh->setMaterial(sphereMaterial);
    sphereMesh->setPosition(glm::vec3(2.0f, 0.0f, 0.0f));
    scene->addMesh(sphereMesh, "Sphere");
    
    // Add a directional light that will cast shadows
    auto directionalLight = std::make_shared<ElementalRenderer::Light>(
        ElementalRenderer::Light::Type::DIRECTIONAL,
        glm::vec3(1.0f, 1.0f, 1.0f),  // Color
        1.0f                          // Intensity
    );
    directionalLight->setDirection(glm::vec3(-0.5f, -1.0f, -0.3f));
    directionalLight->setCastShadows(true);
    directionalLight->setShadowMapSize(2048);
    scene->addLight(directionalLight, "DirectionalLight");
    
    // Create a camera
    ElementalRenderer::Camera camera;
    camera.setPosition(glm::vec3(0.0f, 3.0f, 8.0f));
    camera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    camera.setPerspective(45.0f, 16.0f/9.0f, 0.1f, 100.0f);
    
    // Create a render graph for shadow mapping
    auto renderGraph = std::make_shared<ElementalRenderer::RenderGraph>("ShadowMappingGraph");
    
    // Shadow pass - renders the scene from the light's perspective
    auto shadowPass = std::make_shared<ElementalRenderer::RenderPass>("ShadowPass", [&]() {
        // Get the shadow shader
        auto shadowShader = ElementalRenderer::ShadowShader::getInstance();
        shadowShader->bind();
        
        // Set up the light's view and projection matrices
        glm::mat4 lightView = glm::lookAt(
            glm::vec3(0.0f) - directionalLight->getDirection() * 10.0f,
            glm::vec3(0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 50.0f);
        shadowShader->setLightSpaceMatrix(lightProjection * lightView);
        
        // Render the scene to the shadow map
        for (const auto& mesh : scene->getMeshes()) {
            shadowShader->setModelMatrix(mesh->getTransformMatrix());
            mesh->draw();
        }
    });
    shadowPass->addWriteResource("ShadowMap");
    renderGraph->addPass(shadowPass);
    
    // Main rendering pass - uses the shadow map to render shadows
    auto mainPass = std::make_shared<ElementalRenderer::RenderPass>("MainPass", [&]() {
        // Clear the framebuffer
        ElementalRenderer::Renderer::clear();
        
        // Get the PBR shader with shadow mapping
        auto pbrShader = ElementalRenderer::PBRShader::getInstance();
        pbrShader->bind();
        
        // Set camera parameters
        pbrShader->setViewMatrix(camera.getViewMatrix());
        pbrShader->setProjectionMatrix(camera.getProjectionMatrix());
        pbrShader->setCameraPosition(camera.getPosition());
        
        // Set light parameters
        pbrShader->setLightDirection(directionalLight->getDirection());
        pbrShader->setLightColor(directionalLight->getColor() * directionalLight->getIntensity());
        
        // Set shadow parameters
        glm::mat4 lightView = glm::lookAt(
            glm::vec3(0.0f) - directionalLight->getDirection() * 10.0f,
            glm::vec3(0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 50.0f);
        pbrShader->setLightSpaceMatrix(lightProjection * lightView);
        pbrShader->setShadowMap(0); // Texture unit 0
        
        // Render the scene
        for (const auto& mesh : scene->getMeshes()) {
            pbrShader->setModelMatrix(mesh->getTransformMatrix());
            pbrShader->setMaterial(mesh->getMaterial());
            mesh->draw();
        }
    });
    mainPass->addReadResource("ShadowMap");
    renderGraph->addPass(mainPass);
    
    // Build the render graph
    renderGraph->buildDependencyGraph();
    
    // Animation variables
    float rotationAngle = 0.0f;
    
    // Main loop
    while (!ElementalRenderer::Renderer::shouldClose()) {
        // Update rotation
        rotationAngle += 0.01f;
        cubeMesh->setRotation(glm::vec3(0.0f, rotationAngle, 0.0f));
        sphereMesh->setRotation(glm::vec3(0.0f, -rotationAngle, 0.0f));
        
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