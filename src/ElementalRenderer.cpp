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
#include "Shaders/RenderGraph.h"
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // TODO: Set up error callback if debug mode is enabled
    if (options.enableDebug) {
        std::cout << "Debug mode enabled. OpenGL errors will be reported." << std::endl;
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
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

    // Create a render graph for this frame
    auto renderGraph = std::make_shared<RenderGraph>("SceneRenderGraph");

    // Create clear pass
    auto clearPass = std::make_shared<RenderPass>("ClearPass", [&]() {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    });
    clearPass->addWriteResource("FrameBuffer");
    renderGraph->addPass(clearPass);

    // Create shadow pass if there are lights
    if (!lights.empty()) {
        auto shadowPass = std::make_shared<RenderPass>("ShadowPass", [&]() {
            // Shadow map rendering logic would go here
            std::cout << "Executing Shadow Pass" << std::endl;
            // For each light that casts shadows, render the scene from light's perspective
            for (const auto& light : lights) {
                // Check if light casts shadows - using getCastShadows() if available, otherwise assume all lights cast shadows
                bool castsShadows = true;
                try {
                    castsShadows = light->getCastShadows();
                } catch (...) {
                    // If method doesn't exist, assume all lights cast shadows
                    castsShadows = true;
                }

                if (castsShadows) {
                    // Set up shadow map rendering
                    // Render shadow-casting objects
                }
            }
        });
        shadowPass->addReadResource("FrameBuffer");
        shadowPass->addWriteResource("ShadowMap");
        renderGraph->addPass(shadowPass);
    }

    // Create geometry pass
    auto geometryPass = std::make_shared<RenderPass>("GeometryPass", [&]() {
        std::cout << "Executing Geometry Pass" << std::endl;

        for (const auto& mesh : meshes) {
            auto material = mesh->getMaterial();
            if (!material) {
                continue;
            }

            auto shader = material->getShader();
            if (!shader) {
                continue;
            }

            material->apply();

            shader->setMat4("view", viewMatrix);
            shader->setMat4("projection", projectionMatrix);
            shader->setMat4("viewProjection", viewProjectionMatrix);
            shader->setVec3("camPos", cameraPosition);

            int lightCount = std::min(static_cast<int>(lights.size()), 4);
            shader->setInt("lightCount", lightCount);

            for (int i = 0; i < lightCount; ++i) {
                const auto& light = lights[i];
                shader->setVec3("lightPositions[" + std::to_string(i) + "]", light->getPosition());
                shader->setVec3("lightColors[" + std::to_string(i) + "]", light->getColor() * light->getIntensity());
            }

            mesh->render();
        }
    });
    geometryPass->addReadResource("FrameBuffer");
    if (!lights.empty()) {
        geometryPass->addReadResource("ShadowMap");
    }
    geometryPass->addWriteResource("GBuffer");
    renderGraph->addPass(geometryPass);

    // Create post-processing pass if needed
    auto postProcessPass = std::make_shared<RenderPass>("PostProcessPass", [&]() {
        std::cout << "Executing Post-Process Pass" << std::endl;
        // Apply post-processing effects
        // This would typically involve rendering a full-screen quad with a post-processing shader
    });
    postProcessPass->addReadResource("GBuffer");
    postProcessPass->addWriteResource("FinalImage");
    renderGraph->addPass(postProcessPass);

    // Create present pass
    auto presentPass = std::make_shared<RenderPass>("PresentPass", [&]() {
        glfwSwapBuffers(s_window);
        glfwPollEvents();
    });
    presentPass->addReadResource("FinalImage");
    renderGraph->addPass(presentPass);

    // Build and execute the render graph
    renderGraph->buildDependencyGraph();
    renderGraph->execute();
}

} // namespace ElementalRenderer
