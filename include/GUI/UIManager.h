/**
 * @file UIManager.h
 * @brief UI management using ImGui for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_UI_MANAGER_H
#define ELEMENTAL_RENDERER_UI_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "../Scene.h"
#include "../Camera.h"
#include "../Light.h"
#include "../Material.h"
/**
 * @file UIManager.h
 * @brief UI management system using ImGui
 */

#ifndef ELEMENTAL_RENDERER_UIMANAGER_H
#define ELEMENTAL_RENDERER_UIMANAGER_H

#include <memory>
#include <string>
#include <functional>

struct GLFWwindow;

namespace ElementalRenderer {

class Scene;
class Camera;

namespace GUI {

class UIManager {
public:
    UIManager();
    ~UIManager();

    bool initialize(GLFWwindow* window, const std::string& glslVersion = "#version 130");
    void shutdown();
    
    void beginFrame();
    void endFrame();
    
    void setScene(std::shared_ptr<Scene> scene);
    void setCamera(std::shared_ptr<Camera> camera);
    
    void showDemoWindow(bool* open = nullptr);
    void showSceneHierarchy(bool* open = nullptr);
    void showInspector(bool* open = nullptr);
    void showViewport(bool* open = nullptr);
    void showStatsWindow(bool* open = nullptr);
    
    void registerCustomWindow(const std::string& name, std::function<void(bool*)> drawFunction);

private:
    GLFWwindow* m_window;
    std::shared_ptr<Scene> m_scene;
    std::shared_ptr<Camera> m_camera;
    
    struct CustomWindow {
        std::string name;
        std::function<void(bool*)> drawFunction;
        bool isOpen;
    };
    
    std::vector<CustomWindow> m_customWindows;
    
    void setupStyle();
    void drawMainMenuBar();
};

} // namespace GUI
} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_UIMANAGER_H
// Forward declare GLFW types to avoid including GLFW
struct GLFWwindow;

namespace ElementalRenderer {

/**
 * @brief UI Manager for handling ImGui integration and UI rendering
 */
class UIManager {
public:
    /**
     * @brief Initialize the UI system
     * @param window GLFW window pointer
     * @return true if initialization was successful, false otherwise
     */
    static bool initialize(GLFWwindow* window);
    
    /**
     * @brief Begin a new UI frame
     */
    static void beginFrame();
    
    /**
     * @brief End the UI frame and render UI elements
     */
    static void endFrame();
    
    /**
     * @brief Shutdown the UI system
     */
    static void shutdown();
    
    /**
     * @brief Render scene settings panel
     * @param scene Reference to the scene
     * @param camera Reference to the camera
     */
    static void renderScenePanel(Scene& scene, Camera& camera);
    
    /**
     * @brief Render material editor panel
     * @param material Reference to the material
     */
    static void renderMaterialEditor(Material& material);
    
    /**
     * @brief Render lighting panel
     * @param lights Vector of lights
     */
    static void renderLightingPanel(std::vector<Light>& lights);
    
    /**
     * @brief Render shader selection panel
     * @param shaderList Vector of shader names
     * @param currentShader Reference to current shader index
     */
    static void renderShaderPanel(const std::vector<std::string>& shaderList, int& currentShader);
    
    /**
     * @brief Render post-processing panel
     * @param enabled Reference to enabled flag
     * @param effectType Reference to effect type
     * @param effectStrength Reference to effect strength
     */
    static void renderPostProcessPanel(bool& enabled, int& effectType, float& effectStrength);
    
    /**
     * @brief Render performance metrics
     * @param frameTime Frame time in milliseconds
     * @param fps Frames per second
     */
    static void renderPerformanceMetrics(float frameTime, float fps);
    
    /**
     * @brief Show a simple message dialog
     * @param title Dialog title
     * @param message Dialog message
     * @param type Message type (0=info, 1=warning, 2=error)
     */
    static void showMessageDialog(const std::string& title, const std::string& message, int type = 0);
    
    /**
     * @brief Show an input dialog
     * @param title Dialog title
     * @param label Input field label
     * @param value Current/default value
     * @param callback Callback function receiving the entered value
     */
    static void showInputDialog(const std::string& title, const std::string& label, 
                                const std::string& value, std::function<void(const std::string&)> callback);
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_UI_MANAGER_H
