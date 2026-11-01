#pragma once

#include "../vendors/glad/include/glad/glad.h"
#include "../vendors/GLFW/include/GLFW/glfw3.h"
#include "../vendors/glm/glm/glm.hpp"
#include "../vendors/glm/glm/gtc/type_ptr.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <utility>


class OpenGLModShader
{
    unsigned int shader_program = 0;
    std::unordered_map<std::string, int> uniform_cache;

    static bool checkError(unsigned int shader, std::string error_type, std::string type);
    static bool readFile(const std::string &path, std::string &out);
    static unsigned int compile(unsigned int type, const std::string &source, std::string label);
    int location(const std::string &name);

public:
    OpenGLModShader(const OpenGLModShader &) = delete;
    OpenGLModShader &operator=(const OpenGLModShader &) = delete;
    OpenGLModShader(OpenGLModShader &&other) noexcept;
    OpenGLModShader &operator=(OpenGLModShader &&other) noexcept;

    OpenGLModShader(const std::string &vs_path, const std::string &fs_path);
    ~OpenGLModShader();

    bool isValid() const;
    unsigned int id() const;
    void use() const;

    void setBool(const std::string &name, bool value);
    void setInt(const std::string &name, int value);
    void setFloat(const std::string &name, float value);
    void setVec2(const std::string &name, const glm::vec2 &v);
    void setVec3(const std::string &name, const glm::vec3 &v);
    void setVec4(const std::string &name, const glm::vec4 &v);
    void setMat3(const std::string &name, const glm::mat3 &m);
    void setMat4(const std::string &name, const glm::mat4 &m);

    template<typename Drawable>
    void draw(const Drawable &obj)
    {
        use();
        setMat4("uModel", obj.modelMatrix());
        setVec3("uTint", obj.tint);

        obj.draw();
    }
};
