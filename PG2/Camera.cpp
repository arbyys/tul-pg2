#include <iostream>

#include <glm/ext/matrix_transform.hpp>

#include "Camera.hpp"

#define print(x) std::cout << x << "\n"

// pseudo MC spectator mod, po dokonèeí odstranit všechny výskyty:
#define DEBUG_FLY false

Camera::Camera(glm::vec3 position) : position(position)
{
    this->world_up = glm::vec3(0.0f, 1.0f, 0.0f);
    this->world_down = glm::vec3(0.0f, 0.0f, 0.0f);
    // initialization of the camera reference system
    this->UpdateCameraVectors();

    is_sprint_toggled = false;
    jumping = false;
    falling = false;
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(this->position, this->position + this->front, this->up);
}

glm::vec3 Camera::ProcessInput(GLFWwindow* window, GLfloat delta_time)
{
    glm::vec3 direction(0,0,0);
    glm::vec3 zero(0,0,0);

    glm::vec3 horizont_front;
    glm::vec3 horizont_right;

    if (DEBUG_FLY) {
        horizont_front = front;
        horizont_right = right;
    } else {
        horizont_front = glm::vec3(front.x, 0, front.z);
        horizont_right = glm::vec3(right.x, 0, right.z);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        direction += horizont_front;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        direction += -horizont_front;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        direction += -horizont_right;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        direction += horizont_right;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !jumping && !falling) {
        jumping = true;
        last_jump_time = glfwGetTime();
    }


    if (direction == zero) {
        is_sprint_toggled = false;
    }


    float movement_speed;
    if (DEBUG_FLY) {
        movement_speed = (is_sprint_toggled) ? movement_speed_sprint*3 : movement_speed_normal*10;
    } else {
        movement_speed = (is_sprint_toggled) ? movement_speed_sprint : movement_speed_normal;
    }

    return direction == zero ? zero : glm::normalize(direction) * movement_speed * delta_time;
}

void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
{
    xoffset *= this->mouse_sensitivity;
    yoffset *= this->mouse_sensitivity;

    this->yaw += xoffset;
    //this->pitch += yoffset;
    this->pitch -= yoffset;

    // Constraint Pitch
    if (this->pitch > 89.0f) this->pitch = 89.0f;
    if (this->pitch < -89.0f) this->pitch = -89.0f;

    this->UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    front.y = sin(glm::radians(this->pitch));
    front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

    this->front = glm::normalize(front);
    this->right = glm::normalize(glm::cross(this->front, glm::vec3(0.0f, 1.0f, 0.0f)));
    this->up = glm::normalize(glm::cross(this->right, this->front));
}

void Camera::ToggleSprint()
{
    is_sprint_toggled = !is_sprint_toggled;
}

void Camera::OnLand()
{
    jumping = false;
    falling = false;
}

void Camera::StopJump() {
    jumping = false;
    falling = true;
}

bool Camera::IsJumping()
{
    return jumping;
}
bool Camera::Isfalling()
{
    return falling;
}
bool Camera::IsSprinting() {
    return is_sprint_toggled;
}
