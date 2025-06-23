/**
 * @file Renderer.h
 * @brief Renderer implementation for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_RENDERER_H
#define ELEMENTAL_RENDERER_RENDERER_H

#include "Camera.h"
#include "Scene.h"
#include <memory>
#include <string>
#include <vector>

namespace ElementalRenderer {

// Forward declarations
class StyleShaderManager;
struct RendererOptions;

/**
 * @brief Main renderer class for initializing and managing the rendering process
 */
class Renderer {
public:
    /**
     * @brief Initialize the renderer with given options
     * @param options Configuration options for the renderer
     * @return true if initialization was successful, false otherwise
     */
    static bool initialize(const RendererOptions& options = RendererOptions());

    /**
     * @brief Shut down the renderer and clean up resources
     */
    static void shutdown();

    /**
     * @brief Check if the renderer is currently initialized
     * @return true if the renderer is initialized, false otherwise
     */
    static bool isInitialized();

    /**
     * @brief Get the version of the Elemental Renderer
     * @return String representation of the version
     */
    static std::string getVersion();

    /**
     * @brief Render a scene using the specified camera
     * @param scene The scene to render
     * @param camera The camera to use for rendering
     */
    static void renderScene(const Scene& scene, const Camera& camera);

    // Additional methods from the original Renderer.h
    static void resize(int width, int height);
    static void setClearColor(float r, float g, float b, float a = 1.0f);
    static bool setShaderStyle(const std::string& styleName);
    static bool setShaderStyleByIndex(int index);
    static std::string getCurrentStyleName();
    static std::vector<std::string> getAvailableStyles();
    static std::vector<std::string> getAvailableStyleDescriptions();

private:
    // Private constructor to enforce static usage
    Renderer();
    ~Renderer();

    // Internal state
    static bool s_initialized;
    static int s_viewportWidth;
    static int s_viewportHeight;
    static float s_clearColor[4];
    static std::unique_ptr<StyleShaderManager> s_styleShaderManager;

    // Internal rendering methods
    static void setupRenderState();
    static void renderSceneInternal();
    static void applyPostProcessing();
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_RENDERER_H
