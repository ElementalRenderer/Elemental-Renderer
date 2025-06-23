#include "../include/Renderer.h"
#include "../include/ElementalRenderer.h"
#include <iostream>
#include <glad/glad.h>  // OpenGL loader... should be included before other OpenGL-related headers
#include <GLFW/glfw3.h>

namespace ElementalRenderer {

// Initialize static members
bool Renderer::s_initialized = false;
int Renderer::s_viewportWidth = 800;
int Renderer::s_viewportHeight = 600;
float Renderer::s_clearColor[4] = {0.2f, 0.2f, 0.2f, 1.0f};
std::unique_ptr<StyleShaderManager> Renderer::s_styleShaderManager = nullptr;

// Private constructor and destructor
Renderer::Renderer() {
    // Private constructor to enforce static usage
}

Renderer::~Renderer() {
    // Private destructor to enforce static usage
}

bool Renderer::initialize(const RendererOptions& options) {
    if (s_initialized) {
        std::cerr << "Renderer already initialized" << std::endl;
        return true;
    }

    s_viewportWidth = options.width;
    s_viewportHeight = options.height;

    // Initialize GLFW and OpenGL here
    // ...

    s_styleShaderManager = std::make_unique<StyleShaderManager>();
    if (!s_styleShaderManager->initialize()) {
        std::cerr << "Failed to initialize style shader manager" << std::endl;
        return false;
    }

    s_styleShaderManager->applyStyle(StyleShader::Style::DEFAULT);

    setupRenderState();

    s_initialized = true;
    return true;
}

void Renderer::shutdown() {
    if (!s_initialized) {
        return;
    }

    s_styleShaderManager.reset();
    // Cleanup GLFW and OpenGL here
    // ...

    s_initialized = false;
}

bool Renderer::isInitialized() {
    return s_initialized;
}

std::string Renderer::getVersion() {
    return Version::toString();
}

void Renderer::renderScene(const Scene& scene, const Camera& camera) {
    if (!s_initialized) {
        std::cerr << "Renderer not initialized" << std::endl;
        return;
    }

    glClearColor(s_clearColor[0], s_clearColor[1], s_clearColor[2], s_clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up camera and scene for rendering
    // ...

    renderSceneInternal();

    applyPostProcessing();
}

void Renderer::resize(int width, int height) {
    s_viewportWidth = width;
    s_viewportHeight = height;

    glViewport(0, 0, width, height);
}

void Renderer::setClearColor(float r, float g, float b, float a) {
    s_clearColor[0] = r;
    s_clearColor[1] = g;
    s_clearColor[2] = b;
    s_clearColor[3] = a;
}

bool Renderer::setShaderStyle(const std::string& styleName) {
    if (!s_initialized || !s_styleShaderManager) {
        return false;
    }
    return s_styleShaderManager->applyStyleByName(styleName);
}

bool Renderer::setShaderStyleByIndex(int index) {
    if (!s_initialized || !s_styleShaderManager) {
        return false;
    }
    return s_styleShaderManager->applyStyleByIndex(index);
}

std::string Renderer::getCurrentStyleName() {
    if (!s_initialized || !s_styleShaderManager) {
        return "";
    }
    return s_styleShaderManager->getCurrentStyleName();
}

std::vector<std::string> Renderer::getAvailableStyles() {
    if (!s_initialized || !s_styleShaderManager) {
        return {};
    }
    return s_styleShaderManager->getAvailableStyleNames();
}

std::vector<std::string> Renderer::getAvailableStyleDescriptions() {
    if (!s_initialized || !s_styleShaderManager) {
        return {};
    }
    return s_styleShaderManager->getAvailableStyleDescriptions();
}

void Renderer::setupRenderState() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void Renderer::renderSceneInternal() {
    if (!s_styleShaderManager) {
        return;
    }

    auto shader = s_styleShaderManager->getCurrentShader();
    if (!shader) {
        std::cerr << "No active shader for rendering" << std::endl;
        return;
    }

    shader->use();

    // This part needs to be updated to use the camera passed to renderScene
    // auto viewMatrix = camera->getViewMatrix();
    // auto projMatrix = camera->getProjectionMatrix();
    // shader->setMat4("view", viewMatrix);
    // shader->setMat4("projection", projMatrix);
    // shader->setVec3("viewPos", camera->getPosition());
}

void Renderer::applyPostProcessing() {
    if (!s_styleShaderManager) {
        return;
    }

    auto currentStyle = s_styleShaderManager->getCurrentStyle();
    if (currentStyle == StyleShader::Style::PIXEL_ART) {
        // Apply pixel art post-processing
    }
}

} // namespace ElementalRenderer
