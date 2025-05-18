/**
 * @file Scene.cpp
 * @brief Implementation of the Scene class
 */

#include "Scene.h"
#include "Mesh.h"
#include "Light.h"
#include "Material.h"
#include <glm/glm.hpp>
#include <iostream>

namespace ElementalRenderer {

Scene::Scene()
    : m_name("Unnamed Scene")
    , m_ambientLight(0.1f, 0.1f, 0.1f)
{
}

Scene::Scene(const std::string& name)
    : m_name(name)
    , m_ambientLight(0.1f, 0.1f, 0.1f)
{
}

size_t Scene::addMesh(std::shared_ptr<Mesh> mesh, const std::string& name) {
    if (!mesh) {
        std::cerr << "Warning: Attempted to add null mesh to scene" << std::endl;
        return static_cast<size_t>(-1);
    }
    
    size_t index = m_meshes.size();
    m_meshes.push_back(mesh);
    
    if (!name.empty()) {
        m_meshNameMap[name] = index;
    }
    
    return index;
}

std::shared_ptr<Mesh> Scene::getMesh(size_t index) const {
    if (index >= m_meshes.size()) {
        return nullptr;
    }
    
    return m_meshes[index];
}

std::shared_ptr<Mesh> Scene::getMeshByName(const std::string& name) const {
    auto it = m_meshNameMap.find(name);
    if (it == m_meshNameMap.end()) {
        return nullptr;
    }
    
    return m_meshes[it->second];
}

bool Scene::removeMesh(size_t index) {
    if (index >= m_meshes.size()) {
        return false;
    }

    for (auto it = m_meshNameMap.begin(); it != m_meshNameMap.end(); ) {
        if (it->second == index) {
            it = m_meshNameMap.erase(it);
        } else {
            if (it->second > index) {
                it->second--;
            }
            ++it;
        }
    }
    
    m_meshes.erase(m_meshes.begin() + index);
    return true;
}

bool Scene::removeMeshByName(const std::string& name) {
    auto it = m_meshNameMap.find(name);
    if (it == m_meshNameMap.end()) {
        return false;
    }
    
    size_t index = it->second;
    m_meshNameMap.erase(it);
    
    return removeMesh(index);
}

const std::vector<std::shared_ptr<Mesh>>& Scene::getMeshes() const {
    return m_meshes;
}

size_t Scene::addLight(std::shared_ptr<Light> light, const std::string& name) {
    if (!light) {
        std::cerr << "Warning: Attempted to add null light to scene" << std::endl;
        return static_cast<size_t>(-1);
    }
    
    size_t index = m_lights.size();
    m_lights.push_back(light);
    
    if (!name.empty()) {
        m_lightNameMap[name] = index;
    }
    
    return index;
}

std::shared_ptr<Light> Scene::getLight(size_t index) const {
    if (index >= m_lights.size()) {
        return nullptr;
    }
    
    return m_lights[index];
}

std::shared_ptr<Light> Scene::getLightByName(const std::string& name) const {
    auto it = m_lightNameMap.find(name);
    if (it == m_lightNameMap.end()) {
        return nullptr;
    }
    
    return m_lights[it->second];
}

bool Scene::removeLight(size_t index) {
    if (index >= m_lights.size()) {
        return false;
    }

    for (auto it = m_lightNameMap.begin(); it != m_lightNameMap.end(); ) {
        if (it->second == index) {
            it = m_lightNameMap.erase(it);
        } else {
            if (it->second > index) {
                it->second--;
            }
            ++it;
        }
    }
    
    m_lights.erase(m_lights.begin() + index);
    return true;
}

bool Scene::removeLightByName(const std::string& name) {
    auto it = m_lightNameMap.find(name);
    if (it == m_lightNameMap.end()) {
        return false;
    }
    
    size_t index = it->second;
    m_lightNameMap.erase(it);
    
    return removeLight(index);
}

const std::vector<std::shared_ptr<Light>>& Scene::getLights() const {
    return m_lights;
}

void Scene::setAmbientLight(const glm::vec3& color) {
    m_ambientLight = color;
}

glm::vec3 Scene::getAmbientLight() const {
    return m_ambientLight;
}

void Scene::setName(const std::string& name) {
    m_name = name;
}

std::string Scene::getName() const {
    return m_name;
}

void Scene::clear() {
    m_meshes.clear();
    m_lights.clear();
    m_meshNameMap.clear();
    m_lightNameMap.clear();
}

std::shared_ptr<Scene> Scene::createTestScene(const std::string& name) {
    auto scene = std::make_shared<Scene>(name);

    scene->setAmbientLight(glm::vec3(0.05f, 0.05f, 0.1f));

    auto floorMesh = Mesh::createPlane(20.0f, 20.0f);
    auto floorMaterial = Material::createPBRMaterial(
        glm::vec3(0.8f, 0.8f, 0.9f),  // albedo (light gray-blue)
        0.1f,                          // metallic
        0.8f                           // roughness
    );
    floorMesh->setMaterial(floorMaterial);
    scene->addMesh(floorMesh, "Floor");

    for (int i = 0; i < 5; ++i) {
        auto cubeMesh = Mesh::createCube(0.8f);
        auto cubeMaterial = Material::createPBRMaterial(
            glm::vec3(0.9f, 0.1f + i * 0.15f, 0.1f),  // albedo (reddish)
            0.8f,                                      // metallic
            0.2f                                       // roughness
        );
        cubeMesh->setMaterial(cubeMaterial);

        float x = -4.0f + i * 2.0f;
        float y = 0.4f;  // Half height of cube
        float z = 0.0f;

        // cubeMesh->setPosition(glm::vec3(x, y, z));
        
        std::string name = "Cube_" + std::to_string(i);
        scene->addMesh(cubeMesh, name);
    }

    for (int i = 0; i < 3; ++i) {
        auto sphereMesh = Mesh::createSphere(0.6f);
        auto sphereMaterial = Material::createPBRMaterial(
            glm::vec3(0.1f, 0.1f, 0.8f + i * 0.05f),  // albedo (bluish)
            0.1f + i * 0.3f,                          // metallic
            0.5f                                       // roughness
        );
        sphereMesh->setMaterial(sphereMaterial);

        float x = -2.0f + i * 2.0f;
        float y = 0.6f;  // Sphere radius
        float z = -3.0f;

        // sphereMesh->setPosition(glm::vec3(x, y, z));
        
        std::string name = "Sphere_" + std::to_string(i);
        scene->addMesh(sphereMesh, name);
    }

    auto sunLight = Light::createDirectionalLight(
        glm::vec3(-0.5f, -1.0f, -0.3f),  // direction
        glm::vec3(1.0f, 0.95f, 0.8f),     // color (warm sunlight)
        1.2f                              // intensity
    );
    scene->addLight(sunLight, "Sun");

    auto pointLight = Light::createPointLight(
        glm::vec3(3.0f, 2.0f, 0.0f),    // position
        glm::vec3(0.0f, 1.0f, 0.0f),    // color (green)
        2.0f,                           // intensity
        10.0f                           // range
    );
    scene->addLight(pointLight, "GreenPointLight");

    auto spotLight = Light::createSpotLight(
        glm::vec3(-2.0f, 3.0f, 2.0f),   // position
        glm::vec3(0.5f, -1.0f, -0.5f),  // direction
        glm::vec3(1.0f, 0.3f, 0.1f),    // color (orange)
        3.0f,                           // intensity
        8.0f,                           // range
        25.0f,                          // inner angle
        35.0f                           // outer angle
    );
    scene->addLight(spotLight, "OrangeSpotLight");
    
    std::cout << "Created test scene with " << scene->getMeshes().size() 
              << " meshes and " << scene->getLights().size() << " lights." << std::endl;
    
    return scene;
}

} // namespace ElementalRenderer
