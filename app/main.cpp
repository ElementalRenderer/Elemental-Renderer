/**
 * @file main.cpp
 * @brief Example application using Elemental Renderer
 */

#include "ElementalRenderer.h"
#include "Scene.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "Light.h"
#include <iostream>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main() {
    std::cout << "Elemental Renderer Demo" << std::endl;
    std::cout << "Version: " << ElementalRenderer::Renderer::getVersion() << std::endl;
    
    ElementalRenderer::RendererOptions options;
    options.width = 1280;
    options.height = 720;
    options.windowTitle = "Elemental Renderer Demo";
    options.enableDebug = true;
    
    if (!ElementalRenderer::Renderer::initialize(options)) {
        std::cerr << "Failed to initialize Elemental Renderer!" << std::endl;
        return 1;
    }
    
    auto scene = ElementalRenderer::Scene::createTestScene("Demo Scene");
    if (!scene) {
        std::cerr << "Failed to create test scene!" << std::endl;
        ElementalRenderer::Renderer::shutdown();
        return 1;
    }
    
    auto camera = ElementalRenderer::Camera::createPerspectiveCamera(
        glm::vec3(0.0f, 2.0f, 5.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        45.0f,
        static_cast<float>(options.width) / options.height
    );
    
    std::cout << "Rendering a single frame..." << std::endl;
    ElementalRenderer::Renderer::renderScene(*scene, camera);
    
    ElementalRenderer::Renderer::shutdown();
    
    std::cout << "Elemental Renderer Demo completed successfully!" << std::endl;
    return 0;
}
