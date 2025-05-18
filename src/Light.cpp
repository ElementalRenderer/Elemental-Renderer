/**
 * @file Light.cpp
 * @brief Implementation of light classes
 */

#include "Light.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace ElementalRenderer {

Light::Light()
    : m_type(LightType::DIRECTIONAL)
    , m_color(1.0f, 1.0f, 1.0f)
    , m_intensity(1.0f)
    , m_castShadows(true)
{
}

Light::Light(LightType type)
    : m_type(type)
    , m_color(1.0f, 1.0f, 1.0f)
    , m_intensity(1.0f)
    , m_castShadows(true)
{
}

void Light::setColor(const glm::vec3& color) {
    m_color = color;
}

glm::vec3 Light::getColor() const {
    return m_color;
}

void Light::setIntensity(float intensity) {
    m_intensity = intensity;
}

float Light::getIntensity() const {
    return m_intensity;
}

void Light::setCastShadows(bool castShadows) {
    m_castShadows = castShadows;
}

bool Light::getCastShadows() const {
    return m_castShadows;
}

LightType Light::getType() const {
    return m_type;
}

std::shared_ptr<Light> Light::createDirectionalLight(
    const glm::vec3& direction,
    const glm::vec3& color,
    float intensity)
{
    auto light = std::make_shared<DirectionalLight>(
        glm::normalize(direction), 
        color, 
        intensity
    );
    
    return light;
}

std::shared_ptr<Light> Light::createPointLight(
    const glm::vec3& position,
    const glm::vec3& color,
    float intensity,
    float range)
{
    auto light = std::make_shared<PointLight>(
        position,
        color,
        intensity,
        range
    );
    
    return light;
}

std::shared_ptr<Light> Light::createSpotLight(
    const glm::vec3& position,
    const glm::vec3& direction,
    const glm::vec3& color,
    float intensity,
    float range,
    float innerAngle,
    float outerAngle)
{
    auto light = std::make_shared<SpotLight>(
        position,
        glm::normalize(direction),
        color,
        intensity,
        range,
        innerAngle,
        outerAngle
    );
    
    return light;
}

DirectionalLight::DirectionalLight()
    : Light(LightType::DIRECTIONAL)
    , m_direction(0.0f, -1.0f, 0.0f)
{
}

DirectionalLight::DirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity)
    : Light(LightType::DIRECTIONAL)
    , m_direction(glm::normalize(direction))
{
    setColor(color);
    setIntensity(intensity);
}

void DirectionalLight::setDirection(const glm::vec3& direction) {
    m_direction = glm::normalize(direction);
}

glm::vec3 DirectionalLight::getDirection() const {
    return m_direction;
}

PointLight::PointLight()
    : Light(LightType::POINT)
    , m_position(0.0f)
    , m_range(10.0f)
{
}

PointLight::PointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float range)
    : Light(LightType::POINT)
    , m_position(position)
    , m_range(range)
{
    setColor(color);
    setIntensity(intensity);
}

void PointLight::setPosition(const glm::vec3& position) {
    m_position = position;
}

glm::vec3 PointLight::getPosition() const {
    return m_position;
}

void PointLight::setRange(float range) {
    m_range = range;
}

float PointLight::getRange() const {
    return m_range;
}

// SpotLight implementation
SpotLight::SpotLight()
    : Light(LightType::SPOT)
    , m_position(0.0f)
    , m_direction(0.0f, -1.0f, 0.0f)
    , m_range(10.0f)
    , m_innerAngle(30.0f)
    , m_outerAngle(45.0f)
{
}

SpotLight::SpotLight(const glm::vec3& position, const glm::vec3& direction, 
                    const glm::vec3& color, float intensity,
                    float range, float innerAngle, float outerAngle)
    : Light(LightType::SPOT)
    , m_position(position)
    , m_direction(glm::normalize(direction))
    , m_range(range)
    , m_innerAngle(innerAngle)
    , m_outerAngle(outerAngle)
{
    setColor(color);
    setIntensity(intensity);

    if (m_innerAngle > m_outerAngle) {
        std::cerr << "Warning: Inner angle is greater than outer angle. Swapping values." << std::endl;
        std::swap(m_innerAngle, m_outerAngle);
    }
}

void SpotLight::setPosition(const glm::vec3& position) {
    m_position = position;
}

glm::vec3 SpotLight::getPosition() const {
    return m_position;
}

void SpotLight::setDirection(const glm::vec3& direction) {
    m_direction = glm::normalize(direction);
}

glm::vec3 SpotLight::getDirection() const {
    return m_direction;
}

void SpotLight::setRange(float range) {
    m_range = range;
}

float SpotLight::getRange() const {
    return m_range;
}

void SpotLight::setInnerAngle(float degrees) {
    m_innerAngle = degrees;

    if (m_innerAngle > m_outerAngle) {
        m_innerAngle = m_outerAngle;
    }
}

float SpotLight::getInnerAngle() const {
    return m_innerAngle;
}

void SpotLight::setOuterAngle(float degrees) {
    m_outerAngle = degrees;

    if (m_outerAngle < m_innerAngle) {
        m_outerAngle = m_innerAngle;
    }
}

float SpotLight::getOuterAngle() const {
    return m_outerAngle;
}

} // namespace ElementalRenderer
