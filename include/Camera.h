#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    float yaw;
    float pitch;
    bool firstMouse;

    float speed;
    float sensitivity;
    float fov;

public:
    Camera(glm::vec3 startPosition = glm::vec3(0.0f, 0.0f, 3.0f),
           glm::vec3 startFront = glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec3 startUp = glm::vec3(0.0f, 1.0f, 0.0f),
           float startYaw = -90.0f, float startPitch = 0.0f, float startSpeed = 0.05f,
           float startSensitivity = 0.1f, float startFov = 45.0f, bool firstMouse = false)
        : position(startPosition), front(startFront), up(startUp), yaw(startYaw), pitch(startPitch),
          speed(startSpeed), sensitivity(startSensitivity), fov(startFov), firstMouse(firstMouse) {}

    float getSpeed() const { return speed; }
    void setSpeed(float newSpeed) { speed = newSpeed; }

    float getSensitivity() const { return sensitivity; }
    void setSensitivity(float newSensitivity) { sensitivity = newSensitivity; }

    float getFov() const { return fov; }
    void setFov(float newFov) { fov = newFov; }

    glm::vec3 getPosition() const { return position; }
    glm::vec3 getFront() const { return front; }
    glm::vec3 getUp() const { return up; }

    float getYaw() const { return yaw; }
    float getPitch() const { return pitch; }

    void processMouseMovement(float xOffset, float yOffset);
    void processKeyboardInput(int key);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;
};
