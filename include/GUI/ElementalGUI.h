/**
 * @file ElementalGUI.h
 * @brief Custom GUI rendering system for Elemental Renderer
 */

#ifndef ELEMENTAL_RENDERER_ELEMENTAL_GUI_H
#define ELEMENTAL_RENDERER_ELEMENTAL_GUI_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <glm/glm.hpp>

// Forward declaration of GLFWwindow
struct GLFWwindow;

namespace ElementalRenderer {

// Forward declarations
class Shader;
class Texture;

/**
 * @brief Base class for all GUI elements
 */
class GUIElement {
public:
    /**
     * @brief Constructor
     * @param position Element position
     * @param size Element size
     */
    GUIElement(const glm::vec2& position, const glm::vec2& size);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~GUIElement();
    
    /**
     * @brief Render the element
     */
    virtual void render() = 0;
    
    /**
     * @brief Process mouse input for the element
     * @param mousePos Mouse position
     * @param mouseButton Mouse button state
     * @return true if the input was handled by this element
     */
    virtual bool processMouseInput(const glm::vec2& mousePos, int mouseButton);
    
    /**
     * @brief Process key input for the element
     * @param key Key code
     * @param action Key action (press, release)
     * @return true if the input was handled by this element
     */
    virtual bool processKeyInput(int key, int action);
    
    /**
     * @brief Check if a point is inside the element
     * @param point Point to check
     * @return true if the point is inside the element
     */
    bool isPointInside(const glm::vec2& point) const;
    
    /**
     * @brief Set the position of the element
     * @param position New position
     */
    void setPosition(const glm::vec2& position);
    
    /**
     * @brief Get the position of the element
     * @return Position of the element
     */
    const glm::vec2& getPosition() const;
    
    /**
     * @brief Set the size of the element
     * @param size New size
     */
    void setSize(const glm::vec2& size);
    
    /**
     * @brief Get the size of the element
     * @return Size of the element
     */
    const glm::vec2& getSize() const;
    
    /**
     * @brief Set visibility of the element
     * @param visible true to make the element visible, false to hide it
     */
    void setVisible(bool visible);
    
    /**
     * @brief Check if the element is visible
     * @return true if the element is visible
     */
    bool isVisible() const;
    
    /**
     * @brief Set the element to be enabled or disabled
     * @param enabled true to enable the element, false to disable it
     */
    void setEnabled(bool enabled);
    
    /**
     * @brief Check if the element is enabled
     * @return true if the element is enabled
     */
    bool isEnabled() const;

protected:
    glm::vec2 m_position;
    glm::vec2 m_size;
    bool m_visible;
    bool m_enabled;
    bool m_hovered;
};

/**
 * @brief Button element
 */
class Button : public GUIElement {
public:
    /**
     * @brief Constructor
     * @param position Button position
     * @param size Button size
     * @param text Button text
     */
    Button(const glm::vec2& position, const glm::vec2& size, const std::string& text);
    
    /**
     * @brief Render the button
     */
    void render() override;
    
    /**
     * @brief Process mouse input for the button
     * @param mousePos Mouse position
     * @param mouseButton Mouse button state
     * @return true if the input was handled by this button
     */
    bool processMouseInput(const glm::vec2& mousePos, int mouseButton) override;
    
    /**
     * @brief Set button text
     * @param text New text
     */
    void setText(const std::string& text);
    
    /**
     * @brief Get button text
     * @return Button text
     */
    const std::string& getText() const;
    
    /**
     * @brief Set the color of the button
     * @param color Button color
     */
    void setColor(const glm::vec4& color);
    
    /**
     * @brief Set the hover color of the button
     * @param color Button hover color
     */
    void setHoverColor(const glm::vec4& color);
    
    /**
     * @brief Set the text color of the button
     * @param color Text color
     */
    void setTextColor(const glm::vec4& color);
    
    /**
     * @brief Set the callback function for when the button is clicked
     * @param callback Function to call when the button is clicked
     */
    void setOnClickCallback(std::function<void()> callback);

private:
    std::string m_text;
    glm::vec4 m_color;
    glm::vec4 m_hoverColor;
    glm::vec4 m_textColor;
    bool m_pressed;
    std::function<void()> m_onClick;
};

/**
 * @brief Label/text element
 */
class Label : public GUIElement {
public:
    /**
     * @brief Constructor
     * @param position Label position
     * @param text Label text
     */
    Label(const glm::vec2& position, const std::string& text);
    
    /**
     * @brief Render the label
     */
    void render() override;
    
