#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Audio.hpp"

class Camera
{
public:
    // Camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up; // camera local UP vector

    GLfloat yaw = 0.0f;
    GLfloat pitch = 0.0f;
    GLfloat roll = 0.0f;

    // Camera options
    GLfloat movement_speed_normal = 25.0f;
    GLfloat movement_speed_sprint = 45.0f;
    GLfloat mouse_sensitivity = 0.1f;

    Camera(glm::vec3 position);
    glm::mat4 GetViewMatrix();
    glm::vec3 ProcessInput(GLFWwindow* window, GLfloat deltaTime, Audio& audio);
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset);

    void ToggleSprint();
private:
    glm::vec3 world_up;
    glm::vec3 world_down;

    bool is_sprint_toggled;
    bool jumping;
    bool falling;

    float gravity = 25.0f;
    float jumpStrength = 2.0f;
    float jumpHeight = 12.5f;
    int momentum = 1;

    void UpdateCameraVectors();
};
