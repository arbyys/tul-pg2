#pragma once

#include <unordered_map>

#include "Model.hpp"
#include "ShaderProgram.hpp"
#include "Camera.hpp"
#include "Audio.hpp"

#define MAP_SCALE 0.4f
#define MAP_MOVE -200.0f
#define N_PROJECTILES 10

class App {
public:
    App();

    bool Init();
    void InitAssets();
    int Run();

    ~App();
private:
    std::map<std::string, Model*> scene_non_transparent;

    static bool is_vsync_on;
    static bool is_fullscreen_on;
    
    static GLFWmonitor* monitor;
    static const GLFWvidmode* mode;
    static int window_xcor;
    static int window_ycor;
    static int window_width;
    static int window_height;
    static int window_width_return_from_fullscreen;
    static int window_height_return_from_fullscreen;

    float FOV = 89.0f;
    glm::mat4 mx_projection = glm::identity<glm::mat4>();
    static Camera camera;
    static double last_cursor_xpos;
    static double last_cursor_ypos;

    GLFWwindow* window = nullptr;
    glm::vec4 clear_color = glm::vec4(0, 0, 0, 0);

    void UpdateProjectionMatrix();

    void GetInformation();

    static void error_callback(int error, const char* description);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    ShaderProgram my_shader;
    Audio audio;
    //colision
    std::map<std::pair<unsigned int, unsigned int>, unsigned int> heights;

    // projectiles
    const float projectile_speed = 20.0f;
    int projectile_n = 0;                   // Currently used projectile
    Model* projectiles[N_PROJECTILES]{};    // Pool of projectiles
    glm::vec3 projectile_directions[N_PROJECTILES]{};
    bool is_projectile_moving[N_PROJECTILES]{};
    void Shoot();
    void UpdateProjectiles(float delta_time);
};