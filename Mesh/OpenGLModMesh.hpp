#pragma once

#include "../vendors/glad/include/glad/glad.h"
#include "../vendors/glm/glm/glm.hpp"
#include "../vendors/glm/glm/gtc/matrix_transform.hpp"

#include <vector>
#include <utility>
#include <cmath>
#include <numbers>

class OpenGLModMesh
{
    unsigned int vao = 0;
    unsigned int vbo = 0;
    unsigned int ebo = 0;

    int vertex_count = 0;
    int index_count = 0;

    void release();
public:
    glm::vec3 position{0.0f};
    glm::vec3 scale{1.0f};
    float rotation = 0.0f;
    glm::vec3 tint{1.0f};

    OpenGLModMesh(const OpenGLModMesh&) = delete;
    OpenGLModMesh& operator=(const OpenGLModMesh&) = delete;
    OpenGLModMesh(OpenGLModMesh&& other) noexcept;
    OpenGLModMesh& operator=(OpenGLModMesh&& other) noexcept;

    OpenGLModMesh(const std::vector<float> &vertices, const std::vector<int> &attributes, const std::vector<unsigned int> &indices={});
    ~OpenGLModMesh();

    void draw(unsigned int mode=GL_TRIANGLES) const;

    static OpenGLModMesh circle(float radius, int segments=32, glm::vec3 color=glm::vec3(1.0f), bool filled=true);
    static OpenGLModMesh ellipse(glm::vec2 radius, int segments=32, glm::vec3 color=glm::vec3(1.0f), bool filled=true);

    glm::mat4 modelMatrix() const;

};
