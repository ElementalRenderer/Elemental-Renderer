# Getting Started with ElementalRenderer

This guide will walk you through the basics of using ElementalRenderer to create and render 3D scenes in your C++ applications.

## Prerequisites

Before you begin, make sure you have:
- C++17 compatible compiler
- CMake 3.14 or later
- GLM library

For detailed installation instructions, see [INSTALL_PREREQUISITES.md](../INSTALL_PREREQUISITES.md).

## Setting Up Your Project

### 1. Include ElementalRenderer in your CMake project

```cmake
# Find ElementalRenderer
find_package(ElementalRenderer REQUIRED)

# Link your application with ElementalRenderer
target_link_libraries(your_application PRIVATE ElementalRenderer)
```

### 2. Include the main header in your code

```cpp
#include <ElementalRenderer.h>
```

## Creating Your First Scene

Here's a simple example that creates a basic scene with a cube and renders it:

```cpp
#include <ElementalRenderer.h>
#include <iostream>

int main() {
    // Initialize the renderer
    ElementalRenderer::RendererOptions options;
    options.windowWidth = 1280;
    options.windowHeight = 720;
    options.windowTitle = "My First ElementalRenderer App";
    
    if (!ElementalRenderer::Renderer::initialize(options)) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return -1;
    }
    
    // Create a scene
    auto scene = std::make_shared<ElementalRenderer::Scene>("My Scene");
    
    // Add a cube mesh
    auto cubeMesh = ElementalRenderer::Mesh::createCube(1.0f);
    scene->addMesh(cubeMesh, "Cube");
    
    // Add a light
    auto light = std::make_shared<ElementalRenderer::Light>(
        ElementalRenderer::Light::Type::DIRECTIONAL,
        glm::vec3(1.0f, 1.0f, 1.0f),  // Color
        1.0f                          // Intensity
    );
    light->setDirection(glm::vec3(-1.0f, -1.0f, -1.0f));
    scene->addLight(light, "MainLight");
    
    // Create a camera
    ElementalRenderer::Camera camera;
    camera.setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    camera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    camera.setPerspective(45.0f, 16.0f/9.0f, 0.1f, 100.0f);
    
    // Main loop
    while (!ElementalRenderer::Renderer::shouldClose()) {
        // Update camera or scene if needed
        
        // Render the scene
        ElementalRenderer::Renderer::renderScene(*scene, camera);
        
        // Swap buffers and poll events
        ElementalRenderer::Renderer::swapBuffers();
        ElementalRenderer::Renderer::pollEvents();
    }
    
    // Clean up
    ElementalRenderer::Renderer::shutdown();
    
    return 0;
}
```

## Working with Materials

ElementalRenderer provides a flexible material system:

```cpp
// Create a basic material
auto material = std::make_shared<ElementalRenderer::Material>();
material->setDiffuseColor(glm::vec3(0.8f, 0.2f, 0.2f));  // Red
material->setSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
material->setRoughness(0.3f);
material->setMetallic(0.0f);

// Apply the material to a mesh
cubeMesh->setMaterial(material);

// Load a texture
auto texture = ElementalRenderer::Texture::loadFromFile("path/to/texture.png");
material->setDiffuseTexture(texture);
```

## Using Different Lighting Models

ElementalRenderer supports various lighting models that can be easily swapped:

```cpp
// Get the lighting model manager
auto& lightingManager = ElementalRenderer::LightingModelManager::getInstance();

// Use Oren-Nayar lighting model for rough diffuse surfaces
lightingManager.setActiveLightingModel("OrenNayar");

// Use Cook-Torrance for physically-based specular reflection
lightingManager.setActiveLightingModel("CookTorrance");

// Create a custom BRDF model
auto customBRDF = std::make_shared<ElementalRenderer::CustomBRDFModel>("MyCustomBRDF");
customBRDF->setShaderCode(R"(
    float3 calculateBRDF(float3 lightDir, float3 viewDir, float3 normal, MaterialProperties material) {
        // Your custom BRDF implementation here
        // ...
        return result;
    }
)");
lightingManager.registerLightingModel(customBRDF);
lightingManager.setActiveLightingModel("MyCustomBRDF");
```

## Using the Node-based Shader Editor

ElementalRenderer includes a powerful node-based shader editor:

```cpp
// Get the shader editor manager
auto& editorManager = ElementalRenderer::ShaderEditorManager::getInstance();

// Initialize the editor
editorManager.initialize(1280, 720);

// Show the editor
editorManager.showEditor();

// Create a shader from the current graph
auto shader = editorManager.createShaderFromGraph();

// Apply the shader to a material
material->setShader(shader);
```

## Using the Render Graph

The heuristic render graph system automatically manages render pass dependencies:

```cpp
// Create a render graph
auto renderGraph = std::make_shared<ElementalRenderer::RenderGraph>("MyRenderGraph");

// Add render passes with their resource dependencies
auto shadowPass = std::make_shared<ElementalRenderer::RenderPass>("ShadowPass", []() {
    // Shadow mapping code here
});
shadowPass->addWriteResource("ShadowMap");
renderGraph->addPass(shadowPass);

auto geometryPass = std::make_shared<ElementalRenderer::RenderPass>("GeometryPass", []() {
    // Geometry rendering code here
});
geometryPass->addReadResource("ShadowMap");
geometryPass->addWriteResource("GBuffer");
renderGraph->addPass(geometryPass);

// Build and execute the render graph
renderGraph->buildDependencyGraph();
renderGraph->execute();
```

## Next Steps

Now that you've learned the basics, check out these resources for more advanced usage:
- [Demo Scenes](./DemoScenes.md) - Examples of shadow mapping, PBR, and post-processing
- [Custom BRDFs](./CustomBRDFs.md) - How to create custom lighting models
- [Node Shaders](./NodeShaders.md) - Advanced usage of the node-based shader system
- [API Reference](./APIReference.md) - Complete documentation of the ElementalRenderer API