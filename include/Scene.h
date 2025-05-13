/**
 * @file Scene.h
 * @brief Scene management for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_SCENE_H
#define ELEMENTAL_RENDERER_SCENE_H

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace ElementalRenderer {

class Mesh;
class Light;
class Camera;
/**
 * @file Scene.h
 * @brief Scene management for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_SCENE_H
#define ELEMENTAL_RENDERER_SCENE_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>

namespace ElementalRenderer {

class Mesh;
class Light;

/**
 * @brief Class for managing a scene graph containing meshes, lights, and other objects
 */
class Scene {
public:
    Scene(const std::string& name = "Untitled Scene");
    
    ~Scene();
    
    void setName(const std::string& name);
    
    std::string getName() const;
    
    void addMesh(std::shared_ptr<Mesh> mesh);
    
    void removeMesh(std::shared_ptr<Mesh> mesh);
    
    const std::vector<std::shared_ptr<Mesh>>& getMeshes() const;
    
    void addLight(std::shared_ptr<Light> light);
    
    void removeLight(std::shared_ptr<Light> light);
    
    const std::vector<std::shared_ptr<Light>>& getLights() const;
    
    static std::shared_ptr<Scene> createTestScene(const std::string& name = "Test Scene");

private:
    std::string m_name;
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::vector<std::shared_ptr<Light>> m_lights;
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_SCENE_H
/**
 * @brief Class for managing 3D scenes
 */
class Scene {
public:
    Scene();
    
    explicit Scene(const std::string& name);
    
    size_t addMesh(std::shared_ptr<Mesh> mesh, const std::string& name = "");
    
    std::shared_ptr<Mesh> getMesh(size_t index) const;
    
    std::shared_ptr<Mesh> getMeshByName(const std::string& name) const;
    
    bool removeMesh(size_t index);
    
    bool removeMeshByName(const std::string& name);
    
    const std::vector<std::shared_ptr<Mesh>>& getMeshes() const;
    
    size_t addLight(std::shared_ptr<Light> light, const std::string& name = "");
    
    std::shared_ptr<Light> getLight(size_t index) const;
    
    std::shared_ptr<Light> getLightByName(const std::string& name) const;
    
    bool removeLight(size_t index);
    
    bool removeLightByName(const std::string& name);
    
    const std::vector<std::shared_ptr<Light>>& getLights() const;
    
    void setAmbientLight(const glm::vec3& color);
    
    glm::vec3 getAmbientLight() const;
    
    void setName(const std::string& name);
    
    std::string getName() const;
    
    void clear();
    
    static std::shared_ptr<Scene> createTestScene(const std::string& name = "Test Scene");

private:
    std::string m_name;
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::vector<std::shared_ptr<Light>> m_lights;
    std::unordered_map<std::string, size_t> m_meshNameMap;
    std::unordered_map<std::string, size_t> m_lightNameMap;
    glm::vec3 m_ambientLight;
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_SCENE_H
