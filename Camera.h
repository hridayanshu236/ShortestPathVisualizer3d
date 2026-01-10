#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    // Camera attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Euler angles
    float Yaw;
    float Pitch;

    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // Constructor
    Camera(glm::vec3 position = glm::vec3(0.0f, 8.0f, 15.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f,
        float pitch = -30.0f);

    // Get view matrix
    glm::mat4 GetViewMatrix();

    // Process keyboard input
    void ProcessKeyboard(int direction, float deltaTime);

    // Process mouse movement
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    // Process mouse scroll
    void ProcessMouseScroll(float yoffset);

private:
    // Update camera vectors
    void updateCameraVectors();
};

// Camera movement directions
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

#endif