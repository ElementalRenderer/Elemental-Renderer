/**
 * @file ElementalGUIManager.h
 * @brief Manager class for ElementalGUI system
 */

#ifndef ELEMENTAL_RENDERER_ELEMENTAL_GUI_MANAGER_H
#define ELEMENTAL_RENDERER_ELEMENTAL_GUI_MANAGER_H

#include "ElementalGUI.h"
#include <memory>
#include <string>
#include <functional>
#include <vector>
#include <map>

struct GLFWwindow;

namespace ElementalRenderer {

class Scene;
class Camera;
class Light;
class Material;

/**
 * @brief Manager class for ElementalGUI system
 * 
 * This class provides higher-level GUI components and handling
 * built on top of the ElementalGUI system.
 */
class ElementalGUIManager {
public:
    /**
     * @brief Initialize the GUI manager
     * @param window GLFW window
     * @param width Window width
     * @param height Window height
     * @return true if initialization was successful
     */
    static bool initialize(GLFWwindow* window, int width, int height);
    
    /**
     * @brief Shutdown the GUI manager
     */
    static void shutdown();
    
    /**
     * @brief Begin a new GUI frame
     */
    static void beginFrame();
    
    /**
     * @brief End the GUI frame and render all UI elements
     */
    static void endFrame();
    
    /**
     * @brief Process mouse movement
     * @param xpos Mouse X position
     * @param ypos Mouse Y position
     */
    static void processMouseMove(double xpos, double ypos);
    
    /**
     * @brief Process mouse button events
     * @param button Mouse button
     * @param action Button action (press, release)
     * @param mods Modifier keys
     */
    static void processMouseButton(int button, int action, int mods);
    
    /**
     * @brief Process key events
     * @param key Key code
     * @param scancode System-specific scancode
     * @param action Key action (press, release, repeat)
     * @param mods Modifier keys
     */
    static void processKey(int key, int scancode, int action, int mods);
    
    /**
     * @brief Process character input
     * @param codepoint Unicode code point
     */
    static void processChar(unsigned int codepoint);
    
    /**
     * @brief Create a scene panel
     * @param scene Scene to edit
     * @param camera Camera to edit
     * @return Shared pointer to the created panel
     */
    static std::shared_ptr<Panel> createScenePanel(Scene& scene, Camera& camera);
    
    /**
     * @brief Create a material editor panel
     * @param material Material to edit
     * @return Shared pointer to the created panel
     */
    static std::shared_ptr<Panel> createMaterialPanel(Material& material);
    
    /**
     * @brief Create a lighting panel
     * @param lights Vector of lights to edit
     * @return Shared pointer to the created panel
     */
    static std::shared_ptr<Panel> createLightingPanel(std::vector<Light>& lights);
    
    /**
     * @brief Create a shader selection panel
     * @param shaderList List of available shaders
     * @param currentShader Reference to current shader index
     * @return Shared pointer to the created panel
     */
    static std::shared_ptr<Panel> createShaderPanel(const std::vector<std::string>& shaderList, int& currentShader);
    
    /**
     * @brief Create a post-processing panel
     * @param enabled Reference to enabled flag
     * @param effectType Reference to effect type
     * @param effectStrength Reference to effect strength
     * @return Shared pointer to the created panel
     */
    static std::shared_ptr<Panel> createPostProcessPanel(bool& enabled, int& effectType, float& effectStrength);
    
    /**
     * @brief Create a performance metrics panel
     * @param updateCallback Callback function to update metrics
     * @return Shared pointer to the created panel
     */
    static std::shared_ptr<Panel> createPerformancePanel(std::function<void(float&, float&)> updateCallback);
    
    /**
     * @brief Show a message dialog
     * @param title Dialog title
     * @param message Dialog message
     * @param type Message type (0=info, 1=warning, 2=error)
     */
    static void showMessageDialog(const std::string& title, const std::string& message, int type = 0);
    
    /**
     * @brief Show an input dialog
     * @param title Dialog title
     * @param label Input label
     * @param defaultValue Default input value
     * @param callback Function to call with the entered value
     */
    static void showInputDialog(const std::string& title, const std::string& label, 
                               const std::string& defaultValue, std::function<void(const std::string&)> callback);
    
    /**
     * @brief Update the scene panel if it exists
     * @param scene Scene to update with
     * @param camera Camera to update with
     */
    static void updateScenePanel(Scene& scene, Camera& camera);
    
    /**
     * @brief Update the performance metrics
     * @param frameTime Current frame time in milliseconds
     * @param fps Current frames per second
     */
    static void updatePerformanceMetrics(float frameTime, float fps);

private:
    static glm::vec2 s_mousePos;
    static bool s_initialized;
    static std::shared_ptr<Panel> s_scenePanel;
    static std::shared_ptr<Panel> s_materialPanel;
    static std::shared_ptr<Panel> s_lightingPanel;
    static std::shared_ptr<Panel> s_shaderPanel;
    static std::shared_ptr<Panel> s_postProcessPanel;
    static std::shared_ptr<Panel> s_performancePanel;
    
    // Dialog panels
    static std::shared_ptr<Panel> s_messageDialog;
    static std::shared_ptr<Panel> s_inputDialog;
    
    // Helper methods for dialog creation
    static void createMessageDialog();
    static void createInputDialog();
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_ELEMENTAL_GUI_MANAGER_H
