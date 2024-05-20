#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera
{
public:
    // Camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up; // camera local UP vector

    GLfloat yaw = -90.0f;
    GLfloat pitch = 0.0f;
    GLfloat roll = 0.0f;

    // Camera options
    GLfloat movement_speed_normal = 1.5f;
    GLfloat movement_speed_sprint = 5.5f;
    GLfloat mouse_sensitivity = 0.15f;

    Camera(glm::vec3 position);
    glm::mat4 GetViewMatrix();
    glm::vec3 ProcessInput(GLFWwindow* window, GLfloat deltaTime);
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset);

    void ToggleSprint();
private:
    //glm::vec3 world_up;

    bool is_sprint_toggled;

    void UpdateCameraVectors();
};
