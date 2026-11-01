#pragma once

#include "../vendors/glad/include/glad/glad.h"

#include <vector>
#include <utility>

class OpenGLModMesh
{
    unsigned int vao = 0;
    unsigned int vbo = 0;
    unsigned int ebo = 0;

    int vertex_count = 0;
    int index_count = 0;

    void release();
public:
    OpenGLModMesh(const OpenGLModMesh&) = delete;
    OpenGLModMesh& operator=(const OpenGLModMesh&) = delete;
    OpenGLModMesh(OpenGLModMesh&& other) noexcept;
    OpenGLModMesh& operator=(OpenGLModMesh&& other) noexcept;

    OpenGLModMesh(const std::vector<float> &vertices, const std::vector<int> &attributes, const std::vector<unsigned int> &indices={});
    ~OpenGLModMesh();

    void draw(unsigned int mode=GL_TRIANGLES) const;
};
