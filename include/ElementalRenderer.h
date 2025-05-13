/**
 * @file ElementalRenderer.h
 * @brief Main header file for the Elemental Renderer library
 */

#ifndef ELEMENTAL_RENDERER_H
#define ELEMENTAL_RENDERER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace ElementalRenderer {

/**
 * @brief Version information for Elemental Renderer
 */
struct Version {
    static constexpr int MAJOR = 0;
    static constexpr int MINOR = 1;
    static constexpr int PATCH = 0;
    
    static std::string toString() {
        return std::to_string(MAJOR) + "." + std::to_string(MINOR) + "." + std::to_string(PATCH);
    }
};

/**
 * @brief Forward declarations
 */
class Shader;
class Texture;
class Mesh;
class Material;
class Light;
class Camera;
class Scene;
class Renderer;

/**
 * @brief Initialization options for the renderer
 */
struct RendererOptions {
    int width = 800;
    int height = 600;
    std::string windowTitle = "Elemental Renderer";
    bool vsync = true;
    bool fullscreen = false;
    bool enableDebug = false;
};

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
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_H
