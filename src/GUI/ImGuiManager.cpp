/**
 * @file ImGuiManager.cpp
 * @brief Implementation of ImGui integration for ElementalRenderer
 */

#include "GUI/ImGuiManager.h"
#include "Scene.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"
#include "Shaders/RenderGraph.h"
#include "Shaders/ShaderGraph.h"
#include "Shaders/ShaderNode.h"
#include "Shaders/LightingModel.h"
#include "Shaders/LightingModelManager.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <chrono>
#include <cmath>

namespace ElementalRenderer {

// Static instance for singleton
static ImGuiManager* s_instance = nullptr;

ImGuiManager& ImGuiManager::getInstance() {
    if (!s_instance) {
        s_instance = new ImGuiManager();
    }
    return *s_instance;
}

ImGuiManager::ImGuiManager()
    : m_window(nullptr)
    , m_activeScene(nullptr)
    , m_activeCamera(nullptr)
    , m_activeRenderGraph(nullptr)
    , m_activeShaderGraph(nullptr)
    , m_selectedMaterial(nullptr)
    , m_selectedLight(nullptr)
    , m_selectedNode(nullptr)
    , m_selectedRenderPass(nullptr)
    , m_frameMetricsIndex(0)
    , m_frameMetricsCount(100) // Store 100 frames of metrics
    , m_showSceneHierarchy(true)
    , m_showInspector(true)
    , m_showRenderGraph(true)
    , m_showShaderGraph(false)
    , m_showPerformance(true)
    , m_showDemoWindow(false)
{
    m_frameMetrics.resize(m_frameMetricsCount);
}

ImGuiManager::~ImGuiManager() {
    shutdown();
}

bool ImGuiManager::initialize(GLFWwindow* window, const char* glslVersion) {
    m_window = window;
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Enable Multi-Viewport / Platform Windows
    
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);
    
    // Setup Dear ImGui style
    setupStyle();
    
    return true;
}

void ImGuiManager::shutdown() {
    if (ImGui::GetCurrentContext()) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}

void ImGuiManager::newFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Begin dockspace
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(2);
    
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    
    showMainMenuBar();
    
    // Show windows
    if (m_showSceneHierarchy) showSceneHierarchy(&m_showSceneHierarchy);
    if (m_showInspector) showInspector(&m_showInspector);
    if (m_showRenderGraph) showRenderGraph(&m_showRenderGraph);
    if (m_showShaderGraph) showShaderGraph(&m_showShaderGraph);
    if (m_showPerformance) showPerformance(&m_showPerformance);
    if (m_showDemoWindow) ImGui::ShowDemoWindow(&m_showDemoWindow);
    
    ImGui::End(); // DockSpace
}

void ImGuiManager::render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    // Update and Render additional Platform Windows
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void ImGuiManager::processEvents() {
    // ImGui_ImplGlfw_ProcessEvents is called by ImGui_ImplGlfw_NewFrame
}

void ImGuiManager::setActiveScene(Scene* scene) {
    m_activeScene = scene;
}

void ImGuiManager::setActiveCamera(Camera* camera) {
    m_activeCamera = camera;
}

void ImGuiManager::setActiveRenderGraph(RenderGraph* renderGraph) {
    m_activeRenderGraph = renderGraph;
}

void ImGuiManager::setActiveShaderGraph(ShaderGraph* shaderGraph) {
    m_activeShaderGraph = shaderGraph;
}

