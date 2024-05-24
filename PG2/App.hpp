#pragma once

#include <unordered_map>

#include "Model.hpp"
#include "ShaderProgram.hpp"
#include "Camera.hpp"
#include "Audio.hpp"

#define MAP_SCALE 0.4f
#define MAP_MOVE -200.0f
#define N_GLASSES 3

#define PlayerHeight 10.0f //how high is player above terrain
#define Jump_speedUP 15.0f // multiplayer for jump
#define Jump_speedDown 30.0f //multiplayer for falling
#define Jump_time 1 // how long can player jump in seconds

#define Walk_auidio_delay 2 //how often play walk audio in seconds

//chair
#define Chair_speed 20.0f
#define Chair_max_X 80.0f
#define Chair_min_X -110.0f
#define Chair_max_Z 80.0f
#define Chair_min_Z -100.0f
#define ChairHeight 1.0f
#define Change_chair_direction 3

//projectile
#define projectile_speed 25.0f
#define N_PROJECTILES 3
#define PROJECTILE_SCALE 0.3f

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
    static bool is_crosshair_toggled;
    
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

    unsigned int VAO;
    unsigned int VBO;

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
    ShaderProgram crosshair_shader;
    Audio audio;
    //colision
    std::map<std::pair<unsigned int, unsigned int>, unsigned int> heights;
    float GetMapY(float x, float z);
    glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
    double audio_walk_last_time = 0; // last time a walking audio was played
    void PlayerMovement(float deltatime);
    glm::vec3 camera_movement{};
    std::vector<Model*> collisions;

    //chair
    void ChairMovement(float delta_time);
    float Last_chair_direction = 0;
    glm::vec3 chair_direction{};
    void Teleport_chair();

    // projectiles
    Model* projectiles[N_PROJECTILES]{}; // Pool of projectiles
    glm::vec3 projectile_directions[N_PROJECTILES]{};
    bool is_projectile_moving[N_PROJECTILES]{};
    unsigned int projectile_id = 0;
    void ProjectileMovement(float delta_time);
    void Shoot();
    bool IsCollision(glm::vec3 bullet, Model* model);
};