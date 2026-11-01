#include "OpenGLModCamera.hpp"

OpenGLModCamera::OpenGLModCamera(glm::vec3 position, float yaw, float pitch)
{
    this->yaw = yaw;
    this->pitch = pitch;
    this->position = position;

    updateVectors();
}

glm::mat4 OpenGLModCamera::viewMatrix() const
{
    return glm::lookAt(position, position + front_vec, up_vec);
}

glm::mat4 OpenGLModCamera::projectionMatrix(float aspect_ratio) const
{
    return glm::perspective(glm::radians(fov), aspect_ratio, nearplane, farplane);
}

void OpenGLModCamera::move(Move direction, float delta_time)
{
    const float distance = speed * delta_time;
    switch (direction)
    {
        case Move::Forward: position += front_vec * distance; break;
        case Move::Backward: position -= front_vec * distance; break;
        case Move::Left: position -= right_vec * distance; break;
        case Move::Right: position += right_vec * distance; break;
        case Move::Up: position += world_up * distance;  break;
        case Move::Down: position -= world_up * distance;  break;
    }
}

void OpenGLModCamera::rotate(float x_offset, float y_offset, bool constrain_pitch)
{
    yaw   += x_offset * sensitivity;
    pitch += y_offset * sensitivity;

    if (constrain_pitch) pitch = std::clamp(pitch, -89.0f, 89.0f);

    updateVectors();
}

void OpenGLModCamera::zoom(float scroll_offset)
{
    fov = std::clamp(fov - scroll_offset * 2.0f, 1.0f, 90.0f);
}

void OpenGLModCamera::updateVectors()
{
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    front_vec = glm::normalize(f);
    right_vec = glm::normalize(glm::cross(front_vec, world_up));
    up_vec    = glm::normalize(glm::cross(right_vec, front_vec));
}
