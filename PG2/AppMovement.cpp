#include <iostream>

#include "App.hpp"

void App::PlayerMovement(float delta_time) {
    camera_movement = camera.ProcessInput(window, delta_time);
    camera.position += camera_movement;
    // todo lepší gravitace, 7.0 do konstanty (výchozí pozice kamery), doøešit momentum
    if (camera.IsJumping()) {
        camera.position += world_up * Jump_speedUP * delta_time; // tady plus momentum
        if (camera.last_jump_time + Jump_time < glfwGetTime()) {
            camera.StopJump();
        }
    }
    else {
        camera.position -= world_up * Jump_speedDown * delta_time; // tady plus momentum
    }
    //ceck colisiont on ground
    float minPosition = GetMapY(camera.position.x, camera.position.z) + PlayerHeight;
    if (camera.position.y < minPosition)
    {
        camera.position = glm::vec3(camera.position.x, minPosition, camera.position.z);
        if (camera.Isfalling() || camera.IsJumping())
        {
            camera.OnLand();
            //todo play land music
        }
    }
    if (camera_movement.x > 0 || camera_movement.z > 0)
    {
        if (audio_walk_last_time + Walk_auidio_delay < glfwGetTime()) {
            if (camera.IsSprinting()) {
                //todo play sprinting audio
            }
            else {
                //todo play walking audio
            }
            audio_walk_last_time = glfwGetTime();
        }
    }

    glm::vec3 debugposition = camera.position; //only for debug todo delete this
    
}
void App::ChairMovement(float delta_time) {
    if (Last_chair_direction + Change_chair_direction < glfwGetTime()) {
        chair_direction = glm::vec3((float)rand() / RAND_MAX - 0.5f, 0, (float)rand() / RAND_MAX - 0.5f);
        Last_chair_direction = glfwGetTime();
    }
    scene_non_transparent["chair"]->position += chair_direction * delta_time * Chair_speed;
    if (scene_non_transparent["chair"]->position.x > Chair_max_X) 
    { scene_non_transparent["chair"]->position.x = Chair_max_X; }
    else if (scene_non_transparent["chair"]->position.x < Chair_min_X) 
    { scene_non_transparent["chair"]->position.x = Chair_min_X; }
    if (scene_non_transparent["chair"]->position.z > Chair_max_Z) 
    { scene_non_transparent["chair"]->position.z = Chair_max_Z; }
    else if (scene_non_transparent["chair"]->position.z < Chair_min_Z) 
    { scene_non_transparent["chair"]->position.z = Chair_min_Z; }

    float minPosition = GetMapY(scene_non_transparent["chair"]->position.x, scene_non_transparent["chair"]->position.z) + ChairHeight;
    scene_non_transparent["chair"]->position.y = minPosition;

}
void App::ProjectileMovement(float delta_time){

}