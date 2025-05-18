#include "../../include/GUI/ModelEditor.h"
#include <iostream>
#include <fstream>
#include <sstream>

ModelEditor::ModelEditor() : modified(false) {
    // Initialize an empty mesh
    currentMesh.name = "NewModel";
}

ModelEditor::~ModelEditor() {
    // cleanup resources if needed, later ;3
}

bool ModelEditor::initialize() {
    return true;
}

void ModelEditor::renderEditorUI() {
}

void ModelEditor::handleInput() {
}

void ModelEditor::createNewModel(const std::string& name) {
    currentMesh = EditableMesh();
    currentMesh.name = name;
    modified = true;
}

void ModelEditor::addVertex(float x, float y, float z) {

    currentMesh.vertices.push_back(x);
    currentMesh.vertices.push_back(y);
    currentMesh.vertices.push_back(z);

    currentMesh.texCoords.push_back(0.0f);
    currentMesh.texCoords.push_back(0.0f);

    currentMesh.normals.push_back(0.0f);
    currentMesh.normals.push_back(1.0f);
    currentMesh.normals.push_back(0.0f);
    
    modified = true;
}

void ModelEditor::addFace(const std::vector<int>& vertexIndices) {

    if (vertexIndices.size() >= 3) {

        for (size_t i = 0; i < vertexIndices.size() - 2; ++i) {
            currentMesh.indices.push_back(vertexIndices[0]);
            currentMesh.indices.push_back(vertexIndices[i + 1]);
            currentMesh.indices.push_back(vertexIndices[i + 2]);

            currentMesh.materialIndices.push_back(0);
        }

        calculateNormals();
        modified = true;
    }
}

void ModelEditor::removeVertex(int index) {
    if (index >= 0 && index < currentMesh.vertices.size() / 3) {

        currentMesh.vertices.erase(currentMesh.vertices.begin() + index * 3, 
                                  currentMesh.vertices.begin() + (index + 1) * 3);
        currentMesh.normals.erase(currentMesh.normals.begin() + index * 3,
                                 currentMesh.normals.begin() + (index + 1) * 3);
        currentMesh.texCoords.erase(currentMesh.texCoords.begin() + index * 2,
                                   currentMesh.texCoords.begin() + (index + 1) * 2);

        for (auto& idx : currentMesh.indices) {
            if (idx > index) {
                idx--;
            }
        }

        for (size_t i = 0; i < currentMesh.indices.size(); i += 3) {
            if (currentMesh.indices[i] == index || 
                currentMesh.indices[i+1] == index || 
                currentMesh.indices[i+2] == index) {
                
                currentMesh.indices.erase(currentMesh.indices.begin() + i, 
                                        currentMesh.indices.begin() + i + 3);
                currentMesh.materialIndices.erase(currentMesh.materialIndices.begin() + i/3);
                i -= 3;
            }
        }
        
        modified = true;
    }
}

void ModelEditor::removeFace(int index) {

    if (index >= 0 && index < currentMesh.indices.size() / 3) {
        currentMesh.indices.erase(currentMesh.indices.begin() + index * 3,
                                currentMesh.indices.begin() + (index + 1) * 3);
        currentMesh.materialIndices.erase(currentMesh.materialIndices.begin() + index);
        modified = true;
    }
}

void ModelEditor::moveVertex(int index, float x, float y, float z) {

    if (index >= 0 && index < currentMesh.vertices.size() / 3) {
        currentMesh.vertices[index * 3] = x;
        currentMesh.vertices[index * 3 + 1] = y;
        currentMesh.vertices[index * 3 + 2] = z;

        calculateNormals();
        modified = true;
    }
}

void ModelEditor::assignMaterial(int faceIndex, const std::shared_ptr<Material>& material) {
    auto it = std::find(currentMesh.materials.begin(), currentMesh.materials.end(), material);
    int materialIndex;
    
    if (it == currentMesh.materials.end()) {
        currentMesh.materials.push_back(material);
        materialIndex = currentMesh.materials.size() - 1;
    } else {
        materialIndex = std::distance(currentMesh.materials.begin(), it);
    }

    if (faceIndex >= 0 && faceIndex < currentMesh.materialIndices.size()) {
        currentMesh.materialIndices[faceIndex] = materialIndex;
        modified = true;
    }
}

void ModelEditor::createTexture(const std::string& name, int width, int height) {

}

void ModelEditor::applyTexture(int faceIndex, const std::shared_ptr<Texture>& texture) {

}

