// Basic includes
#include <iostream>
#include <chrono>
#include <stack>
#include <random>
#include <sstream>

// OpenGL Extension Wrangler: allow all multiplatform GL functions
#include <GL/glew.h> 
// WGLEW = Windows GL Extension Wrangler (change for different platform) 
// platform specific functions (in this case Windows)
#include <GL/wglew.h> 

// GLFW toolkit
// Uses GL calls to open GL context, i.e. GLEW must be first.
#include <GLFW/glfw3.h>

// OpenGL math
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Our app
#include "App.hpp"
#include "gl_err_callback.hpp"
#include "ShaderProgram.hpp"

#define print(x) std::cout << x << "\n"

bool App::is_vsync_on = false;
bool App::is_fullscreen_on = false;
bool App::is_crosshair_toggled = true;
GLFWmonitor* App::monitor;
const GLFWvidmode* App::mode;
int App::window_xcor{};
int App::window_ycor{};
int App::window_width = 800;
int App::window_height = 600;
int App::window_width_return_from_fullscreen{};
int App::window_height_return_from_fullscreen{};

Camera App::camera = Camera(glm::vec3(0, 0, 1000));
double App::last_cursor_xpos{};
double App::last_cursor_ypos{};

App::App()
{
    // default constructor
    // nothing to do here (for now...)
    std::cout << "Constructed...\n";
}

void App::InitAssets()
{
    glm::vec3 position{};
    float scale{};
    glm::vec4 rotation{};

    // load models, load textures, load shaders, initialize level, etc...
    std::filesystem::path VS_path("./resources/shaders/shader.vert");
    std::filesystem::path FS_path("./resources/shaders/shader.frag");
    my_shader = ShaderProgram(VS_path, FS_path);

    std::filesystem::path crosshair_VS_path("./resources/shaders/crosshair.vert");
    std::filesystem::path crosshair_FS_path("./resources/shaders/crosshair.frag");
    crosshair_shader = ShaderProgram(crosshair_VS_path, crosshair_FS_path);

    //load models
    std::filesystem::path chair_model("./resources/objects/chair.obj");
    std::filesystem::path chair_texture("./resources/textures/chair.jpg");

    auto chair = new Model(chair_model, chair_texture, glm::vec3(4.0f, 1.0f, 8.0f), 0.08f, glm::vec4(1.0f, 0.0f, 0.0f, -90.0f));
    scene_non_transparent.insert(std::make_pair("chair", chair));

    std::filesystem::path table_model("./resources/objects/table.obj");
    std::filesystem::path table_texture("./resources/textures/table.png");

    auto table = new Model(table_model, table_texture, glm::vec3(4.0f, 1.0f, 8.0f), 3.0f, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
    scene_non_transparent.insert(std::make_pair("table", table));

    auto map = Model::CreateTerrain(glm::vec3(MAP_MOVE, 0.0f, MAP_MOVE), MAP_SCALE, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),&heights);
    scene_non_transparent.insert(std::make_pair("map", map));


    // projectiles

    /*
    std::filesystem::path model_path2("./resources/objects/sphere.obj");
    print("Loading projectiles:");
    position = glm::vec3(0.0f, -10.0f, 0.0f); // Hidden
    scale = 0.05f;
    rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    for (int i = 0; i < N_PROJECTILES; i++) {
        auto name = "obj_projectile_" + std::to_string(i);
        auto obj_projectile_x = new Model(model_path2);
        projectiles[i] = obj_projectile_x;
    }
    */
}

// App initialization, if returns true then run run()
bool App::Init()
{
    try {
        // Set GLFW error callback
        glfwSetErrorCallback(error_callback);

        // Init GLFW :: https://www.glfw.org/documentation.html
        if (!glfwInit()) {
            return false;
        }

        // Set OpenGL version
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        // Set OpenGL profile
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core, comment this line for Compatible

        // Open window (GL canvas) with no special properties :: https://www.glfw.org/docs/latest/quick.html#quick_create_window
        window = glfwCreateWindow(window_width, window_height, "Moje krasne okno", NULL, NULL);
        if (!window) {
            glfwTerminate();
            return false;
        }
        glfwSetWindowUserPointer(window, this);

        // Fullscreen On/Off
        monitor = glfwGetPrimaryMonitor(); // Get primary monitor
        mode = glfwGetVideoMode(monitor); // Get resolution of the monitor

        // Setup callbacks
        glfwMakeContextCurrent(window);
        glfwSetKeyCallback(window, key_callback);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetScrollCallback(window, scroll_callback);                     // Mousewheel


        // Set V-Sync ON.
        glfwSwapInterval(1);
        is_vsync_on = true;

        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Init GLEW :: http://glew.sourceforge.net/basic.html
        GLenum err = glewInit();
        if (GLEW_OK != err) {
            fprintf(stderr, "Error: %s\n", glewGetErrorString(err)); /* Problem: glewInit failed, something is seriously wrong. */
        }
        wglewInit();

        //...after ALL GLFW & GLEW init ...
        if (GLEW_ARB_debug_output)
        {
            glDebugMessageCallback(MessageCallback, 0);
            glEnable(GL_DEBUG_OUTPUT);

            //default is asynchronous debug output, use this to simulate glGetError() functionality
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

            std::cout << "GL_DEBUG enabled.\n";
        }
        else {
            std::cout << "GL_DEBUG NOT SUPPORTED!\n";
        }

        // set GL params
        /*
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        /**/
        glEnable(GL_DEPTH_TEST);
        // first init OpenGL, THAN init assets: valid context MUST exist
        InitAssets();
    }
    catch (std::exception const& e) {
        std::cerr << "Init failed : " << e.what() << "\n";
        //throw;
        exit(-1);
    }
    std::cout << "Initialized...\n";
    return true;
}