    /**
     * @brief Set label text
     * @param text New text
     */
    void setText(const std::string& text);
    
    /**
     * @brief Get label text
     * @return Label text
     */
    const std::string& getText() const;
    
    /**
     * @brief Set the color of the text
     * @param color Text color
     */
    void setTextColor(const glm::vec4& color);
    
    /**
     * @brief Set the text scale
     * @param scale Text scale
     */
    void setTextScale(float scale);

private:
    std::string m_text;
    glm::vec4 m_textColor;
    float m_textScale;
};

/**
 * @brief Checkbox element
 */
class Checkbox : public GUIElement {
public:
    /**
     * @brief Constructor
     * @param position Checkbox position
     * @param size Checkbox size
     * @param label Checkbox label
     * @param checked Initial checked state
     */
    Checkbox(const glm::vec2& position, const glm::vec2& size, const std::string& label, bool checked = false);
    
    /**
     * @brief Render the checkbox
     */
    void render() override;
    
    /**
     * @brief Process mouse input for the checkbox
     * @param mousePos Mouse position
     * @param mouseButton Mouse button state
     * @return true if the input was handled by this checkbox
     */
    bool processMouseInput(const glm::vec2& mousePos, int mouseButton) override;
    
    /**
     * @brief Set whether the checkbox is checked
     * @param checked true to check the checkbox, false to uncheck it
     */
    void setChecked(bool checked);
    
    /**
     * @brief Check if the checkbox is checked
     * @return true if the checkbox is checked
     */
    bool isChecked() const;
    
    /**
     * @brief Set the label text
     * @param label New label text
     */
    void setLabel(const std::string& label);
    
    /**
     * @brief Get the label text
     * @return Label text
     */
    const std::string& getLabel() const;
    
    /**
     * @brief Set the callback function for when the checkbox state changes
     * @param callback Function to call when the checkbox is toggled
     */
    void setOnChangeCallback(std::function<void(bool)> callback);

private:
    std::string m_label;
    bool m_checked;
    glm::vec4 m_boxColor;
    glm::vec4 m_checkColor;
    glm::vec4 m_labelColor;
    std::function<void(bool)> m_onChange;
};

/**
 * @brief Slider element for adjusting numeric values
 */
class Slider : public GUIElement {
public:
    /**
     * @brief Constructor
     * @param position Slider position
     * @param size Slider size
     * @param label Slider label
     * @param minValue Minimum value
     * @param maxValue Maximum value
     * @param value Initial value
     */
    Slider(const glm::vec2& position, const glm::vec2& size, const std::string& label, float minValue, float maxValue, float value);
    
    /**
     * @brief Render the slider
     */
    void render() override;
    
    /**
     * @brief Process mouse input for the slider
     * @param mousePos Mouse position
     * @param mouseButton Mouse button state
     * @return true if the input was handled by this slider
     */
    bool processMouseInput(const glm::vec2& mousePos, int mouseButton) override;
    
    /**
     * @brief Set the current value
     * @param value New value
     */
    void setValue(float value);
    
    /**
     * @brief Get the current value
     * @return Current value
     */
    float getValue() const;
    
    /**
     * @brief Set the minimum and maximum values
     * @param minValue Minimum value
     * @param maxValue Maximum value
     */
    void setRange(float minValue, float maxValue);
    
    /**
     * @brief Set the label text
     * @param label New label text
     */
    void setLabel(const std::string& label);
    
    /**
     * @brief Get the label text
     * @return Label text
     */
    const std::string& getLabel() const;
    
    /**
     * @brief Set the callback function for when the slider value changes
     * @param callback Function to call when the slider value changes
     */
    void setOnChangeCallback(std::function<void(float)> callback);

private:
    std::string m_label;
    float m_minValue;
    float m_maxValue;
    float m_value;
    bool m_dragging;
    glm::vec4 m_trackColor;
    glm::vec4 m_handleColor;
    glm::vec4 m_activeHandleColor;
    glm::vec4 m_labelColor;
    std::function<void(float)> m_onChange;
    
    float getNormalizedValue() const;
    void setNormalizedValue(float normalizedValue);
};

/**
 * @brief Text input field
 */
class TextInput : public GUIElement {
public:
    /**
     * @brief Constructor
     * @param position Input position
     * @param size Input size
     * @param placeholder Placeholder text
     */
    TextInput(const glm::vec2& position, const glm::vec2& size, const std::string& placeholder = "");
    
    /**
     * @brief Render the text input
     */
    void render() override;
    
