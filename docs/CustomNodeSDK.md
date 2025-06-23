# Custom Node SDK for ElementalRenderer

This guide explains how to create custom shader nodes for ElementalRenderer using the Custom Node SDK. The SDK provides two ways to create custom nodes:

1. **JSON-based nodes**: Simple nodes can be defined using JSON files, which are hot-reloadable.
2. **C++ nodes**: More complex nodes can be implemented in C++ for advanced functionality.

## Table of Contents

- [Overview](#overview)
- [JSON-based Nodes](#json-based-nodes)
  - [JSON Node Structure](#json-node-structure)
  - [Code Template Syntax](#code-template-syntax)
  - [Example JSON Node](#example-json-node)
- [C++ Nodes](#c-nodes)
  - [Using the DEFINE_CUSTOM_NODE Macro](#using-the-define_custom_node-macro)
  - [Implementing the generateCode Method](#implementing-the-generatecode-method)
  - [Example C++ Node](#example-c-node)
- [Node Registration](#node-registration)
- [Hot Reloading](#hot-reloading)
- [Best Practices](#best-practices)

## Overview

The Custom Node SDK allows you to extend ElementalRenderer's shader graph system with your own custom nodes. These nodes can be used in the visual shader editor just like built-in nodes.

Key components of the SDK:

- **CustomNodeManager**: Singleton class that manages node definitions and provides hot-reloading.
- **ICustomNodeDefinition**: Interface for custom node definitions.
- **JsonNodeDefinition**: Implementation for JSON-based nodes.
- **CustomNodeDefinition**: Base class for C++ custom node definitions.
- **CustomShaderNode**: Implementation of ShaderNode that uses a custom node definition.

## JSON-based Nodes

JSON-based nodes are the simplest way to create custom nodes. They are defined in JSON files and can be hot-reloaded during development.

### JSON Node Structure

A JSON node definition has the following structure:

```json
{
    "name": "NodeName",
    "category": "NodeCategory",
    "description": "Node description",
    "inputs": [
        {
            "name": "InputName",
            "type": "InputType",
            "defaultValue": "DefaultValue"
        }
    ],
    "outputs": [
        {
            "name": "OutputName",
            "type": "OutputType"
        }
    ],
    "code": "HLSL code template"
}
```

You can add more inputs and outputs as needed.

- **name**: The name of the node as it appears in the editor.
- **category**: The category of the node in the node palette.
- **description**: A description of what the node does.
- **inputs**: An array of input pins.
  - **name**: The name of the input pin.
  - **type**: The data type of the input pin (float, vec2, vec3, vec4, int, bool, sampler2D, matrix).
  - **defaultValue**: The default value used when the input is not connected.
- **outputs**: An array of output pins.
  - **name**: The name of the output pin.
  - **type**: The data type of the output pin.
- **code**: HLSL code template that implements the node's functionality.

### Code Template Syntax

The code template uses placeholders that are replaced with actual variable names during code generation:

- `{{InputName}}`: Replaced with the variable name of the input.
- `{{OutputName_out}}`: Replaced with the variable name of the output.

### Example JSON Node

Here's an example of a JSON node that calculates the Fresnel effect:

```json
{
    "name": "Fresnel",
    "category": "Lighting",
    "description": "Calculates Fresnel effect based on view direction and normal",
    "inputs": [
        {
            "name": "Normal",
            "type": "vec3",
            "defaultValue": "float3(0, 0, 1)"
        },
        {
            "name": "ViewDirection",
            "type": "vec3",
            "defaultValue": "float3(0, 0, 1)"
        },
        {
            "name": "Power",
            "type": "float",
            "defaultValue": "5.0"
        },
        {
            "name": "BaseReflectivity",
            "type": "float",
            "defaultValue": "0.04"
        }
    ],
    "outputs": [
        {
            "name": "Fresnel",
            "type": "float"
        }
    ],
    "code": "// Calculate Fresnel effect (Schlick approximation)\nfloat3 normalizedNormal = normalize({{Normal}});\nfloat3 normalizedViewDir = normalize({{ViewDirection}});\nfloat nDotV = max(dot(normalizedNormal, normalizedViewDir), 0.0);\nfloat {{Fresnel_out}} = {{BaseReflectivity}} + (1.0 - {{BaseReflectivity}}) * pow(1.0 - nDotV, {{Power}});\n"
}
```

## C++ Nodes

For more complex nodes, you can create C++ implementations that have full access to the shader graph system.

### Using the DEFINE_CUSTOM_NODE Macro

The SDK provides a macro to simplify the creation of custom nodes:

```cpp
DEFINE_CUSTOM_NODE(NodeClass, "NodeName", "NodeCategory", "NodeDescription")
{
    // Setup the node's inputs and outputs
    addInputPin("InputName", NodePin::Type::TYPE, "DefaultValue");
    addOutputPin("OutputName", NodePin::Type::TYPE);
}
```

### Implementing the generateCode Method

You need to implement the `generateCode` method to generate HLSL code for your node:

```cpp
bool NodeClass::generateCode(const ElementalRenderer::ShaderGraph* graph, std::string& code,
                           std::unordered_map<int, std::string>& outputVariables,
                           const std::unordered_map<int, std::string>& inputVariables) const
{
    // Get input variable names
    std::string inputVar = inputVariables.at(0);

    // Generate output variable names
    std::string outputVar = "output_" + std::to_string(getId());

    // Store output variable names for connections
    outputVariables[getId() * 1000 + 0] = outputVar;

    // Generate HLSL code
    code += "float " + outputVar + " = " + inputVar + " * 2.0;\n";

    return true;
}
```

### Example C++ Node

Here's an example of a C++ node that generates 3D Perlin noise:

```cpp
// Define a custom noise node using the DEFINE_CUSTOM_NODE macro
DEFINE_CUSTOM_NODE(NoiseNode, "Perlin Noise", "Procedural", "Generates 3D Perlin noise")
{
    // Setup the node's inputs and outputs
    addInputPin("Position", NodePin::Type::VEC3, "float3(0, 0, 0)");
    addInputPin("Scale", NodePin::Type::FLOAT, "1.0");
    addOutputPin("Noise", NodePin::Type::FLOAT);
    addOutputPin("Gradient", NodePin::Type::VEC3);
}

// Implement the code generation function
bool NoiseNode::generateCode(const ElementalRenderer::ShaderGraph* graph, std::string& code,
                           std::unordered_map<int, std::string>& outputVariables,
                           const std::unordered_map<int, std::string>& inputVariables) const
{
    // Get input variable names
    std::string positionVar = inputVariables.at(0);
    std::string scaleVar = inputVariables.at(1);

    // Generate output variable names
    std::string noiseVar = "noise_" + std::to_string(getId());
    std::string gradientVar = "noiseGradient_" + std::to_string(getId());

    // Store output variable names for connections
    outputVariables[getId() * 1000 + 0] = noiseVar;
    outputVariables[getId() * 1000 + 1] = gradientVar;

    // Generate the HLSL code for Perlin noise
    code += "// Perlin noise implementation\n";
    code += "float " + noiseVar + ";\n";
    code += "float3 " + gradientVar + ";\n";

    // ... (Perlin noise implementation) ...

    // Call the noise function with our inputs
    code += "{\n";
    code += "    float3 gradientTemp;\n";
    code += "    " + noiseVar + " = perlinNoise(" + positionVar + " * " + scaleVar + ", gradientTemp);\n";
    code += "    " + noiseVar + " = " + noiseVar + " * 0.5 + 0.5;\n";  // Remap from [-1,1] to [0,1]
    code += "    " + gradientVar + " = gradientTemp;\n";
    code += "}\n";

    return true;
}
```

## Node Registration

To make your custom nodes available in the shader editor, you need to register them with the CustomNodeManager:

```cpp
// For C++ nodes
REGISTER_CUSTOM_NODE(NoiseNode);

// For JSON nodes
auto& manager = CustomNodeManager::getInstance();
manager.loadFromDirectory("path/to/json/nodes", true);  // true for recursive search
```

## Hot Reloading

The SDK supports hot reloading of JSON node definitions, which is useful during development:

```cpp
auto& manager = CustomNodeManager::getInstance();
manager.setHotReloadingEnabled(true);

// In your update loop
manager.checkForChanges();  // Reloads any modified JSON files
```

## Best Practices

1. **Keep nodes focused**: Each node should do one thing well.
2. **Use meaningful names**: Choose clear names for nodes, inputs, and outputs.
3. **Provide good default values**: Default values should produce reasonable results.
4. **Document your nodes**: Use the description field to explain what the node does.
5. **Organize nodes into categories**: Use categories to organize related nodes.
6. **Test your nodes**: Make sure your nodes work correctly in different contexts.
7. **Consider performance**: Complex nodes can impact shader performance.
8. **Use JSON for simple nodes**: JSON nodes are easier to create and modify.
9. **Use C++ for complex nodes**: C++ nodes provide more flexibility for complex operations.
10. **Enable hot reloading during development**: Hot reloading makes it easier to iterate on node designs.
