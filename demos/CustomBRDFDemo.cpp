#include <ElementalRenderer.h>
#include <iostream>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @file CustomBRDFDemo.cpp
 * @brief Demonstrates creating and using custom BRDFs with ElementalRenderer
 */

int main() {
    // Initialize the renderer
    ElementalRenderer::RendererOptions options;
    options.windowWidth = 1280;
    options.windowHeight = 720;
    options.windowTitle = "Custom BRDF Demo";
    
    if (!ElementalRenderer::Renderer::initialize(options)) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return -1;
    }
    
    // Create a scene
    auto scene = std::make_shared<ElementalRenderer::Scene>("Custom BRDF Scene");
    
    // Add a ground plane
    auto groundMesh = ElementalRenderer::Mesh::createPlane(20.0f, 20.0f);
    auto groundMaterial = std::make_shared<ElementalRenderer::Material>();
    groundMaterial->setDiffuseColor(glm::vec3(0.5f, 0.5f, 0.5f));
    groundMaterial->setSpecularColor(glm::vec3(0.2f, 0.2f, 0.2f));
    groundMaterial->setRoughness(0.8f);
    groundMesh->setMaterial(groundMaterial);
    groundMesh->setPosition(glm::vec3(0.0f, -2.0f, 0.0f));
    scene->addMesh(groundMesh, "Ground");
    
    // Create spheres with different materials for comparison
    const float spacing = 2.5f;
    
    // Standard Phong model
    auto phongSphereMesh = ElementalRenderer::Mesh::createSphere(1.0f, 32, 32);
    auto phongMaterial = std::make_shared<ElementalRenderer::Material>();
    phongMaterial->setDiffuseColor(glm::vec3(0.8f, 0.8f, 0.8f));
    phongMaterial->setSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
    phongMaterial->setRoughness(0.3f);
    phongSphereMesh->setMaterial(phongMaterial);
    phongSphereMesh->setPosition(glm::vec3(-spacing * 2, 0.0f, 0.0f));
    scene->addMesh(phongSphereMesh, "PhongSphere");
    
    // Oren-Nayar model
    auto orenNayarSphereMesh = ElementalRenderer::Mesh::createSphere(1.0f, 32, 32);
    auto orenNayarMaterial = std::make_shared<ElementalRenderer::Material>();
    orenNayarMaterial->setDiffuseColor(glm::vec3(0.8f, 0.8f, 0.8f));
    orenNayarMaterial->setSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
    orenNayarMaterial->setRoughness(0.3f);
    orenNayarSphereMesh->setMaterial(orenNayarMaterial);
    orenNayarSphereMesh->setPosition(glm::vec3(-spacing, 0.0f, 0.0f));
    scene->addMesh(orenNayarSphereMesh, "OrenNayarSphere");
    
    // Cook-Torrance model
    auto cookTorranceSphereMesh = ElementalRenderer::Mesh::createSphere(1.0f, 32, 32);
    auto cookTorranceMaterial = std::make_shared<ElementalRenderer::Material>();
    cookTorranceMaterial->setDiffuseColor(glm::vec3(0.8f, 0.8f, 0.8f));
    cookTorranceMaterial->setSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
    cookTorranceMaterial->setRoughness(0.3f);
    cookTorranceMaterial->setMetallic(0.5f);
    cookTorranceSphereMesh->setMaterial(cookTorranceMaterial);
    cookTorranceSphereMesh->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    scene->addMesh(cookTorranceSphereMesh, "CookTorranceSphere");
    
    // Custom BRDF 1: Anisotropic GGX
    auto anisotropicSphereMesh = ElementalRenderer::Mesh::createSphere(1.0f, 32, 32);
    auto anisotropicMaterial = std::make_shared<ElementalRenderer::Material>();
    anisotropicMaterial->setDiffuseColor(glm::vec3(0.8f, 0.8f, 0.8f));
    anisotropicMaterial->setSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
    anisotropicMaterial->setRoughness(0.3f);
    anisotropicMaterial->setMetallic(0.5f);
    anisotropicMaterial->setParameter("anisotropy", 0.8f);
    anisotropicSphereMesh->setMaterial(anisotropicMaterial);
    anisotropicSphereMesh->setPosition(glm::vec3(spacing, 0.0f, 0.0f));
    scene->addMesh(anisotropicSphereMesh, "AnisotropicSphere");
    
    // Custom BRDF 2: Cloth model
    auto clothSphereMesh = ElementalRenderer::Mesh::createSphere(1.0f, 32, 32);
    auto clothMaterial = std::make_shared<ElementalRenderer::Material>();
    clothMaterial->setDiffuseColor(glm::vec3(0.8f, 0.8f, 0.8f));
    clothMaterial->setSpecularColor(glm::vec3(0.3f, 0.3f, 0.3f));
    clothMaterial->setRoughness(0.7f);
    clothMaterial->setParameter("fuzz", 0.5f);
    clothMaterial->setParameter("threadDirection", glm::vec3(1.0f, 0.0f, 0.0f));
    clothSphereMesh->setMaterial(clothMaterial);
    clothSphereMesh->setPosition(glm::vec3(spacing * 2, 0.0f, 0.0f));
    scene->addMesh(clothSphereMesh, "ClothSphere");
    
    // Add lights
    auto directionalLight = std::make_shared<ElementalRenderer::Light>(
        ElementalRenderer::Light::Type::DIRECTIONAL,
        glm::vec3(1.0f, 1.0f, 1.0f),  // Color
        1.0f                          // Intensity
    );
    directionalLight->setDirection(glm::vec3(-0.5f, -1.0f, -0.3f));
    scene->addLight(directionalLight, "DirectionalLight");
    
    // Create a camera
    ElementalRenderer::Camera camera;
    camera.setPosition(glm::vec3(0.0f, 3.0f, 8.0f));
    camera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    camera.setPerspective(45.0f, 16.0f/9.0f, 0.1f, 100.0f);
    
    // Get the lighting model manager
    auto& lightingManager = ElementalRenderer::LightingModelManager::getInstance();
    
    // Create custom BRDF models
    
    // 1. Anisotropic GGX BRDF
    auto anisotropicGGX = std::make_shared<ElementalRenderer::CustomBRDFModel>("AnisotropicGGX");
    anisotropicGGX->setShaderCode(R"(
        // Anisotropic GGX BRDF implementation
        float3 calculateBRDF(float3 lightDir, float3 viewDir, float3 normal, MaterialProperties material) {
            float3 halfVector = normalize(lightDir + viewDir);
            
            // Extract material properties
            float roughness = material.roughness;
            float metallic = material.metallic;
            float anisotropy = material.customParams[0]; // Anisotropy parameter
            
            // Create anisotropic roughness
            float roughnessX = max(0.001, roughness * (1.0 + anisotropy));
            float roughnessY = max(0.001, roughness * (1.0 - anisotropy));
            
            // Create tangent and bitangent
            float3 tangent = normalize(cross(normal, float3(0.0, 1.0, 0.0)));
            float3 bitangent = normalize(cross(normal, tangent));
            
            // Compute anisotropic NDF (Normal Distribution Function)
            float NdotH = max(dot(normal, halfVector), 0.0);
            float TdotH = dot(tangent, halfVector);
            float BdotH = dot(bitangent, halfVector);
            float a2 = roughnessX * roughnessY;
            float NDF = a2 / (3.14159265359 * roughnessX * roughnessY * pow(pow(TdotH/roughnessX, 2.0) + pow(BdotH/roughnessY, 2.0) + NdotH*NdotH, 2.0));
            
            // Compute Fresnel term (Schlick approximation)
            float3 F0 = lerp(float3(0.04, 0.04, 0.04), material.diffuseColor, metallic);
            float HdotV = max(dot(halfVector, viewDir), 0.0);
            float3 F = F0 + (1.0 - F0) * pow(1.0 - HdotV, 5.0);
            
            // Compute Geometry term (Smith GGX)
            float NdotV = max(dot(normal, viewDir), 0.001);
            float NdotL = max(dot(normal, lightDir), 0.001);
            float k = roughness * roughness / 2.0;
            float G1V = NdotV / (NdotV * (1.0 - k) + k);
            float G1L = NdotL / (NdotL * (1.0 - k) + k);
            float G = G1V * G1L;
            
            // Compute specular BRDF
            float3 specular = (NDF * G * F) / (4.0 * NdotV * NdotL);
            
            // Compute diffuse BRDF (Lambert)
            float3 diffuse = material.diffuseColor / 3.14159265359;
            
            // Combine diffuse and specular, accounting for energy conservation
            float3 kD = (1.0 - F) * (1.0 - metallic);
            return kD * diffuse + specular;
        }
    )");
    
    // 2. Cloth BRDF
    auto clothBRDF = std::make_shared<ElementalRenderer::CustomBRDFModel>("ClothBRDF");
    clothBRDF->setShaderCode(R"(
        // Cloth BRDF implementation
        float3 calculateBRDF(float3 lightDir, float3 viewDir, float3 normal, MaterialProperties material) {
            // Extract material properties
            float roughness = material.roughness;
            float fuzz = material.customParams[0]; // Fuzz parameter
            float3 threadDir = float3(material.customParams[1], material.customParams[2], material.customParams[3]); // Thread direction
            
            // Normalize thread direction
            threadDir = normalize(threadDir);
            
            // Create tangent and bitangent
            float3 tangent = normalize(cross(normal, float3(0.0, 1.0, 0.0)));
            float3 bitangent = normalize(cross(normal, tangent));
            
            // Align thread direction with tangent space
            float3 alignedThreadDir = normalize(threadDir.x * tangent + threadDir.y * bitangent + threadDir.z * normal);
            
            // Compute thread alignment factor
            float threadAlignment = abs(dot(alignedThreadDir, lightDir));
            
            // Compute Oren-Nayar diffuse term for rough cloth
            float NdotL = max(dot(normal, lightDir), 0.0);
            float NdotV = max(dot(normal, viewDir), 0.0);
            
            float gamma = dot(viewDir - normal * NdotV, lightDir - normal * NdotL);
            float roughness2 = roughness * roughness;
            
            float A = 1.0 - 0.5 * (roughness2 / (roughness2 + 0.33));
            float B = 0.45 * (roughness2 / (roughness2 + 0.09));
            
            float alpha = max(acos(NdotV), acos(NdotL));
            float beta = min(acos(NdotV), acos(NdotL));
            
            float diffuseTerm = NdotL * (A + B * max(0.0, gamma) * sin(alpha) * tan(beta));
            
            // Add fuzz term for microfiber scattering
            float fuzzTerm = fuzz * pow(1.0 - abs(dot(normal, viewDir)), 5.0) * threadAlignment;
            
            // Combine terms
            float3 diffuse = material.diffuseColor * (diffuseTerm + fuzzTerm) / 3.14159265359;
            float3 specular = material.specularColor * pow(max(dot(reflect(-lightDir, normal), viewDir), 0.0), 8.0) * 0.1;
            
            return diffuse + specular;
        }
    )");
    
    // Register custom BRDFs with the lighting manager
    lightingManager.registerLightingModel(anisotropicGGX);
    lightingManager.registerLightingModel(clothBRDF);
    
    // Set initial lighting models for each sphere
    phongMaterial->setLightingModel("Phong");
    orenNayarMaterial->setLightingModel("OrenNayar");
    cookTorranceMaterial->setLightingModel("CookTorrance");
    anisotropicMaterial->setLightingModel("AnisotropicGGX");
    clothMaterial->setLightingModel("ClothBRDF");
    
    // Animation variables
    float time = 0.0f;
    
    // Main loop
    while (!ElementalRenderer::Renderer::shouldClose()) {
        // Update time
        time += 0.01f;
        
        // Rotate camera around the scene
        float cameraX = 8.0f * sin(time * 0.1f);
        float cameraZ = 8.0f * cos(time * 0.1f);
        camera.setPosition(glm::vec3(cameraX, 3.0f, cameraZ));
        camera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
        
        // Render the scene
        ElementalRenderer::Renderer::clear();
        ElementalRenderer::Renderer::renderScene(*scene, camera);
        
        // Draw labels for each sphere
        ElementalRenderer::Renderer::drawText("Phong", -spacing * 2, -1.5f, 0.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        ElementalRenderer::Renderer::drawText("Oren-Nayar", -spacing, -1.5f, 0.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        ElementalRenderer::Renderer::drawText("Cook-Torrance", 0.0f, -1.5f, 0.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        ElementalRenderer::Renderer::drawText("Anisotropic GGX", spacing, -1.5f, 0.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        ElementalRenderer::Renderer::drawText("Cloth", spacing * 2, -1.5f, 0.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        
        // Swap buffers and poll events
        ElementalRenderer::Renderer::swapBuffers();
        ElementalRenderer::Renderer::pollEvents();
    }
    
    // Clean up
    ElementalRenderer::Renderer::shutdown();
    
    return 0;
}