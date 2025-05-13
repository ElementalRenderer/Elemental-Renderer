/**
 * @file Shader.h
 * @brief Shader management for Elemental Renderer
 */
/**
 * @file Shader.h
 * @brief Shader handling for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_SHADER_H
#define ELEMENTAL_RENDERER_SHADER_H

#include <string>
#include <memory>
#include <glm/glm.hpp>

namespace ElementalRenderer {

/**
 * @brief Class for handling shaders
 */
class Shader {
public:
    Shader();
    
    virtual ~Shader();
    
    bool compile(const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource = "");
    
    void use() const;
    
    unsigned int getId() const;
    
    void setBool(const std::string& name, bool value) const;
    
    void setInt(const std::string& name, int value) const;
    
    void setFloat(const std::string& name, float value) const;
    
    void setVec2(const std::string& name, const glm::vec2& value) const;
    
    void setVec3(const std::string& name, const glm::vec3& value) const;
    
    void setVec4(const std::string& name, const glm::vec4& value) const;
    
    void setMat2(const std::string& name, const glm::mat2& value) const;
    
    void setMat3(const std::string& name, const glm::mat3& value) const;
    
    void setMat4(const std::string& name, const glm::mat4& value) const;
    
    static std::shared_ptr<Shader> createFromFiles(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "");
    
    static std::shared_ptr<Shader> createFromSource(const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource = "");

protected:
    unsigned int m_id;
    
    void checkCompileErrors(unsigned int shader, const std::string& type);
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_SHADER_H
#ifndef ELEMENTAL_RENDERER_SHADER_H
#define ELEMENTAL_RENDERER_SHADER_H

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace ElementalRenderer {

/**
 * @brief Class for handling shader programs
 */
class Shader {
public:
    /**
     * @brief Shader types
     */
    enum class Type {
        VERTEX,
        FRAGMENT,
        GEOMETRY,
        COMPUTE
    };

    /**
     * @brief Default constructor
     */
    Shader();
    
    /**
     * @brief Destructor
     */
    ~Shader();
    
    /**
     * @brief Load shader from files
     * @param vertexPath Path to vertex shader source
     * @param fragmentPath Path to fragment shader source
     * @param geometryPath Optional path to geometry shader source
     * @return true if loading was successful, false otherwise
     */
    bool loadFromFile(const std::string& vertexPath, const std::string& fragmentPath, 
                     const std::string& geometryPath = "");
    
    /**
     * @brief Load shader from source strings
     * @param vertexSource Vertex shader source code
     * @param fragmentSource Fragment shader source code
     * @param geometrySource Optional geometry shader source code
     * @return true if loading was successful, false otherwise
     */
    bool loadFromSource(const std::string& vertexSource, const std::string& fragmentSource,
                        const std::string& geometrySource = "");
    
    /**
     * @brief Activate the shader for rendering
     */
    void use() const;
    
    /**
     * @brief Set a boolean uniform value
     * @param name Name of the uniform
     * @param value Value to set
     */
    void setBool(const std::string& name, bool value) const;
    
    /**
     * @brief Set an integer uniform value
     * @param name Name of the uniform
     * @param value Value to set
     */
    void setInt(const std::string& name, int value) const;
    
    /**
     * @brief Set a float uniform value
     * @param name Name of the uniform
     * @param value Value to set
     */
    void setFloat(const std::string& name, float value) const;
    
    /**
     * @brief Set a vec2 uniform value
     * @param name Name of the uniform
     * @param value Value to set
     */
    void setVec2(const std::string& name, const glm::vec2& value) const;
    
    /**
     * @brief Set a vec3 uniform value
     * @param name Name of the uniform
     * @param value Value to set
     */
    void setVec3(const std::string& name, const glm::vec3& value) const;
    
    /**
     * @brief Set a vec4 uniform value
     * @param name Name of the uniform
     * @param value Value to set
     */
    void setVec4(const std::string& name, const glm::vec4& value) const;
    
    /**
     * @brief Set a mat2 uniform value
     * @param name Name of the uniform
     * @param value Value to set
     */
    void setMat2(const std::string& name, const glm::mat2& value) const;
    
    /**
     * @brief Set a mat3 uniform value
     * @param name Name of the uniform
     * @param value Value to set
     */
    void setMat3(const std::string& name, const glm::mat3& value) const;
    
    /**
     * @brief Set a mat4 uniform value
     * @param name Name of the uniform
     * @param value Value to set
     */
    void setMat4(const std::string& name, const glm::mat4& value) const;
    
    /**
     * @brief Get the shader program ID
     * @return The shader program ID
     */
    unsigned int getId() const;

private:
    unsigned int m_programId;
    mutable std::unordered_map<std::string, int> m_uniformLocationCache;
    
    /**
     * @brief Get the location of a uniform
     * @param name Name of the uniform
     * @return Location of the uniform
     */
    int getUniformLocation(const std::string& name) const;
    
    /**
     * @brief Compile a shader from source
     * @param type Type of shader to compile
     * @param source Source code of the shader
     * @return Shader ID
     */
    unsigned int compileShader(Type type, const std::string& source);
    
    /**
     * @brief Check if a shader compilation was successful
     * @param shaderId ID of the shader to check
     * @return true if compilation was successful, false otherwise
     */
    bool checkShaderCompilation(unsigned int shaderId);
    
    /**
     * @brief Check if a program linking was successful
     * @param programId ID of the program to check
     * @return true if linking was successful, false otherwise
     */
    bool checkProgramLinking(unsigned int programId);
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_SHADER_H