bool ModelEditor::exportModel(const std::string& filename) {

    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        file << "# Exported from ModelEditor\n";
        file << "# Model name: " << currentMesh.name << "\n\n";

        for (size_t i = 0; i < currentMesh.vertices.size(); i += 3) {
            file << "v " << currentMesh.vertices[i] << " " 
                         << currentMesh.vertices[i+1] << " " 
                         << currentMesh.vertices[i+2] << "\n";
        }

        for (size_t i = 0; i < currentMesh.texCoords.size(); i += 2) {
            file << "vt " << currentMesh.texCoords[i] << " " 
                          << currentMesh.texCoords[i+1] << "\n";
        }

        for (size_t i = 0; i < currentMesh.normals.size(); i += 3) {
            file << "vn " << currentMesh.normals[i] << " " 
                          << currentMesh.normals[i+1] << " " 
                          << currentMesh.normals[i+2] << "\n";
        }

        for (size_t i = 0; i < currentMesh.indices.size(); i += 3) {
            file << "f " << (currentMesh.indices[i]+1) << "/" << (currentMesh.indices[i]+1) << "/" << (currentMesh.indices[i]+1) << " "
                         << (currentMesh.indices[i+1]+1) << "/" << (currentMesh.indices[i+1]+1) << "/" << (currentMesh.indices[i+1]+1) << " "
                         << (currentMesh.indices[i+2]+1) << "/" << (currentMesh.indices[i+2]+1) << "/" << (currentMesh.indices[i+2]+1) << "\n";
        }
        
        file.close();
        modified = false;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error exporting model: " << e.what() << std::endl;
        return false;
    }
}

bool ModelEditor::importModel(const std::string& filename) {
    // Import a model from file
    // This would parse common 3D formats like OBJ or STL
    // Implementation depends on your file format support
    return false; // Placeholder
}

std::shared_ptr<Mesh> ModelEditor::getCreatedMesh() const {
    return nullptr; // Placeholder
}

void ModelEditor::calculateNormals() {

    std::fill(currentMesh.normals.begin(), currentMesh.normals.end(), 0.0f);

    for (size_t i = 0; i < currentMesh.indices.size(); i += 3) {
        unsigned int i0 = currentMesh.indices[i];
        unsigned int i1 = currentMesh.indices[i+1];
        unsigned int i2 = currentMesh.indices[i+2];

        float v0x = currentMesh.vertices[i0*3];
        float v0y = currentMesh.vertices[i0*3+1];
        float v0z = currentMesh.vertices[i0*3+2];
        
        float v1x = currentMesh.vertices[i1*3];
        float v1y = currentMesh.vertices[i1*3+1];
        float v1z = currentMesh.vertices[i1*3+2];
        
        float v2x = currentMesh.vertices[i2*3];
        float v2y = currentMesh.vertices[i2*3+1];
        float v2z = currentMesh.vertices[i2*3+2];

        float e1x = v1x - v0x;
        float e1y = v1y - v0y;
        float e1z = v1z - v0z;
        
        float e2x = v2x - v0x;
        float e2y = v2y - v0y;
        float e2z = v2z - v0z;

        float nx = e1y * e2z - e1z * e2y;
        float ny = e1z * e2x - e1x * e2z;
        float nz = e1x * e2y - e1y * e2x;

        float length = sqrt(nx*nx + ny*ny + nz*nz);
        if (length > 0.0001f) {
            nx /= length;
            ny /= length;
            nz /= length;
        }

        currentMesh.normals[i0*3] += nx;
        currentMesh.normals[i0*3+1] += ny;
        currentMesh.normals[i0*3+2] += nz;
        
        currentMesh.normals[i1*3] += nx;
        currentMesh.normals[i1*3+1] += ny;
        currentMesh.normals[i1*3+2] += nz;
        
        currentMesh.normals[i2*3] += nx;
        currentMesh.normals[i2*3+1] += ny;
        currentMesh.normals[i2*3+2] += nz;
    }

    for (size_t i = 0; i < currentMesh.normals.size(); i += 3) {
        float nx = currentMesh.normals[i];
        float ny = currentMesh.normals[i+1];
        float nz = currentMesh.normals[i+2];
        
        float length = sqrt(nx*nx + ny*ny + nz*nz);
        if (length > 0.0001f) {
            currentMesh.normals[i] = nx / length;
            currentMesh.normals[i+1] = ny / length;
            currentMesh.normals[i+2] = nz / length;
        } else {
            currentMesh.normals[i] = 0.0f;
            currentMesh.normals[i+1] = 1.0f;
            currentMesh.normals[i+2] = 0.0f;
        }
    }
}

void ModelEditor::updateMesh() {
    // Update the engine mesh representation
    // This would synchronize the editable mesh with the rendering mesh
}
