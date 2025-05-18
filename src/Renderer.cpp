#include "../include/Renderer.h"
#include <iostream>
#include <glad/glad.h>  // OpenGL loader... should be included before other OpenGL-related headers
#include <GLFW/glfw3.h>

Renderer::Renderer()
    : viewportWidth(800), viewportHeight(600), camera(nullptr) {

    clearColor[0] = 0.2f;
    clearColor[1] = 0.2f;
    clearColor[2] = 0.2f;
    clearColor[3] = 1.0f;

    styleShaderManager = std::make_unique<StyleShaderManager>();
}

Renderer::~Renderer() {

}

bool Renderer::initialize(int width, int height) {
    viewportWidth = width;
    viewportHeight = height;

    if (!styleShaderManager->initialize()) {
        std::cerr << "Failed to initialize style shader manager" << std::endl;
        return false;
    }

    styleShaderManager->applyStyle(StyleShader::Style::DEFAULT);

    setupRenderState();

    return true;
}

void Renderer::resize(int width, int height) {
    viewportWidth = width;
    viewportHeight = height;

    glViewport(0, 0, width, height);
}

void Renderer::setCamera(std::shared_ptr<Camera> camera) {
    this->camera = camera;
}

void Renderer::render() {
    if (!camera) {
        std::cerr << "No camera set for rendering" << std::endl;
        return;
    }

    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderScene();

    applyPostProcessing();
}

void Renderer::setClearColor(float r, float g, float b, float a) {
    clearColor[0] = r;
    clearColor[1] = g;
    clearColor[2] = b;
    clearColor[3] = a;
}

bool Renderer::setShaderStyle(const std::string& styleName) {
    return styleShaderManager->applyStyleByName(styleName);
}

bool Renderer::setShaderStyleByIndex(int index) {
    return styleShaderManager->applyStyleByIndex(index);
}

std::string Renderer::getCurrentStyleName() const {
    return styleShaderManager->getCurrentStyleName();
}

std::vector<std::string> Renderer::getAvailableStyles() const {
    return styleShaderManager->getAvailableStyleNames();
}

std::vector<std::string> Renderer::getAvailableStyleDescriptions() const {
    return styleShaderManager->getAvailableStyleDescriptions();
}

std::vector<StyleShaderManager::StyleParameter> Renderer::getCurrentStyleParameters() const {
    return styleShaderManager->getCurrentStyleParameters();
}

bool Renderer::setStyleParameter(const std::string& paramName, float value) {
    return styleShaderManager->setStyleParameter(paramName, value);
}

void Renderer::resetStyleParameters() {
    styleShaderManager->resetStyleParameters();
}

void Renderer::setupRenderState() {

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

}

void Renderer::renderScene() {

    auto shader = styleShaderManager->getCurrentShader();
    if (!shader) {
        std::cerr << "No active shader for rendering" << std::endl;
        return;
    }

    shader->use();

    auto viewMatrix = camera->getViewMatrix();
    auto projMatrix = camera->getProjectionMatrix();

    shader->setMat4("view", viewMatrix);
    shader->setMat4("projection", projMatrix);
    shader->setVec3("viewPos", camera->getPosition());

}

void Renderer::applyPostProcessing() {
    auto currentStyle = styleShaderManager->getCurrentStyle();
    if (currentStyle == StyleShader::Style::PIXEL_ART) {

    }
}