    /**
     * @brief Process mouse input for the text input
     * @param mousePos Mouse position
     * @param mouseButton Mouse button state
     * @return true if the input was handled by this text input
     */
    bool processMouseInput(const glm::vec2& mousePos, int mouseButton) override;
    
    /**
     * @brief Process key input for the text input
     * @param key Key code
     * @param action Key action (press, release)
     * @return true if the input was handled by this text input
     */
    bool processKeyInput(int key, int action) override;
    
    /**
     * @brief Set the text in the input field
     * @param text New text
     */
    void setText(const std::string& text);
    
    /**
     * @brief Get the text in the input field
     * @return Input text
     */
    const std::string& getText() const;
    
    /**
     * @brief Set the placeholder text
     * @param placeholder New placeholder text
     */
    void setPlaceholder(const std::string& placeholder);
    
    /**
     * @brief Get the placeholder text
     * @return Placeholder text
     */
    const std::string& getPlaceholder() const;
    
    /**
     * @brief Set the callback function for when the text changes
     * @param callback Function to call when the text changes
     */
    void setOnChangeCallback(std::function<void(const std::string&)> callback);
    
    /**
     * @brief Set the callback function for when Enter is pressed
     * @param callback Function to call when Enter is pressed
     */
    void setOnSubmitCallback(std::function<void(const std::string&)> callback);

private:
    std::string m_text;
    std::string m_placeholder;
    bool m_focused;
    float m_cursorBlinkTime;
    int m_cursorPos;
    glm::vec4 m_backgroundColor;
    glm::vec4 m_focusedBackgroundColor;
    glm::vec4 m_textColor;
    glm::vec4 m_placeholderColor;
    std::function<void(const std::string&)> m_onChange;
    std::function<void(const std::string&)> m_onSubmit;
};

/**
 * @brief Panel container for organizing GUI elements
 */
class Panel : public GUIElement {
public:
    /**
     * @brief Constructor
     * @param position Panel position
     * @param size Panel size
     * @param title Panel title (optional)
     */
    Panel(const glm::vec2& position, const glm::vec2& size, const std::string& title = "");
    
    /**
     * @brief Destructor
     */
    ~Panel() override;
    
    /**
     * @brief Render the panel and its children
     */
    void render() override;
    
    /**
     * @brief Process mouse input for the panel and its children
     * @param mousePos Mouse position
     * @param mouseButton Mouse button state
     * @return true if the input was handled by this panel or its children
     */
    bool processMouseInput(const glm::vec2& mousePos, int mouseButton) override;
    
    /**
     * @brief Process key input for the panel and its children
     * @param key Key code
     * @param action Key action (press, release)
     * @return true if the input was handled by this panel or its children
     */
    bool processKeyInput(int key, int action) override;
    
    /**
     * @brief Add a child element to the panel
     * @param element Element to add
     */
    void addElement(std::shared_ptr<GUIElement> element);
    
    /**
     * @brief Remove a child element from the panel
     * @param element Element to remove
     * @return true if the element was removed
     */
    bool removeElement(std::shared_ptr<GUIElement> element);
    
    /**
     * @brief Set the panel title
     * @param title New title
     */
    void setTitle(const std::string& title);
    
    /**
     * @brief Get the panel title
     * @return Panel title
     */
    const std::string& getTitle() const;
    
    /**
     * @brief Set the background color of the panel
     * @param color Background color
     */
    void setBackgroundColor(const glm::vec4& color);
    
    /**
     * @brief Set whether the panel is draggable
     * @param draggable true to make the panel draggable
     */
    void setDraggable(bool draggable);
    
    /**
     * @brief Check if the panel is draggable
     * @return true if the panel is draggable
     */
    bool isDraggable() const;

private:
    std::string m_title;
    std::vector<std::shared_ptr<GUIElement>> m_children;
    glm::vec4 m_backgroundColor;
    glm::vec4 m_titleBarColor;
    glm::vec4 m_titleTextColor;
    bool m_draggable;
    bool m_dragging;
    glm::vec2 m_dragOffset;
};

/**
 * @brief Main GUI system for managing and rendering GUI elements
 */
class ElementalGUI {
public:
    /**
     * @brief Initialize the GUI system
     * @param window GLFW window
     * @param width Window width
     * @param height Window height
     * @return true if initialization was successful
     */
    static bool initialize(GLFWwindow* window, int width, int height);
    
    /**
     * @brief Shutdown the GUI system
     */
    static void shutdown();
    
    /**
     * @brief Begin a new GUI frame
     */
    static void beginFrame();
    
