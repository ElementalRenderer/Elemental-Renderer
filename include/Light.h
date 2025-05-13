/**
 * @file Light.h
 * @brief Light system for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_LIGHT_H
#define ELEMENTAL_RENDERER_LIGHT_H

#include <glm/glm.hpp>
#include <memory>

namespace ElementalRenderer {

/**
 * @brief Light types
 */
enum class LightType {
    DIRECTIONAL,
    POINT,
    SPOT,
    AREA
};

/**
 * @brief Base class for all light types
 */
class Light {
public:
    /**
     * @brief Default constructor
     */
    Light();
    
    /**
     * @brief Constructor with light type
     * @param type Light type
     */
    explicit Light(LightType type);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~Light() = default;
    
    /**
     * @brief Set the light color
     * @param color RGB color of the light
     */
    void setColor(const glm::vec3& color);
    
    /**
     * @brief Get the light color
     * @return The light color
     */
    glm::vec3 getColor() const;
    
    /**
     * @brief Set the light intensity
     * @param intensity Intensity value
     */
    void setIntensity(float intensity);
    
    /**
     * @brief Get the light intensity
     * @return The light intensity
     */
    float getIntensity() const;
    
    /**
     * @brief Set whether the light casts shadows
     * @param castShadows True if the light should cast shadows
     */
    void setCastShadows(bool castShadows);
    
    /**
     * @brief Check if the light casts shadows
     * @return True if the light casts shadows
     */
    bool getCastShadows() const;
    
    /**
     * @brief Get the light type
     * @return The light type
     */
    LightType getType() const;
    
    /**
     * @brief Create a directional light
     * @param direction Direction of the light
     * @param color RGB color of the light
     * @param intensity Intensity of the light
     * @return Shared pointer to the created light
     */
    static std::shared_ptr<Light> createDirectionalLight(
        const glm::vec3& direction = glm::vec3(0.0f, -1.0f, 0.0f),
        const glm::vec3& color = glm::vec3(1.0f),
        float intensity = 1.0f);
    
    /**
     * @brief Create a point light
     * @param position Position of the light
     * @param color RGB color of the light
     * @param intensity Intensity of the light
     * @param range Range of the light
     * @return Shared pointer to the created light
     */
    static std::shared_ptr<Light> createPointLight(
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::vec3& color = glm::vec3(1.0f),
        float intensity = 1.0f,
        float range = 10.0f);
    
    /**
     * @brief Create a spot light
     * @param position Position of the light
     * @param direction Direction of the light
     * @param color RGB color of the light
     * @param intensity Intensity of the light
     * @param range Range of the light
     * @param innerAngle Inner cone angle in degrees
     * @param outerAngle Outer cone angle in degrees
     * @return Shared pointer to the created light
     */
    static std::shared_ptr<Light> createSpotLight(
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::vec3& direction = glm::vec3(0.0f, -1.0f, 0.0f),
        const glm::vec3& color = glm::vec3(1.0f),
        float intensity = 1.0f,
        float range = 10.0f,
        float innerAngle = 30.0f,
        float outerAngle = 45.0f);
/**
 * @file Light.h
 * @brief Light system for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_LIGHT_H
#define ELEMENTAL_RENDERER_LIGHT_H

#include <glm/glm.hpp>
#include <memory>

namespace ElementalRenderer {

/**
 * @brief Light types
 */
enum class LightType {
    DIRECTIONAL,
    POINT,
    SPOT
};

/**
 * @brief Class for handling lights
 */
class Light {
public:
    Light(LightType type = LightType::POINT);
    
    void setPosition(const glm::vec3& position);
    
    glm::vec3 getPosition() const;
    
    void setDirection(const glm::vec3& direction);
    
    glm::vec3 getDirection() const;
    
    void setColor(const glm::vec3& color);
    
    glm::vec3 getColor() const;
    
    void setIntensity(float intensity);
    
    float getIntensity() const;
    
    void setRange(float range);
    
    float getRange() const;
    
    void setInnerConeAngle(float angleDegrees);
    
    float getInnerConeAngle() const;
    
    void setOuterConeAngle(float angleDegrees);
    
    float getOuterConeAngle() const;
    
    void setType(LightType type);
    
    LightType getType() const;
    
    static std::shared_ptr<Light> createDirectionalLight(
        const glm::vec3& direction = glm::vec3(0.0f, -1.0f, 0.0f),
        const glm::vec3& color = glm::vec3(1.0f),
        float intensity = 1.0f);
    
    static std::shared_ptr<Light> createPointLight(
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::vec3& color = glm::vec3(1.0f),
        float intensity = 1.0f,
        float range = 10.0f);
    