void ImGuiManager::showMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
                // TODO: Implement new scene
            }
            if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {
                // TODO: Implement open scene
            }
            if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
                // TODO: Implement save scene
            }
            if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S")) {
                // TODO: Implement save scene as
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                // TODO: Implement exit
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
                // TODO: Implement undo
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
                // TODO: Implement redo
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "Ctrl+X")) {
                // TODO: Implement cut
            }
            if (ImGui::MenuItem("Copy", "Ctrl+C")) {
                // TODO: Implement copy
            }
            if (ImGui::MenuItem("Paste", "Ctrl+V")) {
                // TODO: Implement paste
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Scene Hierarchy", nullptr, &m_showSceneHierarchy);
            ImGui::MenuItem("Inspector", nullptr, &m_showInspector);
            ImGui::MenuItem("Render Graph", nullptr, &m_showRenderGraph);
            ImGui::MenuItem("Shader Graph", nullptr, &m_showShaderGraph);
            ImGui::MenuItem("Performance", nullptr, &m_showPerformance);
            ImGui::Separator();
            ImGui::MenuItem("ImGui Demo", nullptr, &m_showDemoWindow);
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Tools")) {
            if (ImGui::MenuItem("Shader Editor")) {
                m_showShaderGraph = true;
            }
            if (ImGui::MenuItem("Material Editor")) {
                // TODO: Implement material editor
            }
            if (ImGui::MenuItem("BRDF Editor")) {
                // TODO: Implement BRDF editor
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("Documentation")) {
                // TODO: Open documentation
            }
            if (ImGui::MenuItem("About")) {
                // TODO: Show about dialog
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

void ImGuiManager::showSceneHierarchy(bool* open) {
    if (!ImGui::Begin("Scene Hierarchy", open)) {
        ImGui::End();
        return;
    }
    
    if (!m_activeScene) {
        ImGui::Text("No active scene");
        ImGui::End();
        return;
    }
    
    if (ImGui::Button("Add Object")) {
        ImGui::OpenPopup("AddObjectPopup");
    }
    
    if (ImGui::BeginPopup("AddObjectPopup")) {
        if (ImGui::MenuItem("Cube")) {
            // TODO: Add cube to scene
        }
        if (ImGui::MenuItem("Sphere")) {
            // TODO: Add sphere to scene
        }
        if (ImGui::MenuItem("Plane")) {
            // TODO: Add plane to scene
        }
        if (ImGui::MenuItem("Light")) {
            // TODO: Add light to scene
        }
        ImGui::EndPopup();
    }
    
    ImGui::Separator();
    
    // Display meshes
    if (ImGui::TreeNode("Meshes")) {
        for (size_t i = 0; i < m_activeScene->getMeshes().size(); ++i) {
            auto mesh = m_activeScene->getMeshes()[i];
            if (ImGui::Selectable(("Mesh " + std::to_string(i)).c_str())) {
                // Select mesh
                m_selectedMaterial = mesh->getMaterial().get();
            }
        }
        ImGui::TreePop();
    }
    
    // Display lights
    if (ImGui::TreeNode("Lights")) {
        for (size_t i = 0; i < m_activeScene->getLights().size(); ++i) {
            auto light = m_activeScene->getLights()[i];
            if (ImGui::Selectable(("Light " + std::to_string(i)).c_str())) {
                // Select light
                m_selectedLight = light.get();
            }
        }
        ImGui::TreePop();
    }
    
    ImGui::End();
}

void ImGuiManager::showInspector(bool* open) {
    if (!ImGui::Begin("Inspector", open)) {
        ImGui::End();
        return;
    }
    
    if (m_selectedMaterial) {
        if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
            drawMaterialProperties(m_selectedMaterial);
        }
    }
    
    if (m_selectedLight) {
        if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
            drawLightProperties(m_selectedLight);
        }
    }
    
    ImGui::End();
}

void ImGuiManager::showRenderGraph(bool* open) {
    if (!ImGui::Begin("Render Graph", open)) {
        ImGui::End();
        return;
    }
    
    if (!m_activeRenderGraph) {
        ImGui::Text("No active render graph");
        ImGui::End();
        return;
    }
    
    drawRenderGraph(m_activeRenderGraph);
    
    ImGui::End();
}

void ImGuiManager::showShaderGraph(bool* open) {
    if (!ImGui::Begin("Shader Graph", open)) {
        ImGui::End();
        return;
    }
    
    if (!m_activeShaderGraph) {
        ImGui::Text("No active shader graph");
        ImGui::End();
        return;
    }
    
    drawNodeGraph(m_activeShaderGraph);
    
    ImGui::End();
}

