#include <iostream>

#include "App.hpp"
#define print(x) std::cout << x << "\n"

void App::PlayerMovement(float delta_time) {
    camera_movement = camera.ProcessInput(window, delta_time);
    camera.position += camera_movement;
    if (camera.IsJumping()) {
        camera.position += world_up * Jump_speedUP * delta_time;
        if (camera.last_jump_time + Jump_time < glfwGetTime()) {
            camera.StopJump();
        }
    }
    else {
        camera.position -= world_up * Jump_speedDown * delta_time;
    }
    //check collisions with ground
    float minPosition = GetMapY(camera.position.x, camera.position.z) + PlayerHeight;
    if (camera.position.y < minPosition)
    {
        camera.position = glm::vec3(camera.position.x, minPosition, camera.position.z);
        if (camera.Isfalling() || camera.IsJumping())
        {
            camera.OnLand();
            audio.Play2DOneShot("sound_land");
        }
    }
    if ((camera_movement.x > 0 || camera_movement.z > 0) && !camera.Isfalling() && !camera.IsJumping())
    {
        if (audio_walk_last_time + WALK_AUDIO_DELAY < glfwGetTime()) {
            audio.PlayFootstepSound(camera.IsSprinting());
            audio_walk_last_time = glfwGetTime();
        }
    }
    else
    {
        audio.StopFootstepSound();
    }
    
}
void App::ChairMovement(float delta_time) {
    if (Last_chair_direction + Change_chair_direction < glfwGetTime()) {
        chair_direction = glm::vec3((float)rand() / RAND_MAX - 0.5f, 0, (float)rand() / RAND_MAX - 0.5f);
        Last_chair_direction = glfwGetTime();
        chair_speed = 20.0f + static_cast<float>(rand()) / RAND_MAX * (80.0f - 20.0f); // random chair speed

        random_increment = 30.0f + static_cast<float>(rand()) / RAND_MAX * (450.0f - 30.0f);
    }
    scene_non_transparent["chair"]->position += chair_direction * delta_time * chair_speed;
    scene_non_transparent["chair"]->rotation.w += random_increment * delta_time;
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
void App::Shoot(){
    projectiles[projectile_id]->position = camera.position;
    projectile_directions[projectile_id] = camera.front;
    is_projectile_moving[projectile_id] = true;

    projectile_id = (projectile_id + 1) % N_PROJECTILES;
}
void App::ProjectileMovement(float delta_time){
    for (int i = 0; i < N_PROJECTILES; i++) {
        if (!is_projectile_moving[i]) { continue; }
        auto& position = projectiles[i]->position;
        position += projectile_directions[i] * projectile_speed * delta_time;
        //kolize s modely
        for (Model* collision : collisions) {
            if(IsCollision(position,collision))
            {
                position = glm::vec3(0, -10, 0);
                is_projectile_moving[i] = false;

                if (collision->id == 'c') { 
                    Teleport_chair();
                    audio.Play3DOneShot("sound_teleport", collision->position);
                }
            }
        }
        //kolize se zemí
        float groundY = GetMapY(position.x, position.z);
        if (position.y < groundY) {
            is_projectile_moving[i] = false;
            position.y = groundY;
        }
    }
}

bool App::IsCollision(glm::vec3 bullet,Model* model)
{
    //kvádr
    return bullet.x <= model->position.x + model->collision_max.x &&
        bullet.x >= model->position.x + model->collision_min.x &&
        bullet.y <= model->position.y + model->collision_max.y &&
        bullet.y >= model->position.y + model->collision_min.x &&
        bullet.z <= model->position.z + model->collision_max.z &&
        bullet.z >= model->position.z + model->collision_min.x;
}
void App::Teleport_chair(){
    unsigned int x = (unsigned int)(abs(Chair_max_X) + abs(Chair_min_X));
    unsigned int z = (unsigned int)(abs(Chair_max_Z) + abs(Chair_min_Z));
    scene_non_transparent["chair"]->position = glm::vec3(rand() % x - abs(Chair_min_X), -1 ,rand() % z - abs(Chair_min_Z));
}
