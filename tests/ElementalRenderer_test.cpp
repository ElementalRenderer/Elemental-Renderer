/**
 * @file ElementalRenderer_test.cpp
 * @brief Unit tests for Elemental Renderer library
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "ElementalRenderer.h"
#include "Camera.h"
#include "Scene.h"
#include "Mesh.h"
#include "Material.h"
#include "Light.h"
#include "Texture.h"
#include "Shader.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

TEST_CASE("Elemental Renderer Version") {
    std::string version = ElementalRenderer::Version::toString();
    CHECK(!version.empty());
    
    // Version should be in format x.y.z
    size_t firstDot = version.find('.');
    size_t lastDot = version.rfind('.');
    
    CHECK(firstDot != std::string::npos);
    CHECK(lastDot != std::string::npos);
    CHECK(firstDot != lastDot);
}

TEST_CASE("Camera Creation and Manipulation") {
    // Create a perspective camera
    ElementalRenderer::Camera camera;
    camera.setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    camera.setTarget(glm::vec3(0.0f));
    camera.setPerspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    
    // Check position
    glm::vec3 position = camera.getPosition();
    CHECK(position.x == doctest::Approx(0.0f));
    CHECK(position.y == doctest::Approx(0.0f));
    CHECK(position.z == doctest::Approx(5.0f));
    
    // Check view matrix determinant is non-zero (valid matrix)
    glm::mat4 viewMatrix = camera.getViewMatrix();
    CHECK(glm::determinant(viewMatrix) != doctest::Approx(0.0f));
    
    // Move camera
    camera.moveForward(2.0f);
    position = camera.getPosition();
    CHECK(position.z == doctest::Approx(3.0f));
}

TEST_CASE("Mesh Creation") {
    // Create a cube mesh
    auto mesh = ElementalRenderer::Mesh::createCube(2.0f);
    
    // We can't check much without rendering, but the mesh should exist
    CHECK(mesh != nullptr);
}

TEST_CASE("Scene Management") {
    // Create a scene
    ElementalRenderer::Scene scene("Test Scene");
    
    // Check name
    CHECK(scene.getName() == "Test Scene");
    
    // Add a mesh
    auto mesh = ElementalRenderer::Mesh::createCube();
    size_t meshIndex = scene.addMesh(mesh, "Cube");
    
    // Check mesh count
    CHECK(scene.getMeshes().size() == 1);
    
    // Retrieve mesh by name
    auto retrievedMesh = scene.getMeshByName("Cube");
    CHECK(retrievedMesh == mesh);
    
    // Add a light
    auto light = ElementalRenderer::Light::createPointLight();
    size_t lightIndex = scene.addLight(light, "PointLight");
    
    // Check light count
    CHECK(scene.getLights().size() == 1);
    
    // Remove mesh
    bool removed = scene.removeMeshByName("Cube");
    CHECK(removed);
    CHECK(scene.getMeshes().empty());
}

TEST_CASE("Material Properties") {
    // Create a material
    ElementalRenderer::Material material;
    
    // Set various properties
    material.setFloat("metallic", 0.5f);
    material.setVec3("albedo", glm::vec3(1.0f, 0.5f, 0.3f));
    material.setBool("useNormalMap", true);
    
    // Create a PBR material with helper
    auto pbrMaterial = ElementalRenderer::Material::createPBRMaterial(
        glm::vec3(0.9f, 0.9f, 0.9f),  // albedo
        0.1f,                          // metallic
        0.5f                           // roughness
    );
    
    CHECK(pbrMaterial != nullptr);
}

TEST_CASE("Light Types") {
    // Create directional light
    auto dirLight = ElementalRenderer::Light::createDirectionalLight(
        glm::vec3(0.0f, -1.0f, 0.0f),  // direction
        glm::vec3(1.0f, 0.9f, 0.8f)     // color
    );
    
    CHECK(dirLight != nullptr);
    CHECK(dirLight->getType() == ElementalRenderer::LightType::DIRECTIONAL);
    
    // Create point light
    auto pointLight = ElementalRenderer::Light::createPointLight(
        glm::vec3(0.0f, 3.0f, 0.0f),  // position
        glm::vec3(0.0f, 0.0f, 1.0f),  // blue color
        2.0f,                         // intensity
        10.0f                         // range
    );
    
    CHECK(pointLight != nullptr);
    CHECK(pointLight->getType() == ElementalRenderer::LightType::POINT);
}
