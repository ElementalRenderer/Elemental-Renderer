#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>

#include "../Mesh.h"
#include "../Material.h"
#include "../Texture.h"

/**
 * ModelEditor class provides functionality for users to create and edit 3D models
 * It includes tools for vertex manipulation, face creation, and texture mapping
 */
class ModelEditor {
public:
    ModelEditor();
    ~ModelEditor();

    // Initialize the editor
    bool initialize();
    
    // UI rendering and event handling
    void renderEditorUI();
    void handleInput();
    
    // Model creation and manipulation
    void createNewModel(const std::string& name);
    void addVertex(float x, float y, float z);
    void addFace(const std::vector<int>& vertexIndices);
    void removeVertex(int index);
    void removeFace(int index);
    void moveVertex(int index, float x, float y, float z);
    
    // Texture and material handling
    void assignMaterial(int faceIndex, const std::shared_ptr<Material>& material);
    void createTexture(const std::string& name, int width, int height);
    void applyTexture(int faceIndex, const std::shared_ptr<Texture>& texture);
    
    // Import/Export functionality
    bool exportModel(const std::string& filename);
    bool importModel(const std::string& filename);
    
    // Get the created mesh
    std::shared_ptr<Mesh> getCreatedMesh() const;
    
private:
    struct EditableMesh {
        std::string name;
        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<float> texCoords;
        std::vector<unsigned int> indices;
        std::vector<std::shared_ptr<Material>> materials;
        std::vector<int> materialIndices; // Material index for each face
    };

    EditableMesh currentMesh;
    bool modified;
    
    // Helper methods
    void calculateNormals();
    void updateMesh();
};
