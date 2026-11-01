#include "OpenGLModInput.hpp"

static std::unordered_map<GLFWwindow*, OpenGLModInput*> &registry()
{
    static std::unordered_map<GLFWwindow*, OpenGLModInput*> r;
    return r;
}

static OpenGLModInput* find(GLFWwindow *window)
{
    auto it = registry().find(window);
    return it == registry().end() ? nullptr : it->second;
}

OpenGLModInput::OpenGLModInput(GLFWwindow *reference_window)
{
    this->reference_window = reference_window;

    registry()[reference_window] = this;

    glfwSetKeyCallback(reference_window, keyCallback);
    glfwSetMouseButtonCallback(reference_window, mouseButtonCallback);
    glfwSetCursorPosCallback(reference_window, cursorCallback);
    glfwSetScrollCallback(reference_window, scrollCallback);
}

OpenGLModInput::~OpenGLModInput()
{
    glfwSetKeyCallback(reference_window, nullptr);
    glfwSetMouseButtonCallback(reference_window, nullptr);
    glfwSetCursorPosCallback(reference_window, nullptr);
    glfwSetScrollCallback(reference_window, nullptr);
    registry().erase(reference_window);
}

void OpenGLModInput::newFrame()
{
    key_pressed.fill(false);
    key_released.fill(false);
    button_pressed.fill(false);
    button_released.fill(false);
    mouse_delta = glm::vec2(0.0f);
    scroll_delta = 0.0f;
}

bool OpenGLModInput::keyDown(int key) const
{
    return key >= 0 && key <= GLFW_KEY_LAST && key_down[key];
}

bool OpenGLModInput::keyPressed(int key) const
{
    return key >= 0 && key <= GLFW_KEY_LAST && key_pressed[key];
}

bool OpenGLModInput::keyReleased(int key) const
{
    return key >= 0 && key <= GLFW_KEY_LAST && key_released[key];
}

bool OpenGLModInput::mouseDown(int b) const
{
    return b >= 0 && b <= GLFW_MOUSE_BUTTON_LAST && button_down[b];
}

bool OpenGLModInput::mousePressed(int b) const
{
    return b >= 0 && b <= GLFW_MOUSE_BUTTON_LAST && button_pressed[b];
}

bool OpenGLModInput::mouseReleased(int b) const
{
    return b >= 0 && b <= GLFW_MOUSE_BUTTON_LAST && button_released[b];
}

void OpenGLModInput::setCursorMode(int mode)
{
    glfwSetInputMode(reference_window, GLFW_CURSOR, mode);
    first_mouse = true;
}

void OpenGLModInput::keyCallback(GLFWwindow *reference_window, int key, int scancode, int action, int mods)
{
    OpenGLModInput *in = find(reference_window);
    if (!in || key < 0 || key > GLFW_KEY_LAST) return;

    if (action == GLFW_PRESS)
    {
        in->key_down[key] = true;
        in->key_pressed[key] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        in->key_down[key] = false;
        in->key_released[key] = true;
    }
}

void OpenGLModInput::mouseButtonCallback(GLFWwindow *reference_window, int button, int action, int mods)
{
    OpenGLModInput *in = find(reference_window);
    if (!in || button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return;

    if (action == GLFW_PRESS)
    {
        in->button_down[button] = true;
        in->button_pressed[button] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        in->button_down[button] = false;
        in->button_released[button] = true;
    }
}

void OpenGLModInput::cursorCallback(GLFWwindow* reference_window, double x, double y)
{
    OpenGLModInput *in = find(reference_window);
    if (!in) return;

    glm::vec2 pos(static_cast<float>(x), static_cast<float>(y));
    if (in->first_mouse)
    {
        in->mouse_pos = pos;
        in->first_mouse = false;
    }

    in->mouse_delta += pos - in->mouse_pos;
    in->mouse_pos = pos;
}

void OpenGLModInput::scrollCallback(GLFWwindow *reference_window, double xoffset, double yoffset)
{
    OpenGLModInput *in = find(reference_window);
    if (in) in->scroll_delta += static_cast<float>(yoffset);
}

glm::vec2 OpenGLModInput::mousePosition() const { return mouse_pos; }
glm::vec2 OpenGLModInput::mouseDelta() const { return mouse_delta; }
float OpenGLModInput::scrollDelta() const { return scroll_delta; }