void ImGuiManager::showPerformance(bool* open) {
    if (!ImGui::Begin("Performance", open)) {
        ImGui::End();
        return;
    }
    
    drawPerformanceGraph();
    
    ImGui::End();
}

void ImGuiManager::showMaterialEditor(Material* material, bool* open) {
    if (!ImGui::Begin("Material Editor", open)) {
        ImGui::End();
        return;
    }
    
    if (!material) {
        ImGui::Text("No material selected");
        ImGui::End();
        return;
    }
    
    drawMaterialProperties(material);
    
    ImGui::End();
}

void ImGuiManager::showLightEditor(Light* light, bool* open) {
    if (!ImGui::Begin("Light Editor", open)) {
        ImGui::End();
        return;
    }
    
    if (!light) {
        ImGui::Text("No light selected");
        ImGui::End();
        return;
    }
    
    drawLightProperties(light);
    
    ImGui::End();
}

void ImGuiManager::showBRDFEditor(LightingModel* lightingModel, bool* open) {
    if (!ImGui::Begin("BRDF Editor", open)) {
        ImGui::End();
        return;
    }
    
    if (!lightingModel) {
        ImGui::Text("No lighting model selected");
        ImGui::End();
        return;
    }
    
    drawBRDFProperties(lightingModel);
    
    ImGui::End();
}

void ImGuiManager::showDemoWindow(bool* open) {
    ImGui::ShowDemoWindow(open);
}

void ImGuiManager::setupStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Colors
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
    colors[ImGuiCol_Border]                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
    colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_DockingPreview]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);
    
    // Style
    style.WindowPadding     = ImVec2(8, 8);
    style.FramePadding      = ImVec2(5, 2);
    style.CellPadding       = ImVec2(6, 6);
    style.ItemSpacing       = ImVec2(6, 6);
    style.ItemInnerSpacing  = ImVec2(6, 6);
    style.TouchExtraPadding = ImVec2(0, 0);
    style.IndentSpacing     = 25;
    style.ScrollbarSize     = 15;
    style.GrabMinSize       = 10;
    
    // Borders
    style.WindowBorderSize  = 1;
    style.ChildBorderSize   = 1;
    style.PopupBorderSize   = 1;
    style.FrameBorderSize   = 1;
    style.TabBorderSize     = 1;
    
    // Rounding
    style.WindowRounding    = 7;
    style.ChildRounding     = 4;
    style.FrameRounding     = 3;
    style.PopupRounding     = 4;
    style.ScrollbarRounding = 9;
    style.GrabRounding      = 3;
    style.LogSliderDeadzone = 4;
    style.TabRounding       = 4;
}

