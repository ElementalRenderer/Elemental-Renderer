/**
 * @file SceneEditor.h
 * @brief Scene editor state management for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_SCENE_EDITOR_H
#define ELEMENTAL_RENDERER_SCENE_EDITOR_H

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "../Scene.h"
#include "../Camera.h"

namespace ElementalRenderer {

/**
 * @brief Editor modes for the scene editor
 */
enum class EditorMode {
    SCENE,
    MATERIAL,
    LIGHTING,
    CAMERA,
    ANIMATION
};

/**
 * @brief Stores scene state for undo/redo functionality
 */
struct SceneHistoryState {
    // Serialized scene state for undo/redo
    std::string serializedScene;
    std::string description;
};

/**
 * @brief Scene editor state management
 */
class SceneEditor {
public:
    /**
     * @brief Default constructor
     */
    SceneEditor();
    
    /**
     * @brief Destructor
     */
    ~SceneEditor();
    
    /**
     * @brief Initialize the scene editor
     * @param scene Pointer to the scene
     * @param camera Pointer to the camera
     * @return true if initialization was successful, false otherwise
     */
    bool initialize(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);
    
    /**
     * @brief Get the current editor mode
     * @return Current editor mode
     */
    EditorMode getCurrentMode() const;
    
    /**
     * @brief Set the current editor mode
     * @param mode Editor mode to set
     */
    void setCurrentMode(EditorMode mode);
    
    /**
     * @brief Get the selected object ID
     * @return ID of the selected object, or -1 if none selected
     */
    int getSelectedObjectId() const;
    
    /**
     * @brief Set the selected object ID
     * @param id ID of the object to select
     */
    void setSelectedObjectId(int id);
    
    /**
     * @brief Check if the editor is in dragging mode
     * @return true if dragging, false otherwise
     */
    bool isDragging() const;
    
    /**
     * @brief Set the dragging state
     * @param dragging true to enable dragging, false to disable
     */
    void setDragging(bool dragging);
    
    /**
     * @brief Check if the editor is in play mode
     * @return true if playing, false otherwise
     */
    bool isPlaying() const;
    
    /**
     * @brief Set the play mode
     * @param playing true to enable play mode, false to disable
     */
    void setPlaying(bool playing);
    
    /**
     * @brief Save the current scene state to the undo stack
     * @param description Description of the action
     */
    void saveState(const std::string& description);
    
    /**
     * @brief Undo the last action
     * @return true if the undo was successful, false otherwise
     */
    bool undo();
    
    /**
     * @brief Redo the last undone action
     * @return true if the redo was successful, false otherwise
     */
    bool redo();
    
    /**
     * @brief Check if undo is available
     * @return true if undo is available, false otherwise
     */
    bool canUndo() const;
    
    /**
     * @brief Check if redo is available
     * @return true if redo is available, false otherwise
     */
    bool canRedo() const;
    
    /**
     * @brief Get the scene being edited
     * @return Shared pointer to the scene
     */
    std::shared_ptr<Scene> getScene() const;
    
    /**
     * @brief Get the camera being used
     * @return Shared pointer to the camera
     */
    std::shared_ptr<Camera> getCamera() const;
    
    /**
     * @brief Render the scene editor UI
     */
    void renderUI();
    
    /**
     * @brief Process input for the scene editor
     * @param key Key code
     * @param action GLFW action (press, release, etc.)
     * @param mods Modifier keys
     */
    void processKeyInput(int key, int action, int mods);
    
    /**
     * @brief Process mouse input for the scene editor
     * @param button Mouse button
     * @param action GLFW action (press, release, etc.)
     * @param mods Modifier keys
     * @param xpos Mouse X position
     * @param ypos Mouse Y position
     */
    void processMouseInput(int button, int action, int mods, double xpos, double ypos);

private:
    EditorMode m_currentMode;
    int m_selectedObjectId;
    bool m_isDragging;
    bool m_isPlaying;
    
    std::vector<SceneHistoryState> m_undoStack;
    std::vector<SceneHistoryState> m_redoStack;
    
    std::shared_ptr<Scene> m_scene;
    std::shared_ptr<Camera> m_camera;
    
    // Helper functions
    std::string serializeScene() const;
    void deserializeScene(const std::string& data);
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_SCENE_EDITOR_H
