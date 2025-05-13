/**
 * @file ElementalRenderer.cpp
 * @brief Implementation of the main Elemental Renderer functions
 */

#include "ElementalRenderer.h"
#include "Scene.h"
#include "Camera.h"
#include "Material.h"
#include "Mesh.h"
#include "Light.h"
#include "Shader.h"
#include <iostream>
#include <glad/glad.h>  // OpenGL loader... should be included before other OpenGL-related headers
#include <GLFW/glfw3.h> // input handling

namespace ElementalRenderer {

static bool s_initialized = false;
static RendererOptions s_options;
static GLFWwindow* s_window = nullptr;

bool Renderer::initialize(const RendererOptions& options) {
    if (s_initialized) {
        std::cout << "Elemental Renderer already initialized!" << std::endl;
        return true;
    }
    
    s_options = options;
    
    std::cout << "Initializing Elemental Renderer v" << Version::toString() << std::endl;
    std::cout << "Window size: " << options.width << "x" << options.height << std::endl;

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    s_window = glfwCreateWindow(options.width, options.height, options.windowTitle.c_str(), 
                              options.fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
    if (!s_window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(s_window);
    
    // Set VSync
    glfwSwapInterval(options.vsync ? 1 : 0);
    
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        glfwDestroyWindow(s_window);
        glfwTerminate();
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Set up error callback if debug mode is enabled
    if (options.enableDebug) {
        std::cout << "Debug mode enabled. OpenGL errors will be reported." << std::endl;
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        // Set up debug callback if needed
    }
    
    s_initialized = true;
    std::cout << "Elemental Renderer initialized successfully!" << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;
    
    return true;
}

void Renderer::shutdown() {
    if (!s_initialized) {
        std::cout << "Elemental Renderer not initialized!" << std::endl;
        return;
    }
    
    if (s_window) {
        glfwDestroyWindow(s_window);
        s_window = nullptr;
    }
    
    glfwTerminate();
    
    s_initialized = false;
    std::cout << "Elemental Renderer shutdown complete." << std::endl;
}

bool Renderer::isInitialized() {
    return s_initialized;
}

std::string Renderer::getVersion() {
    return Version::toString();
}

void Renderer::renderScene(const Scene& scene, const Camera& camera) {
    if (!s_initialized) {
        std::cout << "Cannot render: Elemental Renderer not initialized!" << std::endl;
        return;
    }
    
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    glm::mat4 viewProjectionMatrix = camera.getViewProjectionMatrix();
    glm::vec3 cameraPosition = camera.getPosition();
    
    std::cout << "Rendering scene: " << scene.getName() << std::endl;
    std::cout << "Using camera at position: " 
              << cameraPosition.x << ", "
              << cameraPosition.y << ", "
              << cameraPosition.z << std::endl;
    
    const auto& meshes = scene.getMeshes();
    const auto& lights = scene.getLights();
    
    std::cout << "Scene contains " << meshes.size() << " meshes and " 
              << lights.size() << " lights." << std::endl;
    
    // Clear the screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Process all meshes in the scene
    for (const auto& mesh : meshes) {
        auto material = mesh->getMaterial();
        if (!material) {
            continue;  // Skip meshes without materials
        }
        
        auto shader = material->getShader();
        if (!shader) {
            continue;  // Skip materials without shaders
        }
        
        // Apply material (this sets all uniforms)
        material->apply();
        
        // Set common uniforms for all shaders
        shader->setMat4("view", viewMatrix);
        shader->setMat4("projection", projectionMatrix);
        shader->setMat4("viewProjection", viewProjectionMatrix);
        shader->setVec3("camPos", cameraPosition);
        
        // Set light information
        int lightCount = std::min(static_cast<int>(lights.size()), 4);  // Limit to max 4 lights
        shader->setInt("lightCount", lightCount);
        
        for (int i = 0; i < lightCount; ++i) {
            const auto& light = lights[i];
            shader->setVec3("lightPositions[" + std::to_string(i) + "]", light->getPosition());
            shader->setVec3("lightColors[" + std::to_string(i) + "]", light->getColor() * light->getIntensity());
        }
        
        // Render the mesh
        mesh->render();
    }
    
    // Swap buffers to present the rendered image
    glfwSwapBuffers(s_window);
    glfwPollEvents();
}

} // namespace ElementalRenderer
