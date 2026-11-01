#pragma once

#include "../vendors/glad/include/glad/glad.h"
#include "../vendors/GLFW/include/GLFW/glfw3.h"
#include "../vendors/glm/glm/glm.hpp"
#include "../vendors/glm/glm/gtc/matrix_transform.hpp"

#include <string>
#include <iostream>
#include <functional>
#include <memory>
#include <vector>
#include <iterator>
#include <cstdlib>

class OpenGLModShader;

class OpenGLModWindow
{
    GLFWwindow *window_ptr = nullptr;
    glm::vec3 window_bg_color_clear;
    std::string window_title;
    glm::ivec2 window_size;

    bool depth_test = false;

    float last_frame_time = 0.0f;
    float delta_time = 0.0f;

    bool show_fps = false;
    int fps_frames = 0;
    int fps_value = 0;
    double fps_timer_start = 0.0;

    std::unique_ptr<OpenGLModShader> fps_shader;
    unsigned int fps_vao = 0;
    unsigned int fps_vbo = 0;

    void initFPSOverlay();
    void drawFPS();
    void releaseFPS();

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
    void setBlending(bool mode=true);
    
    void showFPS(bool show=true);

    void terminate();

    float getTime() const;
    float getDeltaTime() const;
    glm::ivec2 getFrameBufferSize() const;
    float getAspectRatio() const;
    glm::mat4 getOrthoProjection() const;

    glm::mat4 getPixelProjection() const;

};

