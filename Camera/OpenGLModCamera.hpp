#pragma once

#include "../vendors/glm/glm/glm.hpp"
#include "../vendors/glm/glm/gtc/matrix_transform.hpp"

#include <algorithm>

class OpenGLModCamera
{
    float yaw;
    float pitch;
    glm::vec3 front_vec{0.0f, 0.0f, -1.0f};
    glm::vec3 right_vec{1.0f, 0.0f, 0.0f};
    glm::vec3 up_vec{0.0f, 1.0f, 0.0f};
    glm::vec3 world_up{0.0f, 1.0f, 0.0f};

    void updateVectors();
public:
    enum class Move { Forward, Backward, Left, Right, Up, Down };

    glm::vec3 position;
    float speed = 3.0f;
    float sensitivity = 0.1f;
    float fov = 45.0f;
    float nearplane = 0.1f;
    float farplane = 100.0f;

    explicit OpenGLModCamera(
        glm::vec3 position=glm::vec3(0.0f, 0.0f, 3.0f),
        float yaw=-90.0f,
        float pitch=0.0f
    );

    glm::mat4 viewMatrix() const;
    glm::mat4 projectionMatrix(float aspect_ratio) const;

    void move(Move direction, float delta_time);
    void rotate(float x_offset, float y_offset, bool constrain_pitch=true);
    void zoom(float scroll_offset);

    const glm::vec3 &front() const;
    const glm::vec3 &right() const;
    const glm::vec3 &up() const;
};
