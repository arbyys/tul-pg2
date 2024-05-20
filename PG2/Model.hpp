#pragma once

#include <filesystem>

#include "Vertex.hpp"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"

class Model
{
public:
    Model(const std::filesystem::path& file_name);
    void Draw(const ShaderProgram& shader);
private:
    std::vector<Mesh> meshes;
    std::string name;

    //GLuint LoadTexture(const std::filesystem::path& file_name);
    //...
    //TODO GLuint Model::LoadTexture ?

    std::vector<Vertex> vertices{};
    std::vector<GLuint> vertex_indices{};

    // Reading the file
    std::string file_line;
    std::vector<std::string> file_lines;
    void FillFileLines(const std::filesystem::path& file_name);
    void LoadOBJFile(const std::filesystem::path& file_name, std::vector<Vertex>& out_vertices, std::vector<GLuint>& out_vertex_indices);
    void LoadMTLFile(const std::filesystem::path& file_name);
};