int App::Run(void)
{
    try {
        double fps_counter_seconds = 0;
        int fps_counter_frames = 0;

        // init crosshair
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        float crosshairVertices[] = {
            -0.013f,  0.0f,  0.0f,
             0.013f,  0.0f,  0.0f,
              0.0f, -0.013f,  0.0f,
              0.0f,  0.013f,  0.0f,
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        UpdateProjectionMatrix(); //updates mx_projection based on window size
        glViewport(0, 0, window_width, window_height);
        camera.position = glm::vec3(1.0f, 7.0f, 1.0f);
        double last_frame_time = glfwGetTime();
        glm::vec3 camera_movement{};

        //glm::vec4 my_rgba = { 0.28f, 0.94f, 0.12f, 1.0f };

        audio.PlayBgMusic();

        while (!glfwWindowShouldClose(window))
        {
            // Time/FPS measure start
            auto fps_frame_start_timestamp = std::chrono::steady_clock::now();

            // Clear OpenGL canvas, both color buffer and Z-buffer
            glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // After clearing the canvas:

            // React to user ;; Create View Matrix according to camera settings
            double delta_time = glfwGetTime() - last_frame_time;
            last_frame_time = glfwGetTime();
            camera_movement = camera.ProcessInput(window, static_cast<float>(delta_time), audio);
            camera.position += camera_movement;
            
            glm::mat4 mx_view = camera.GetViewMatrix();

            // Set Model Matrix
            glm::mat4 mx_model = glm::identity<glm::mat4>();

            // Activate shader, set uniform vars
            my_shader.Activate();

            my_shader.SetUniform("uMxProjection", mx_projection);
            //my_shader.SetUniform("uMxModel", mx_model);
            my_shader.SetUniform("uMxView", mx_view);

            my_shader.SetUniform("uMaterial.ambient", glm::vec3(0.9f)); // change val
            //my_shader.SetUniform("uMaterial.specular", glm::vec3(1.0f)); // change val
            //my_shader.SetUniform("uMaterial.shininess", 96.0f); // change val, other lights as well

            // point light todo

            // spot light todo

            // dir light todo


            // Draw non transparent scene
            for (auto& [key, value] : scene_non_transparent) {
                value->Draw(my_shader);
            }

            // draw crosshair

            if (is_crosshair_toggled) {
                crosshair_shader.Activate();
                glBindVertexArray(VAO);
                glLineWidth(2.0f);
                glDrawArrays(GL_LINES, 0, 4);
                glBindVertexArray(0);
            }


            // End of frame
            // Swap front and back buffers
            glfwSwapBuffers(window);

            // Poll for and process events
            glfwPollEvents();
            
            // Time/FPS measure end
            auto fps_frame_end_timestamp = std::chrono::steady_clock::now();
            std::chrono::duration<double> fps_elapsed_seconds = fps_frame_end_timestamp - fps_frame_start_timestamp;
            fps_counter_seconds += fps_elapsed_seconds.count();
            fps_counter_frames++;
            if (fps_counter_seconds >= 1) {
                //std::cout << fps_counter_frames << " FPS\n";
                std::stringstream ss;
                ss << fps_counter_frames << " FPS";
                glfwSetWindowTitle(window, ss.str().c_str());
                fps_counter_seconds = 0;
                fps_counter_frames = 0;
            }
        }
    }
    catch (std::exception const& e) {
        std::cerr << "App failed : " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    GetInformation();
    std::cout << "Finished OK...\n";
    return EXIT_SUCCESS;
}

App::~App()
{
    // clean-up
    my_shader.Clear();

    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();

    exit(EXIT_SUCCESS);
    std::cout << "Bye...\n";
}

//updates mx_projection based on window size
void App::UpdateProjectionMatrix(void)
{
    if (window_height < 1) window_height = 1; // avoid division by 0

    float ratio = static_cast<float>(window_width) / window_height;

    mx_projection = glm::perspective(
        glm::radians(FOV),   // The vertical Field of View
        ratio,               // Aspect Ratio. Depends on the size of your window.
        0.1f,                // Near clipping plane. Keep as big as possible, or you'll get precision issues.
        20000.0f             // Far clipping plane. Keep as little as possible.
    );
}

void App::GetInformation() {
    std::cout << "\n============= :: GL Info :: =============\n";
    std::cout << "GL Vendor:\t" << glGetString(GL_VENDOR) << "\n";
    std::cout << "GL Renderer:\t" << glGetString(GL_RENDERER) << "\n";
    std::cout << "GL Version:\t" << glGetString(GL_VERSION) << "\n";
    std::cout << "GL Shading ver:\t" << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n\n";
    
    GLint profile;
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
    if (const auto errorCode = glGetError()) {
        std::cout << "[!] Pending GL error while obtaining profile: " << errorCode << "\n";
        //return;
    }
    if (profile & GL_CONTEXT_CORE_PROFILE_BIT) {
        std::cout << "Core profile" << "\n";
    }
    else {
        std::cout << "Compatibility profile" << "\n";
    }
    std::cout << "=========================================\n\n";
}
