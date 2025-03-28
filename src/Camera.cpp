#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"

void Camera::processMouseMovement(float xOffset, float yOffset)
{
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    this->front = glm::normalize(front);
}

void Camera::processKeyboardInput(int key)
{
    float velocity = speed;
    if (key == GLFW_KEY_W)
        position += velocity * front;
    if (key == GLFW_KEY_S)
        position -= velocity * front;
    if (key == GLFW_KEY_A)
        position -= glm::normalize(glm::cross(front, up)) * velocity;
    if (key == GLFW_KEY_D)
        position += glm::normalize(glm::cross(front, up)) * velocity;
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const
{
    return glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
}
