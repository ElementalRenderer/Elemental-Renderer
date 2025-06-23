/**
 * @file CustomNoiseNode.cpp
 * @brief Example of creating a custom shader node using C++
 */

#include "Shaders/CustomNodeSDK.h"
#include <cmath>

using namespace ElementalRenderer;

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
    
    // Hash function for Perlin noise
    code += "float3 hash3(float3 p) {\n";
    code += "    p = float3(dot(p, float3(127.1, 311.7, 74.7)),\n";
    code += "               dot(p, float3(269.5, 183.3, 246.1)),\n";
    code += "               dot(p, float3(113.5, 271.9, 124.6)));\n";
    code += "    return -1.0 + 2.0 * frac(sin(p) * 43758.5453123);\n";
    code += "}\n\n";
    
    // Perlin noise function
    code += "float perlinNoise(float3 p, out float3 gradient) {\n";
    code += "    float3 i = floor(p);\n";
    code += "    float3 f = frac(p);\n";
    code += "    float3 u = f * f * (3.0 - 2.0 * f);\n";
    
    code += "    float3 ga = hash3(i + float3(0.0, 0.0, 0.0));\n";
    code += "    float3 gb = hash3(i + float3(1.0, 0.0, 0.0));\n";
    code += "    float3 gc = hash3(i + float3(0.0, 1.0, 0.0));\n";
    code += "    float3 gd = hash3(i + float3(1.0, 1.0, 0.0));\n";
    code += "    float3 ge = hash3(i + float3(0.0, 0.0, 1.0));\n";
    code += "    float3 gf = hash3(i + float3(1.0, 0.0, 1.0));\n";
    code += "    float3 gg = hash3(i + float3(0.0, 1.0, 1.0));\n";
    code += "    float3 gh = hash3(i + float3(1.0, 1.0, 1.0));\n";
    
    code += "    float va = dot(ga, f - float3(0.0, 0.0, 0.0));\n";
    code += "    float vb = dot(gb, f - float3(1.0, 0.0, 0.0));\n";
    code += "    float vc = dot(gc, f - float3(0.0, 1.0, 0.0));\n";
    code += "    float vd = dot(gd, f - float3(1.0, 1.0, 0.0));\n";
    code += "    float ve = dot(ge, f - float3(0.0, 0.0, 1.0));\n";
    code += "    float vf = dot(gf, f - float3(1.0, 0.0, 1.0));\n";
    code += "    float vg = dot(gg, f - float3(0.0, 1.0, 1.0));\n";
    code += "    float vh = dot(gh, f - float3(1.0, 1.0, 1.0));\n";
    
    code += "    float3 du = 6.0 * f * (1.0 - f);\n";
    code += "    gradient = ga * (1.0 - u.x) * (1.0 - u.y) * (1.0 - u.z) +\n";
    code += "               gb * u.x * (1.0 - u.y) * (1.0 - u.z) +\n";
    code += "               gc * (1.0 - u.x) * u.y * (1.0 - u.z) +\n";
    code += "               gd * u.x * u.y * (1.0 - u.z) +\n";
    code += "               ge * (1.0 - u.x) * (1.0 - u.y) * u.z +\n";
    code += "               gf * u.x * (1.0 - u.y) * u.z +\n";
    code += "               gg * (1.0 - u.x) * u.y * u.z +\n";
    code += "               gh * u.x * u.y * u.z;\n";
    
    code += "    gradient = gradient * 0.5 + 0.5;\n";
    
    code += "    return va * (1.0 - u.x) * (1.0 - u.y) * (1.0 - u.z) +\n";
    code += "           vb * u.x * (1.0 - u.y) * (1.0 - u.z) +\n";
    code += "           vc * (1.0 - u.x) * u.y * (1.0 - u.z) +\n";
    code += "           vd * u.x * u.y * (1.0 - u.z) +\n";
    code += "           ve * (1.0 - u.x) * (1.0 - u.y) * u.z +\n";
    code += "           vf * u.x * (1.0 - u.y) * u.z +\n";
    code += "           vg * (1.0 - u.x) * u.y * u.z +\n";
    code += "           vh * u.x * u.y * u.z;\n";
    code += "}\n\n";
    
    // Call the noise function with our inputs
    code += "{\n";
    code += "    float3 gradientTemp;\n";
    code += "    " + noiseVar + " = perlinNoise(" + positionVar + " * " + scaleVar + ", gradientTemp);\n";
    code += "    " + noiseVar + " = " + noiseVar + " * 0.5 + 0.5;\n";  // Remap from [-1,1] to [0,1]
    code += "    " + gradientVar + " = gradientTemp;\n";
    code += "}\n";
    
    return true;
}

// Main function to demonstrate how to register the custom node
int main() {
    // Register the custom node with the CustomNodeManager
    REGISTER_CUSTOM_NODE(NoiseNode);
    
    // In a real application, you would typically register nodes at startup
    // and then use them in your shader graph editor
    
    return 0;
}