    static std::shared_ptr<Light> createSpotLight(
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::vec3& direction = glm::vec3(0.0f, -1.0f, 0.0f),
        const glm::vec3& color = glm::vec3(1.0f),
        float intensity = 1.0f,
        float range = 10.0f,
        float innerAngleDegrees = 15.0f,
        float outerAngleDegrees = 30.0f);

private:
    LightType m_type;
    glm::vec3 m_position;
    glm::vec3 m_direction;
    glm::vec3 m_color;
    float m_intensity;
    float m_range;
    float m_innerConeAngle;
    float m_outerConeAngle;
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_LIGHT_H
protected:
    LightType m_type;
    glm::vec3 m_color;
    float m_intensity;
    bool m_castShadows;
};

/**
 * @brief Directional light (sun light)
 */
class DirectionalLight : public Light {
public:
    /**
     * @brief Default constructor
     */
    DirectionalLight();
    
    /**
     * @brief Constructor with direction
     * @param direction Direction of the light
     * @param color RGB color of the light
     * @param intensity Intensity of the light
     */
    DirectionalLight(const glm::vec3& direction, const glm::vec3& color = glm::vec3(1.0f), float intensity = 1.0f);
    
    /**
     * @brief Set the light direction
     * @param direction Direction vector
     */
    void setDirection(const glm::vec3& direction);
    
    /**
     * @brief Get the light direction
     * @return The direction vector
     */
    glm::vec3 getDirection() const;

private:
    glm::vec3 m_direction;
};

/**
 * @brief Point light (omnidirectional)
 */
class PointLight : public Light {
public:
    /**
     * @brief Default constructor
     */
    PointLight();
    
    /**
     * @brief Constructor with position
     * @param position Position of the light
     * @param color RGB color of the light
     * @param intensity Intensity of the light
     * @param range Range of the light
     */
    PointLight(const glm::vec3& position, const glm::vec3& color = glm::vec3(1.0f), 
              float intensity = 1.0f, float range = 10.0f);
    
    /**
     * @brief Set the light position
     * @param position Position vector
     */
    void setPosition(const glm::vec3& position);
    
    /**
     * @brief Get the light position
     * @return The position vector
     */
    glm::vec3 getPosition() const;
    
    /**
     * @brief Set the light range
     * @param range Range value
     */
    void setRange(float range);
    
    /**
     * @brief Get the light range
     * @return The range value
     */
    float getRange() const;

private:
    glm::vec3 m_position;
    float m_range;
};

/**
 * @brief Spot light (cone light)
 */
class SpotLight : public Light {
public:
    /**
     * @brief Default constructor
     */
    SpotLight();
    
    /**
     * @brief Constructor with position and direction
     * @param position Position of the light
     * @param direction Direction of the light
     * @param color RGB color of the light
     * @param intensity Intensity of the light
     * @param range Range of the light
     * @param innerAngle Inner cone angle in degrees
     * @param outerAngle Outer cone angle in degrees
     */
    SpotLight(const glm::vec3& position, const glm::vec3& direction, 
             const glm::vec3& color = glm::vec3(1.0f), float intensity = 1.0f,
             float range = 10.0f, float innerAngle = 30.0f, float outerAngle = 45.0f);
    
    /**
     * @brief Set the light position
     * @param position Position vector
     */
    void setPosition(const glm::vec3& position);
    
    /**
     * @brief Get the light position
     * @return The position vector
     */
    glm::vec3 getPosition() const;
    
    /**
     * @brief Set the light direction
     * @param direction Direction vector
     */
    void setDirection(const glm::vec3& direction);
    
    /**
     * @brief Get the light direction
     * @return The direction vector
     */
    glm::vec3 getDirection() const;
    
    /**
     * @brief Set the light range
     * @param range Range value
     */
    void setRange(float range);
    
    /**
     * @brief Get the light range
     * @return The range value
     */
    float getRange() const;
    
    /**
     * @brief Set the inner cone angle
     * @param degrees Angle in degrees
     */
    void setInnerAngle(float degrees);
    
    /**
     * @brief Get the inner cone angle
     * @return Angle in degrees
     */
    float getInnerAngle() const;
    
    /**
     * @brief Set the outer cone angle
     * @param degrees Angle in degrees
     */
    void setOuterAngle(float degrees);
    
    /**
     * @brief Get the outer cone angle
     * @return Angle in degrees
     */
    float getOuterAngle() const;

private:
    glm::vec3 m_position;
    glm::vec3 m_direction;
    float m_range;
    float m_innerAngle;
    float m_outerAngle;
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_LIGHT_H
