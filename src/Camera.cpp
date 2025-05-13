/**
 * @file Camera.cpp
 * @brief Implementation of the Camera class
 */

#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <cmath>

namespace ElementalRenderer {

Camera::Camera()
    : m_position(0.0f, 0.0f, 5.0f)
    , m_target(0.0f, 0.0f, 0.0f)
    , m_up(0.0f, 1.0f, 0.0f)
    , m_right(1.0f, 0.0f, 0.0f)
    , m_forward(0.0f, 0.0f, -1.0f)
    , m_projectionType(ProjectionType::PERSPECTIVE)
    , m_fov(45.0f)
    , m_aspectRatio(16.0f / 9.0f)
    , m_nearPlane(0.1f)
    , m_farPlane(1000.0f)
    , m_left(-10.0f)
    , m_right(10.0f)
    , m_bottom(-10.0f)
    , m_top(10.0f)
    , m_viewMatrix(1.0f)
    , m_projectionMatrix(1.0f)
    , m_viewProjectionMatrix(1.0f)
    , m_viewDirty(true)
    , m_projectionDirty(true)
{
    updateCameraVectors();
}

Camera::Camera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
    : m_position(position)
    , m_target(target)
    , m_up(up)
    , m_right(1.0f, 0.0f, 0.0f)
    , m_forward(0.0f, 0.0f, -1.0f)
    , m_projectionType(ProjectionType::PERSPECTIVE)
    , m_fov(45.0f)
    , m_aspectRatio(16.0f / 9.0f)
    , m_nearPlane(0.1f)
    , m_farPlane(1000.0f)
    , m_left(-10.0f)
    , m_right(10.0f)
    , m_bottom(-10.0f)
    , m_top(10.0f)
    , m_viewMatrix(1.0f)
    , m_projectionMatrix(1.0f)
    , m_viewProjectionMatrix(1.0f)
    , m_viewDirty(true)
    , m_projectionDirty(true)
{
    updateCameraVectors();
}

void Camera::setPosition(const glm::vec3& position) {
    m_position = position;
    m_viewDirty = true;
}

glm::vec3 Camera::getPosition() const {
    return m_position;
}

void Camera::setTarget(const glm::vec3& target) {
    m_target = target;
    m_viewDirty = true;
    updateCameraVectors();
}

glm::vec3 Camera::getTarget() const {
    return m_target;
}

void Camera::setUp(const glm::vec3& up) {
    m_up = up;
    m_viewDirty = true;
    updateCameraVectors();
}

glm::vec3 Camera::getUp() const {
    return m_up;
}

void Camera::setPerspective(float fovDegrees, float aspectRatio, float nearPlane, float farPlane) {
    m_projectionType = ProjectionType::PERSPECTIVE;
    m_fov = fovDegrees;
    m_aspectRatio = aspectRatio;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
    m_projectionDirty = true;
}

void Camera::setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
    m_projectionType = ProjectionType::ORTHOGRAPHIC;
    m_left = left;
    m_right = right;
    m_bottom = bottom;
    m_top = top;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
    m_projectionDirty = true;
}

void Camera::setProjectionType(ProjectionType type) {
    m_projectionType = type;
    m_projectionDirty = true;
}

ProjectionType Camera::getProjectionType() const {
    return m_projectionType;
}

glm::mat4 Camera::getViewMatrix() const {
    if (m_viewDirty) {
        m_viewMatrix = glm::lookAt(m_position, m_target, m_up);
        m_viewDirty = false;
        
        // also just update the view-projection matrix
        m_viewProjectionMatrix = getProjectionMatrix() * m_viewMatrix;
    }
    
    return m_viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix() const {
    if (m_projectionDirty) {
        if (m_projectionType == ProjectionType::PERSPECTIVE) {
            float fovRadians = glm::radians(m_fov);
            m_projectionMatrix = glm::perspective(fovRadians, m_aspectRatio, m_nearPlane, m_farPlane);
        } else { // ORTHOGRAPHIC :3
            m_projectionMatrix = glm::ortho(m_left, m_right, m_bottom, m_top, m_nearPlane, m_farPlane);
        }
        
        m_projectionDirty = false;
        
        // updating the view-projection
        m_viewProjectionMatrix = m_projectionMatrix * getViewMatrix();
    }
    
    return m_projectionMatrix;
}

glm::mat4 Camera::getViewProjectionMatrix() const {
    // wanna make sure both arent gonna be fucked later
    getViewMatrix();
    getProjectionMatrix();
    
    return m_viewProjectionMatrix;
}

void Camera::moveForward(float distance) {
    glm::vec3 forward = glm::normalize(m_target - m_position);
    m_position += forward * distance;
    m_target += forward * distance;
    m_viewDirty = true;
}

void Camera::moveRight(float distance) {
    m_position += m_right * distance;
    m_target += m_right * distance;
    m_viewDirty = true;
}

void Camera::moveUp(float distance) {
    glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
    m_position += worldUp * distance;
    m_target += worldUp * distance;
    m_viewDirty = true;
}

void Camera::pitch(float degrees) {
    glm::quat rotation = glm::angleAxis(glm::radians(degrees), m_right);
    glm::vec3 direction = m_target - m_position;
    direction = rotation * direction;
    m_target = m_position + direction;
    updateCameraVectors();
    m_viewDirty = true;
}

void Camera::yaw(float degrees) {
    glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
    glm::quat rotation = glm::angleAxis(glm::radians(degrees), worldUp);
    glm::vec3 direction = m_target - m_position;
    direction = rotation * direction;
    m_target = m_position + direction;
    updateCameraVectors();
    m_viewDirty = true;
}

void Camera::roll(float degrees) {
    glm::vec3 forward = glm::normalize(m_target - m_position);
    glm::quat rotation = glm::angleAxis(glm::radians(degrees), forward);
    m_up = rotation * m_up;
    updateCameraVectors();
    m_viewDirty = true;
}

Camera Camera::createPerspectiveCamera(
    const glm::vec3& position,
    const glm::vec3& target,
    float fovDegrees,
    float aspectRatio,
    float nearPlane,
    float farPlane)
{
    Camera camera(position, target);
    camera.setPerspective(fovDegrees, aspectRatio, nearPlane, farPlane);
    return camera;
}

Camera Camera::createOrthographicCamera(
    const glm::vec3& position,
    const glm::vec3& target,
    float size,
    float aspectRatio,
    float nearPlane,
    float farPlane)
{
    float halfHeight = size * 0.5f;
    float halfWidth = halfHeight * aspectRatio;
    
    Camera camera(position, target);
    camera.setOrthographic(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane, farPlane);
    return camera;
}

void Camera::updateCameraVectors() {
    m_forward = glm::normalize(m_target - m_position);
    m_right = glm::normalize(glm::cross(m_forward, m_up));
    m_up = glm::normalize(glm::cross(m_right, m_forward));
}

} // namespace ElementalRenderer
