#include "OpenGLModWindow.hpp"
#include "../Shader/OpenGLModShader.hpp"

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
    releaseFPS();

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
    last_frame_time = getTime();

    while(!glfwWindowShouldClose(window_ptr))
    {
        float now = getTime();
        delta_time = now - last_frame_time;
        last_frame_time = now;

        GLbitfield mask = GL_COLOR_BUFFER_BIT;
        if (depth_test) mask |= GL_DEPTH_BUFFER_BIT;
        glClear(mask);

        if (func) func();

        drawFPS();

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

glm::mat4 OpenGLModWindow::getPixelProjection() const
{
    glm::ivec2 size = getFrameBufferSize();
    return glm::ortho(0.0f, static_cast<float>(size.x), static_cast<float>(size.y), 0.0f, -1.0f, 1.0f);
}

static const char *kFpsVert = R"(#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
uniform vec2 uScreen;
out vec3 vColor;
void main()
{
    vec2 ndc = vec2(aPos.x / uScreen.x * 2.0 - 1.0, 1.0 - aPos.y / uScreen.y * 2.0);
    gl_Position = vec4(ndc, 0.0, 1.0);
    vColor = aColor;
})";

static const char *kFpsFrag = R"(#version 330 core
in vec3 vColor;
out vec4 FragColor;
void main()
{
    FragColor = vec4(vColor, 1.0);
})";

void OpenGLModWindow::showFPS(bool show)
{
    show_fps = show;
    fps_frames = 0;
    fps_timer_start = glfwGetTime();
}

void OpenGLModWindow::initFPSOverlay()
{
    fps_shader = std::make_unique<OpenGLModShader>(OpenGLModShader::fromSource(kFpsVert, kFpsFrag));

    if (!fps_shader->isValid())
    {
        fps_shader.reset();
        show_fps = false;
        return;
    }

    glGenVertexArrays(1, &fps_vao);
    glGenBuffers(1, &fps_vbo);
    glBindVertexArray(fps_vao);
    glBindBuffer(GL_ARRAY_BUFFER, fps_vbo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void OpenGLModWindow::releaseFPS()
{
    if (fps_vbo) glDeleteBuffers(1, &fps_vbo);
    if (fps_vao) glDeleteVertexArrays(1, &fps_vao);
    fps_vbo = fps_vao = 0;

    fps_shader.reset();
}

void OpenGLModWindow::drawFPS()
{
    if (!show_fps) return;
    if (!fps_shader) initFPSOverlay();
    if (!fps_shader) return;

    fps_frames++;
    double now = glfwGetTime();
    double elapsed = now - fps_timer_start;
    if (elapsed >= 0.5)
    {
        fps_value = static_cast<int>(fps_frames / elapsed + 0.5);
        fps_frames = 0;
        fps_timer_start = now;
    }

    static const unsigned char digit_segments[10] = {
        0b0111111,  // 0: a b c d e f
        0b0000110,  // 1: b c
        0b1011011,  // 2: a b d e g
        0b1001111,  // 3: a b c d g
        0b1100110,  // 4: b c f g
        0b1101101,  // 5: a c d f g
        0b1111101,  // 6: a c d e f g
        0b0000111,  // 7: a b c
        0b1111111,  // 8: all
        0b1101111   // 9: a b c d f g
    };

    std::vector<float> v;
    auto rect = [&](float x, float y, float w, float h, float r, float g, float b)
    {
        float quad[] = {
            x, y, r, g, b,
            x + w, y, r, g, b,
            x + w, y + h, r, g, b,
            x, y, r, g, b,
            x + w, y + h, r, g, b,
            x, y + h, r, g, b };
        v.insert(v.end(), std::begin(quad), std::end(quad));
    };

    const float margin = 10.0f, pad = 6.0f;
    const float dw = 16.0f, dh = 28.0f, t = 4.0f, gap = 6.0f;

    std::string text = std::to_string(fps_value);
    float count = static_cast<float>(text.size());
    float box_w = pad * 2 + count * dw + (count - 1) * gap;
    float box_h = pad * 2 + dh;
    rect(margin, margin, box_w, box_h, 0.08f, 0.08f, 0.08f);

    float x = margin + pad;
    float y = margin + pad;
    float mid = y + (dh - t) / 2.0f;
    for (char ch : text)
    {
        unsigned char s = digit_segments[ch - '0'];
        if (s & (1 << 0)) rect(x + t, y, dw - 2 * t, t, 1, 1, 1);  // a
        if (s & (1 << 1)) rect(x + dw - t, y + t, t, mid - y, 1, 1, 1);  // b
        if (s & (1 << 2)) rect(x + dw - t, mid, t, y + dh - t - mid, 1, 1, 1);  // c
        if (s & (1 << 3)) rect(x + t, y + dh - t, dw - 2 * t, t, 1, 1, 1);  // d
        if (s & (1 << 4)) rect(x, mid, t, y + dh - t - mid, 1, 1, 1);  // e
        if (s & (1 << 5)) rect(x, y + t, t, mid - y, 1, 1, 1);  // f
        if (s & (1 << 6)) rect(x + t, mid, dw - 2 * t, t, 1, 1, 1);  // g
        x += dw + gap;
    }

    glm::ivec2 size = getFrameBufferSize();

    GLboolean depth_was_on = glIsEnabled(GL_DEPTH_TEST);
    GLboolean blend_was_on = glIsEnabled(GL_BLEND);
    GLint previous_program = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &previous_program);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    fps_shader->use();
    fps_shader->setVec2("uScreen", glm::vec2(static_cast<float>(size.x), static_cast<float>(size.y)));

    glBindVertexArray(fps_vao);
    glBindBuffer(GL_ARRAY_BUFFER, fps_vbo);
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(v.size() / 5));
    glBindVertexArray(0);

    glUseProgram(static_cast<unsigned int>(previous_program));
    if (depth_was_on) glEnable(GL_DEPTH_TEST);
    if (blend_was_on) glEnable(GL_BLEND);
}
