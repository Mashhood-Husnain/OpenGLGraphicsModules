#pragma once

#include "../vendors/glad/include/glad/glad.h"
#include "../vendors/GLFW/include/GLFW/glfw3.h"
#include "../vendors/glm/glm/glm.hpp"
#include "../vendors/glm/glm/gtc/matrix_transform.hpp"

#include <string>
#include <iostream>
#include <functional>

class OpenGLModWindow
{
    GLFWwindow *window_ptr = nullptr;
    glm::vec3 window_bg_color_clear;
    std::string window_title;
    glm::ivec2 window_size;

    bool depth_test = false;

    float last_frame_time = 0.0f;
    float delta_time = 0.0f;

    void glfwWindowHints();
    bool initWindow(std::string title, glm::vec2 size);

    static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    
public:
    OpenGLModWindow(const OpenGLModWindow &) = delete;
    OpenGLModWindow &operator=(const OpenGLModWindow &) = delete;

    OpenGLModWindow(
        std::string window_title="DEFAULT_WINDOW_TITLE_PLACEHOLDER",
        glm::ivec2 window_size=glm::ivec2(800, 600),
        glm::vec3 window_bg_color_clear = glm::vec3(0.1f, 0.1f, 0.1f)
    );
    ~OpenGLModWindow();

    void run(std::function<void()> func = nullptr);

    GLFWwindow *getWindowPtr() const;
    glm::ivec2 getWindowSize() const;
    
    void setWindowBgColorClear(glm::vec3 window_bg_color_clear);
    void setWindowSize(glm::ivec2 window_size);
    void setWindowTitle(std::string window_title);

    void setDepthTest(bool mode=true);
    
    void terminate();

    float getTime() const;
    float getDeltaTime() const;
    glm::ivec2 getFrameBufferSize() const;
    float getAspectRatio() const;
    glm::mat4 getOrthoProjection() const;

};

