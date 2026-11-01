#pragma once

#include "../vendors/glad/include/glad/glad.h"
#include "../vendors/GLFW/include/GLFW/glfw3.h"
#include "../vendors/glm/glm/glm.hpp"

#include <array>
#include <unordered_map>

class OpenGLModInput
{
    GLFWwindow *reference_window;

    std::array<bool, GLFW_KEY_LAST + 1> key_down{};
    std::array<bool, GLFW_KEY_LAST + 1> key_pressed{};
    std::array<bool, GLFW_KEY_LAST + 1> key_released{};

    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> button_down{};
    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> button_pressed{};
    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> button_released{};

    glm::vec2 mouse_pos{0.0f};
    glm::vec2 mouse_delta{0.0f};

    float scroll_delta = 0.0f;
    bool first_mouse = true;

    static void keyCallback(GLFWwindow *reference_window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow *reference_window, int button, int action, int mods);
    static void cursorCallback(GLFWwindow *reference_window, double x, double y);
    static void scrollCallback(GLFWwindow *reference_window, double xoffset, double yoffset);

public:
    OpenGLModInput(const OpenGLModInput&) = delete;
    OpenGLModInput& operator=(const OpenGLModInput&) = delete;

    explicit OpenGLModInput(GLFWwindow *reference_window);
    ~OpenGLModInput();

    void newFrame();

    bool keyDown(int key) const;
    bool keyPressed(int key) const;
    bool keyReleased(int key) const;

    bool mouseDown(int button) const;
    bool mousePressed(int button) const;
    bool mouseReleased(int button) const;

    glm::vec2 mousePosition() const;
    glm::vec2 mouseDelta() const;
    float scrollDelta() const;

    void setCursorMode(int mode);
};
