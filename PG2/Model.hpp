#pragma once

#include <filesystem>
#include <map>

#include "Vertex.hpp"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"

class Model
{
public:
    Model(glm::vec3 position, float scale, glm::vec4 rotation);
    Model(const std::filesystem::path& obj_file_path, const std::filesystem::path& texture_file_path, glm::vec3 position, float scale, glm::vec4 rotation);
    static Model* CreateTerrain(glm::vec3 position, float scale, glm::vec4 rotation, std::map<std::pair<unsigned int, unsigned int>, unsigned int>* heights);
    void Draw(ShaderProgram& shader);
    float camera_distance;

    glm::vec3 position{};

    char id = 'u';
    glm::vec3 collision_min{};
    glm::vec3 collision_max{};
    glm::vec4 rotation = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

private:
    Mesh mesh;
    //std::vector<Mesh> meshes;
    std::string name;

    float scale{};
    glm::vec4 initial_rotation{};

    //GLuint LoadTexture(const std::filesystem::path& file_name);
    //...


    std::vector<Vertex> vertices{};
    std::vector<GLuint> vertex_indices{};

    
    static glm::vec2 GetTextureByHeight(unsigned int height);//help for create terrain
    // Reading the file
    std::string file_line;
    std::vector<std::string> file_lines;
    void FillFileLines(const std::filesystem::path& file_name);
    void LoadOBJFile(const std::filesystem::path& file_name, std::vector<Vertex>& out_vertices, std::vector<GLuint>& out_vertex_indices);
};
