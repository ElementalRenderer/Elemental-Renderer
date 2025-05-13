/**
 * @file Camera.h
 * @brief Camera system for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_CAMERA_H
#define ELEMENTAL_RENDERER_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace ElementalRenderer {

/**
 * @brief Camera projection types
 */
enum class ProjectionType {
    PERSPECTIVE,
    ORTHOGRAPHIC
};

/**
 * @brief Class for handling camera view and projection
 */
class Camera {
public:
    Camera();
    
    Camera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));
    
    void setPosition(const glm::vec3& position);
    
    glm::vec3 getPosition() const;
    
    void setTarget(const glm::vec3& target);
    
    glm::vec3 getTarget() const;
    
    void setUp(const glm::vec3& up);
    
    glm::vec3 getUp() const;
    
    void setPerspective(float fovDegrees, float aspectRatio, float nearPlane, float farPlane);
    
    void setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);
    
    void setProjectionType(ProjectionType type);
    
    ProjectionType getProjectionType() const;
    
    glm::mat4 getViewMatrix() const;
    
    glm::mat4 getProjectionMatrix() const;
    
    glm::mat4 getViewProjectionMatrix() const;
    
    void moveForward(float distance);
    
    void moveRight(float distance);
    
    void moveUp(float distance);
    
    void pitch(float degrees);
    
    void yaw(float degrees);
    
    void roll(float degrees);
    
    static Camera createPerspectiveCamera(
        const glm::vec3& position = glm::vec3(0.0f, 0.0f, 5.0f),
        const glm::vec3& target = glm::vec3(0.0f),
        float fovDegrees = 45.0f,
        float aspectRatio = 16.0f/9.0f,
        float nearPlane = 0.1f,
        float farPlane = 1000.0f);
    
    static Camera createOrthographicCamera(
        const glm::vec3& position = glm::vec3(0.0f, 0.0f, 5.0f),
        const glm::vec3& target = glm::vec3(0.0f),
        float size = 10.0f,
        float aspectRatio = 16.0f/9.0f,
        float nearPlane = 0.1f,
        float farPlane = 1000.0f);

private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_forward;
    
    ProjectionType m_projectionType;
    
    // Perspective parameters
    float m_fov;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;
    
    // Orthographic parameters
    float m_left;
    float m_right;
    float m_bottom;
    float m_top;
    
    // Cached matrices
    mutable glm::mat4 m_viewMatrix;
    mutable glm::mat4 m_projectionMatrix;
    mutable glm::mat4 m_viewProjectionMatrix;
    
    // Dirty flags
    mutable bool m_viewDirty;
    mutable bool m_projectionDirty;
    
    void updateCameraVectors();
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_CAMERA_H
