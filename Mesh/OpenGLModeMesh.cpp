#include "OpenGLModMesh.hpp"

OpenGLModMesh::OpenGLModMesh(
    const std::vector<float> &vertices,
    const std::vector<int> &attributes,
    const std::vector<unsigned int> &indices)
{
    int stride_floats = 0;
    for (int a : attributes)
    {
        stride_floats += a;
    }

    vertex_count = static_cast<int>(vertices.size()) / stride_floats;
    index_count = static_cast<int>(indices.size());

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    if (!indices.empty())
    {
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }

    int offset = 0;
    for (size_t i = 0; i < attributes.size(); i++)
    {
        glVertexAttribPointer((GLuint)i, attributes[i], GL_FLOAT, GL_FALSE, stride_floats * sizeof(float), (void*)(static_cast<size_t>(offset) * sizeof(float)));
        glEnableVertexAttribArray((GLuint)i);
        offset += attributes[i];
    }

    glBindVertexArray(0);
}

OpenGLModMesh::OpenGLModMesh(OpenGLModMesh&& other) noexcept :
    vao(std::exchange(other.vao, 0)),
    vbo(std::exchange(other.vbo, 0)),
    ebo(std::exchange(other.ebo, 0)),
    vertex_count(other.vertex_count),
    index_count(other.index_count) {}

OpenGLModMesh& OpenGLModMesh::operator=(OpenGLModMesh&& other) noexcept
{
    if (this != &other)
    {
        release();
        vao = std::exchange(other.vao, 0);
        vbo = std::exchange(other.vbo, 0);
        ebo = std::exchange(other.ebo, 0);

        vertex_count = other.vertex_count;
        index_count  = other.index_count;
    }
    return *this;
}

OpenGLModMesh::~OpenGLModMesh()
{
    release();
}

void OpenGLModMesh::release()
{
    if (ebo) glDeleteBuffers(1, &ebo);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);

    vao = vbo = ebo = 0;
}

void OpenGLModMesh::draw(unsigned int mode) const
{
    glBindVertexArray(vao);
    if (index_count > 0)
    {
        glDrawElements(mode, index_count, GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawArrays(mode, 0, vertex_count);
    }
    glBindVertexArray(0);
}

OpenGLModMesh OpenGLModMesh::circle(float radius, int segments, glm::vec3 color, bool filled)
{
    return ellipse(glm::vec2(radius, radius), segments, color, filled);
}

OpenGLModMesh OpenGLModMesh::ellipse(glm::vec2 radius, int segments, glm::vec3 color, bool filled)
{
    segments = std::max(segments, 3);

    const float two_pi = 2 * std::numbers::pi_v<float>;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    auto push = [&](float x, float y)
    {
        vertices.insert(vertices.end(), {x, y, 0.0f, color.r, color.g, color.b});
    };

    if (filled)
    {
        push(0.0f, 0.0f);

        for (int i = 0; i < segments; i++)
        {
            float angle = two_pi * static_cast<float>(i) / static_cast<float>(segments);
            push(radius.x * std::cos(angle), radius.y * std::sin(angle));
        }

        for (int i = 0; i < segments; i++)
        {
            indices.push_back(0);
            indices.push_back(static_cast<unsigned int>(1 + i));
            indices.push_back(static_cast<unsigned int>(1 + (1 + i)) % segments);
        }
    }
    else
    {
        for(int i = 0; i < segments; i++)
        {
            float angle = two_pi * static_cast<float>(i) / static_cast<float>(segments);
            push(radius.x * std::cos(angle), radius.y * std::sin(angle));
        }
    }

    return OpenGLModMesh(vertices, {3, 3}, indices);
}
