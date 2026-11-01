#include "OpenGLModWindow.hpp"

OpenGLModWindow::OpenGLModWindow(std::string window_title, glm::ivec2 window_size, glm::vec3 window_bg_color_clear)
{
    this->window_title = window_title;
    this->window_size = window_size;
    this->window_bg_color_clear = window_bg_color_clear;

    if (!glfwInit())
    {
        std::cout << "Could not initialize GFLW!" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHints();

    if (!initWindow(window_title, window_size))
    {
        std::cerr << "Could not initialize window!" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Could not initialze GLAD!" << std::endl;
        glfwDestroyWindow(window_ptr);
        window_ptr = nullptr;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glViewport(0, 0, window_size.x, window_size.y);
    glfwSetWindowUserPointer(window_ptr, this);
    glfwSetFramebufferSizeCallback(window_ptr, framebuffer_size_callback);

    glClearColor(window_bg_color_clear.x, window_bg_color_clear.y, window_bg_color_clear.z, 1.0f);
}

OpenGLModWindow::~OpenGLModWindow()
{
    glfwDestroyWindow(window_ptr);
    window_ptr = nullptr;

    glfwTerminate();
}

void OpenGLModWindow::glfwWindowHints()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

bool OpenGLModWindow::initWindow(std::string title, glm::vec2 size)
{
    window_ptr = glfwCreateWindow(size.x, size.y, title.c_str(), NULL, NULL);
    if (!window_ptr) return false;

    glfwMakeContextCurrent(window_ptr);
    return true;
}

void OpenGLModWindow::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    auto *self = static_cast<OpenGLModWindow *>(glfwGetWindowUserPointer(window));
    if (self) self->window_size = glm::vec2(width, height);

    glViewport(0, 0, width, height);
}

void OpenGLModWindow::run(std::function<void()> func)
{
    while(!glfwWindowShouldClose(window_ptr))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        if (depth_test) glClear(GL_DEPTH_BUFFER_BIT);

        if (func) func();

        glfwSwapBuffers(window_ptr);
        glfwPollEvents();
    }
}

GLFWwindow *OpenGLModWindow::getWindowPtr() const { return window_ptr; }
glm::ivec2 OpenGLModWindow::getWindowSize() const { return window_size; }

void OpenGLModWindow::setWindowBgColorClear(glm::vec3 window_bg_color_clear)
{
    this->window_bg_color_clear = window_bg_color_clear;
    glClearColor(window_bg_color_clear.x, window_bg_color_clear.y, window_bg_color_clear.z, 1.0f);
}

void OpenGLModWindow::setWindowSize(glm::ivec2 window_size)
{
    this->window_size = window_size;
    glfwSetWindowSize(window_ptr, window_size.x, window_size.y);
}

void OpenGLModWindow::setWindowTitle(std::string window_title)
{
    this->window_title = window_title;
    glfwSetWindowTitle(window_ptr, window_title.c_str());
}

void OpenGLModWindow::setDepthTest(bool mode)
{
    depth_test = mode;

    if (mode)
    {
        glEnable(GL_DEPTH_TEST);
        return;
    }
    
    glDisable(GL_DEPTH_TEST);
}

void OpenGLModWindow::setBlending(bool mode)
{
    if (mode)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        return;
    }

    glDisable(GL_BLEND);
}

void OpenGLModWindow::terminate()
{
    glfwSetWindowShouldClose(window_ptr, true);
}

float OpenGLModWindow::getTime() const
{
    return static_cast<float>(glfwGetTime());
}

float OpenGLModWindow::getDeltaTime() const
{
    return delta_time;
}

glm::ivec2 OpenGLModWindow::getFrameBufferSize() const
{
    int w = 0, h = 0;
    glfwGetFramebufferSize(window_ptr, &w, &h);
    return glm::ivec2(w, h);
}

float OpenGLModWindow::getAspectRatio() const
{
    glm::ivec2 size = getFrameBufferSize();

    if (size.y == 0) return 1.0f;

    return static_cast<float>(size.x) / static_cast<float>(size.y);
}

glm::mat4 OpenGLModWindow::getOrthoProjection() const
{
    float aspect = getAspectRatio();

    return glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);

}