    /**
     * @brief End the GUI frame and render all elements
     */
    static void endFrame();
    
    /**
     * @brief Process mouse input events
     * @param mousePos Mouse position
     * @param button Mouse button
     * @param action Mouse button action (press, release)
     */
    static void processMouseInput(const glm::vec2& mousePos, int button, int action);
    
    /**
     * @brief Process key input events
     * @param key Key code
     * @param action Key action (press, release)
     */
    static void processKeyInput(int key, int action);
    
    /**
     * @brief Process character input events
     * @param codepoint Unicode codepoint
     */
    static void processCharInput(unsigned int codepoint);
    
    /**
     * @brief Create a new panel
     * @param position Panel position
     * @param size Panel size
     * @param title Panel title (optional)
     * @return Shared pointer to the created panel
     */
    static std::shared_ptr<Panel> createPanel(const glm::vec2& position, const glm::vec2& size, const std::string& title = "");
    
    /**
     * @brief Create a new button
     * @param position Button position
     * @param size Button size
     * @param text Button text
     * @return Shared pointer to the created button
     */
    static std::shared_ptr<Button> createButton(const glm::vec2& position, const glm::vec2& size, const std::string& text);
    
    /**
     * @brief Create a new label
     * @param position Label position
     * @param text Label text
     * @return Shared pointer to the created label
     */
    static std::shared_ptr<Label> createLabel(const glm::vec2& position, const std::string& text);
    
    /**
     * @brief Create a new checkbox
     * @param position Checkbox position
     * @param size Checkbox size
     * @param label Checkbox label
     * @param checked Initial checked state
     * @return Shared pointer to the created checkbox
     */
    static std::shared_ptr<Checkbox> createCheckbox(const glm::vec2& position, const glm::vec2& size, const std::string& label, bool checked = false);
    
    /**
     * @brief Create a new slider
     * @param position Slider position
     * @param size Slider size
     * @param label Slider label
     * @param minValue Minimum value
     * @param maxValue Maximum value
     * @param value Initial value
     * @return Shared pointer to the created slider
     */
    static std::shared_ptr<Slider> createSlider(const glm::vec2& position, const glm::vec2& size, const std::string& label, float minValue, float maxValue, float value);
    
    /**
     * @brief Create a new text input
     * @param position Input position
     * @param size Input size
     * @param placeholder Placeholder text
     * @return Shared pointer to the created text input
     */
    static std::shared_ptr<TextInput> createTextInput(const glm::vec2& position, const glm::vec2& size, const std::string& placeholder = "");
    
    /**
     * @brief Add a root-level GUI element
     * @param element Element to add
     */
    static void addElement(std::shared_ptr<GUIElement> element);
    
    /**
     * @brief Remove a root-level GUI element
     * @param element Element to remove
     * @return true if the element was removed
     */
    static bool removeElement(std::shared_ptr<GUIElement> element);
    
    /**
     * @brief Set the scale of the GUI (affects all elements)
     * @param scale Scale factor
     */
    static void setScale(float scale);
    
    /**
     * @brief Get the current scale of the GUI
     * @return Current scale factor
     */
    static float getScale();
    
    /**
     * @brief Set the default font size
     * @param fontSize Font size in pixels
     */
    static void setDefaultFontSize(float fontSize);
    
    /**
     * @brief Get the default font size
     * @return Default font size in pixels
     */
    static float getDefaultFontSize();
    
    /**
     * @brief Load a font from a file
     * @param fontPath Path to the font file
     * @param fontSize Font size in pixels
     * @return true if the font was loaded successfully
     */
    static bool loadFont(const std::string& fontPath, float fontSize);

private:
    static GLFWwindow* s_window;
    static int s_width;
    static int s_height;
    static std::vector<std::shared_ptr<GUIElement>> s_elements;
    static std::shared_ptr<Shader> s_guiShader;
    static std::shared_ptr<Shader> s_textShader;
    static unsigned int s_quadVAO;
    static float s_scale;
    static float s_defaultFontSize;
    static glm::vec2 s_mousePos;
    static int s_mouseButton;
    static std::shared_ptr<GUIElement> s_focusedElement;
    
    // Render helpers
    static void setupRendering();
    static void renderQuad();
    static void renderText(const std::string& text, float x, float y, float scale, const glm::vec4& color);
    
    // Input helpers
    static bool isMouseButtonPressed(int button);
    static std::shared_ptr<GUIElement> findElementAt(const glm::vec2& position);
};

} // namespace ElementalRenderer

#endif // ELEMENTAL_RENDERER_ELEMENTAL_GUI_H