void ImGuiManager::drawNodeGraph(ShaderGraph* graph) {
    if (!graph) return;
    
    ImGui::Text("Shader Graph: %s", graph->getName().c_str());
    ImGui::Separator();
    
    // Node graph display
    ImGui::BeginChild("NodeGraph", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 offset = ImGui::GetCursorScreenPos();
    
    // Draw connections
    for (const auto& connection : graph->getConnections()) {
        auto sourceNode = connection.sourceNode;
        auto targetNode = connection.targetNode;
        
        if (!sourceNode || !targetNode) continue;
        
        ImVec2 sourcePos = ImVec2(
            offset.x + sourceNode->getPosition().x + sourceNode->getSize().x,
            offset.y + sourceNode->getPosition().y + 10.0f + connection.sourceOutputIndex * 20.0f
        );
        
        ImVec2 targetPos = ImVec2(
            offset.x + targetNode->getPosition().x,
            offset.y + targetNode->getPosition().y + 10.0f + connection.targetInputIndex * 20.0f
        );
        
        // Bezier curve
        ImVec2 cp1 = ImVec2(sourcePos.x + 50.0f, sourcePos.y);
        ImVec2 cp2 = ImVec2(targetPos.x - 50.0f, targetPos.y);
        
        drawList->AddBezierCubic(sourcePos, cp1, cp2, targetPos, IM_COL32(200, 200, 100, 255), 2.0f);
    }
    
    // Draw nodes
    for (const auto& node : graph->getNodes()) {
        ImVec2 nodePos = ImVec2(offset.x + node->getPosition().x, offset.y + node->getPosition().y);
        ImVec2 nodeSize = ImVec2(node->getSize().x, node->getSize().y);
        
        // Node background
        drawList->AddRectFilled(
            nodePos,
            ImVec2(nodePos.x + nodeSize.x, nodePos.y + nodeSize.y),
            IM_COL32(60, 60, 70, 255),
            4.0f
        );
        
        // Node border
        drawList->AddRect(
            nodePos,
            ImVec2(nodePos.x + nodeSize.x, nodePos.y + nodeSize.y),
            IM_COL32(100, 100, 140, 255),
            4.0f
        );
        
        // Node title
        drawList->AddText(
            ImVec2(nodePos.x + 5, nodePos.y + 5),
            IM_COL32(255, 255, 255, 255),
            node->getName().c_str()
        );
        
        // Input pins
        for (size_t i = 0; i < node->getInputPins().size(); ++i) {
            const auto& pin = node->getInputPins()[i];
            ImVec2 pinPos = ImVec2(nodePos.x, nodePos.y + 30.0f + i * 20.0f);
            
            // Pin circle
            drawList->AddCircleFilled(
                pinPos,
                5.0f,
                pin.isConnected ? IM_COL32(100, 200, 100, 255) : IM_COL32(200, 100, 100, 255)
            );
            
            // Pin name
            drawList->AddText(
                ImVec2(pinPos.x + 10, pinPos.y - 7),
                IM_COL32(200, 200, 200, 255),
                pin.name.c_str()
            );
        }
        
        // Output pins
        for (size_t i = 0; i < node->getOutputPins().size(); ++i) {
            const auto& pin = node->getOutputPins()[i];
            ImVec2 pinPos = ImVec2(nodePos.x + nodeSize.x, nodePos.y + 30.0f + i * 20.0f);
            
            // Pin circle
            drawList->AddCircleFilled(
                pinPos,
                5.0f,
                pin.isConnected ? IM_COL32(100, 200, 100, 255) : IM_COL32(200, 100, 100, 255)
            );
            
            // Pin name
            float textWidth = ImGui::CalcTextSize(pin.name.c_str()).x;
            drawList->AddText(
                ImVec2(pinPos.x - textWidth - 10, pinPos.y - 7),
                IM_COL32(200, 200, 200, 255),
                pin.name.c_str()
            );
        }
    }
    
    ImGui::EndChild();
}

void ImGuiManager::drawRenderGraph(RenderGraph* graph) {
    if (!graph) return;
    
    ImGui::Text("Render Graph: %s", graph->getName().c_str());
    ImGui::Separator();
    
    // Render graph display
    ImGui::BeginChild("RenderGraph", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 offset = ImGui::GetCursorScreenPos();
    
    // TODO: Implement render graph visualization
    // This would require additional information from the RenderGraph class
    // about the dependencies between render passes
    
    ImGui::EndChild();
}

void ImGuiManager::drawPerformanceGraph() {
    // Frame time graph
    if (ImGui::CollapsingHeader("Frame Time", ImGuiTreeNodeFlags_DefaultOpen)) {
        static float values[100] = { 0 };
        static int values_offset = 0;
        
        // Update values
        values[values_offset] = m_frameMetrics[m_frameMetricsIndex].frameTime;
        values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
        
        // Calculate average
        float average = 0.0f;
        for (int n = 0; n < IM_ARRAYSIZE(values); n++)
            average += values[n];
        average /= (float)IM_ARRAYSIZE(values);
        
        char overlay[32];
        sprintf(overlay, "Avg %.3f ms", average);
        
        ImGui::PlotLines("Frame Time", values, IM_ARRAYSIZE(values), values_offset, overlay, 0.0f, 33.3f, ImVec2(0, 80.0f));
        
        ImGui::Text("Frame Time: %.3f ms (%.1f FPS)", 
            m_frameMetrics[m_frameMetricsIndex].frameTime,
            1000.0f / m_frameMetrics[m_frameMetricsIndex].frameTime);
        
        ImGui::Text("Render Time: %.3f ms", m_frameMetrics[m_frameMetricsIndex].renderTime);
        ImGui::Text("UI Time: %.3f ms", m_frameMetrics[m_frameMetricsIndex].uiTime);
    }
    
    // Render pass times
    if (ImGui::CollapsingHeader("Render Pass Times", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (m_frameMetrics[m_frameMetricsIndex].renderPassTimes.empty()) {
            ImGui::Text("No render pass timing data available");
        } else {
            for (const auto& [name, time] : m_frameMetrics[m_frameMetricsIndex].renderPassTimes) {
                ImGui::Text("%s: %.3f ms", name.c_str(), time);
            }
        }
    }
    
    // GPU memory usage
    if (ImGui::CollapsingHeader("GPU Memory", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("GPU Memory Usage: N/A");
        // TODO: Implement GPU memory usage tracking
    }
}

void ImGuiManager::drawMaterialProperties(Material* material) {
    if (!material) return;
    
    // Material name
    static char nameBuffer[256] = "";
    strcpy(nameBuffer, material->getName().c_str());
    if (ImGui::InputText("Name", nameBuffer, IM_ARRAYSIZE(nameBuffer))) {
        material->setName(nameBuffer);
    }
    
    // Diffuse color
    glm::vec3 diffuseColor = material->getDiffuseColor();
    float diffuseColorF[3] = { diffuseColor.r, diffuseColor.g, diffuseColor.b };
    if (ImGui::ColorEdit3("Diffuse Color", diffuseColorF)) {
        material->setDiffuseColor(glm::vec3(diffuseColorF[0], diffuseColorF[1], diffuseColorF[2]));
    }
    
    // Specular color
    glm::vec3 specularColor = material->getSpecularColor();
    float specularColorF[3] = { specularColor.r, specularColor.g, specularColor.b };
    if (ImGui::ColorEdit3("Specular Color", specularColorF)) {
        material->setSpecularColor(glm::vec3(specularColorF[0], specularColorF[1], specularColorF[2]));
    }
    
    // Emissive color
    glm::vec3 emissiveColor = material->getEmissiveColor();
    float emissiveColorF[3] = { emissiveColor.r, emissiveColor.g, emissiveColor.b };
    if (ImGui::ColorEdit3("Emissive Color", emissiveColorF)) {
        material->setEmissiveColor(glm::vec3(emissiveColorF[0], emissiveColorF[1], emissiveColorF[2]));
    }
    
    // Emissive intensity
    float emissiveIntensity = material->getEmissiveIntensity();
    if (ImGui::SliderFloat("Emissive Intensity", &emissiveIntensity, 0.0f, 10.0f)) {
        material->setEmissiveIntensity(emissiveIntensity);
    }
    
    // Roughness
    float roughness = material->getRoughness();
    if (ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f)) {
        material->setRoughness(roughness);
    }
    
    // Metallic
    float metallic = material->getMetallic();
    if (ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f)) {
        material->setMetallic(metallic);
    }
    
    // Lighting model
    if (ImGui::BeginCombo("Lighting Model", material->getLightingModelName().c_str())) {
        auto& lightingManager = LightingModelManager::getInstance();
        for (const auto& modelName : lightingManager.getAvailableLightingModels()) {
            bool isSelected = (material->getLightingModelName() == modelName);
            if (ImGui::Selectable(modelName.c_str(), isSelected)) {
                material->setLightingModel(modelName);
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    
    // Textures
    if (ImGui::CollapsingHeader("Textures")) {
        // TODO: Implement texture selection
        ImGui::Text("Texture selection not implemented yet");
    }
    
    // Custom parameters
    if (ImGui::CollapsingHeader("Custom Parameters")) {
        // TODO: Implement custom parameter editing
        ImGui::Text("Custom parameter editing not implemented yet");
    }
}

void ImGuiManager::drawLightProperties(Light* light) {
    if (!light) return;
    
    // Light type
    int lightType = static_cast<int>(light->getType());
    const char* lightTypeNames[] = { "Directional", "Point", "Spot", "Area" };
    if (ImGui::Combo("Type", &lightType, lightTypeNames, IM_ARRAYSIZE(lightTypeNames))) {
        light->setType(static_cast<Light::Type>(lightType));
    }
    
    // Light color
    glm::vec3 color = light->getColor();
    float colorF[3] = { color.r, color.g, color.b };
    if (ImGui::ColorEdit3("Color", colorF)) {
        light->setColor(glm::vec3(colorF[0], colorF[1], colorF[2]));
    }
    
    // Light intensity
    float intensity = light->getIntensity();
    if (ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10.0f)) {
        light->setIntensity(intensity);
    }
    
    // Light position (for point and spot lights)
    if (light->getType() != Light::Type::DIRECTIONAL) {
        glm::vec3 position = light->getPosition();
        float positionF[3] = { position.x, position.y, position.z };
        if (ImGui::DragFloat3("Position", positionF, 0.1f)) {
            light->setPosition(glm::vec3(positionF[0], positionF[1], positionF[2]));
        }
    }
    
    // Light direction (for directional and spot lights)
    if (light->getType() == Light::Type::DIRECTIONAL || light->getType() == Light::Type::SPOT) {
        glm::vec3 direction = light->getDirection();
        float directionF[3] = { direction.x, direction.y, direction.z };
        if (ImGui::DragFloat3("Direction", directionF, 0.1f)) {
            light->setDirection(glm::normalize(glm::vec3(directionF[0], directionF[1], directionF[2])));
        }
    }
    
    // Light range (for point and spot lights)
    if (light->getType() != Light::Type::DIRECTIONAL) {
        float range = light->getRange();
        if (ImGui::SliderFloat("Range", &range, 0.1f, 100.0f)) {
            light->setRange(range);
        }
    }
    
    // Spot angle (for spot lights)
    if (light->getType() == Light::Type::SPOT) {
        float spotAngle = light->getSpotAngle();
        if (ImGui::SliderFloat("Spot Angle", &spotAngle, 0.0f, 90.0f)) {
            light->setSpotAngle(spotAngle);
        }
        
        float spotSoftness = light->getSpotSoftness();
        if (ImGui::SliderFloat("Spot Softness", &spotSoftness, 0.0f, 1.0f)) {
            light->setSpotSoftness(spotSoftness);
        }
    }
    
    // Shadow settings
    bool castShadows = light->getCastShadows();
    if (ImGui::Checkbox("Cast Shadows", &castShadows)) {
        light->setCastShadows(castShadows);
    }
    
    if (castShadows) {
        int shadowMapSize = light->getShadowMapSize();
        if (ImGui::SliderInt("Shadow Map Size", &shadowMapSize, 512, 4096)) {
            light->setShadowMapSize(shadowMapSize);
        }
        
        float shadowBias = light->getShadowBias();
        if (ImGui::SliderFloat("Shadow Bias", &shadowBias, 0.0f, 0.01f, "%.5f")) {
            light->setShadowBias(shadowBias);
        }
    }
}

void ImGuiManager::drawBRDFProperties(LightingModel* lightingModel) {
    if (!lightingModel) return;
    
    // BRDF name
    ImGui::Text("BRDF Model: %s", lightingModel->getName().c_str());
    
    // BRDF parameters
    // TODO: Implement BRDF parameter editing
    ImGui::Text("BRDF parameter editing not implemented yet");
}

} // namespace ElementalRenderer