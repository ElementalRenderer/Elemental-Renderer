/**
 * @file ImGuiManager.h
 * @brief ImGui integration for ElementalRenderer
 */

#ifndef ELEMENTAL_RENDERER_IMGUI_MANAGER_H
#define ELEMENTAL_RENDERER_IMGUI_MANAGER_H

#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>

// Forward declarations
struct GLFWwindow;

namespace ElementalRenderer {

// Forward declarations
class Scene;
class Camera;
class Material;
class Light;
class RenderGraph;
class RenderPass;
class ShaderGraph;
class ShaderNode;
class LightingModel;

/**
 * @brief Manager class for ImGui integration
 */
class ImGuiManager {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the ImGuiManager instance
     */
    static ImGuiManager& getInstance();

    /**
     * @brief Initialize ImGui
     * @param window GLFW window
     * @param glslVersion GLSL version string (e.g., "#version 130")
     * @return true if initialization was successful
     */
    bool initialize(GLFWwindow* window, const char* glslVersion = "#version 130");

    /**
     * @brief Shutdown ImGui
     */
    void shutdown();

    /**
     * @brief Begin a new ImGui frame
     */
    void newFrame();

    /**
     * @brief Render ImGui
     */
    void render();

    /**
     * @brief Process events
     */
    void processEvents();

    /**
     * @brief Set the active scene
     * @param scene Pointer to the scene
     */
    void setActiveScene(Scene* scene);

    /**
     * @brief Set the active camera
     * @param camera Pointer to the camera
     */
    void setActiveCamera(Camera* camera);

    /**
     * @brief Set the active render graph
     * @param renderGraph Pointer to the render graph
     */
    void setActiveRenderGraph(RenderGraph* renderGraph);

    /**
     * @brief Set the active shader graph
     * @param shaderGraph Pointer to the shader graph
     */
    void setActiveShaderGraph(ShaderGraph* shaderGraph);

    /**
     * @brief Show the main menu bar
     */
    void showMainMenuBar();

    /**
     * @brief Show the scene hierarchy window
     * @param open Pointer to a boolean that controls the window visibility
     */
    void showSceneHierarchy(bool* open);

    /**
     * @brief Show the inspector window
     * @param open Pointer to a boolean that controls the window visibility
     */
    void showInspector(bool* open);

    /**
     * @brief Show the render graph window
     * @param open Pointer to a boolean that controls the window visibility
     */
    void showRenderGraph(bool* open);

    /**
     * @brief Show the shader graph window
     * @param open Pointer to a boolean that controls the window visibility
     */
    void showShaderGraph(bool* open);

    /**
     * @brief Show the performance window
     * @param open Pointer to a boolean that controls the window visibility
     */
    void showPerformance(bool* open);

    /**
     * @brief Show the material editor window
     * @param material Pointer to the material to edit
     * @param open Pointer to a boolean that controls the window visibility
     */
    void showMaterialEditor(Material* material, bool* open);

    /**
     * @brief Show the light editor window
     * @param light Pointer to the light to edit
     * @param open Pointer to a boolean that controls the window visibility
     */
    void showLightEditor(Light* light, bool* open);

    /**
     * @brief Show the BRDF editor window
     * @param lightingModel Pointer to the lighting model to edit
     * @param open Pointer to a boolean that controls the window visibility
     */
    void showBRDFEditor(LightingModel* lightingModel, bool* open);

    /**
     * @brief Show the demo window
     * @param open Pointer to a boolean that controls the window visibility
     */
    void showDemoWindow(bool* open);

private:
    // Private constructor to enforce singleton
    ImGuiManager();
    ~ImGuiManager();

    // Prevent copying
    ImGuiManager(const ImGuiManager&) = delete;
    ImGuiManager& operator=(const ImGuiManager&) = delete;

    // Helper methods
    void setupStyle();
    void drawNodeGraph(ShaderGraph* graph);
    void drawRenderGraph(RenderGraph* graph);
    void drawPerformanceGraph();
    void drawMaterialProperties(Material* material);
    void drawLightProperties(Light* light);
    void drawBRDFProperties(LightingModel* lightingModel);

    // Internal state
    GLFWwindow* m_window;
    Scene* m_activeScene;
    Camera* m_activeCamera;
    RenderGraph* m_activeRenderGraph;
    ShaderGraph* m_activeShaderGraph;
    Material* m_selectedMaterial;
    Light* m_selectedLight;
    ShaderNode* m_selectedNode;
    RenderPass* m_selectedRenderPass;
    
    // Performance metrics
    struct FrameMetrics {
        float frameTime;
        float renderTime;
        float uiTime;
        std::unordered_map<std::string, float> renderPassTimes;
    };
    
    std::vector<FrameMetrics> m_frameMetrics;
    int m_frameMetricsIndex;
    int m_frameMetricsCount;
    
    // Window states
    bool m_showSceneHierarchy;
    bool m_showInspector;
    bool m_showRenderGraph;
    bool m_showShaderGraph;
    bool m_showPerformance;
    bool m_showDemoWindow;
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_IMGUI_MANAGER_H