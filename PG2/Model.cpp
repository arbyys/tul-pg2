#include <iostream>
#include <fstream>
#include <string>

//#include "OBJLoader.hpp"
#include "Model.hpp"

#include "Texture.hpp"


#define print(x) std::cout << x << "\n"

Model::Model(glm::vec3 position, float scale, glm::vec4 rotation):
    position(position),
    scale(scale),
    rotation(rotation)
{}

Model::Model(const std::filesystem::path& obj_file_path, const std::filesystem::path& texture_file_path, glm::vec3 position, float scale, glm::vec4 rotation) :
    position(position),
    scale(scale),
    rotation(rotation)
{

    LoadOBJFile(obj_file_path, vertices, vertex_indices);

    GLuint texture_id = TextureInit(texture_file_path.string().c_str());
    mesh = Mesh(GL_TRIANGLES, vertices, vertex_indices, texture_id);
    //meshes.push_back(mesh);
}

Model* Model::CreateTerrain(glm::vec3 position, float scale, glm::vec4 rotation) {
    Model* to_return = new Model(position, scale, rotation);

    glm::vec3 normalA{};
    glm::vec3 normalB{};
    glm::vec3 normal{};
    unsigned int indices_counter = 0;
    
    cv::Mat map = cv::imread("./resources/heightmap/map.png", cv::IMREAD_GRAYSCALE);

    unsigned int step_size = 10;
    //  ^   3-----2
    //  |   |    /|
    //  |   |  /  |
    //  |   |/    |
    //  z   0-----1
    //   x ------->
    for (unsigned int x = 0; x < map.cols - step_size;x += step_size ){
        for (unsigned int z = 0; z < map.rows - step_size; z += step_size) {
            glm::vec3 p0(x, map.at<uchar>(cv::Point(x,z)), z);
            glm::vec3 p1(x + step_size, map.at<uchar>(cv::Point(x+step_size,z)), z);
            glm::vec3 p2(x + step_size, map.at<uchar>(cv::Point(x+ step_size, z+ step_size)), z + step_size);
            glm::vec3 p3(x , map.at<uchar>(cv::Point(x, z+ step_size)), z + step_size);

            unsigned int avarangeHeight = (map.at<uchar>(cv::Point(x, z))
                + map.at<uchar>(cv::Point(x+ step_size, z))
                + map.at<uchar>(cv::Point(x+ step_size, z+ step_size))
                + map.at<uchar>(cv::Point(x, z+ step_size))) / 4;
            //todo textures
            glm::vec2 tc0 = GetTextureByHeight(avarangeHeight);
            glm::vec2 tc1 = tc0 + glm::vec2((1.0f / 16), 0.0f);		    
            glm::vec2 tc2 = tc0 + glm::vec2((1.0f / 16), (1.0f / 16));  
            glm::vec2 tc3 = tc0 + glm::vec2(0.0f, (1.0f / 16));
            //normal vectors
            normalA = glm::normalize(glm::cross(p1 - p0, p2 - p0));
            normalB = glm::normalize(glm::cross(p2 - p0, p3 - p0));

            to_return->vertices.emplace_back(Vertex{ p0, -normal, tc0 });
            to_return->vertices.emplace_back(Vertex{ p1, -normal, tc1 });
            to_return->vertices.emplace_back(Vertex{ p2, -normal, tc2 });
            to_return->vertices.emplace_back(Vertex{ p3, -normal, tc3 });

            to_return->vertex_indices.emplace_back(indices_counter);
            to_return->vertex_indices.emplace_back(indices_counter + 2); //todo záleží na pořadí ?
            to_return->vertex_indices.emplace_back(indices_counter + 1);
            to_return->vertex_indices.emplace_back(indices_counter);
            to_return->vertex_indices.emplace_back(indices_counter + 3);
            to_return->vertex_indices.emplace_back(indices_counter + 2);

            indices_counter += 4;

        }
    }

    std::filesystem::path texture_path = "./resources/textures/atlas.png";
    GLuint texture_id = TextureInit(texture_path.string().c_str());
    Mesh mesh = Mesh(GL_TRIANGLES, to_return->vertices, to_return->vertex_indices, texture_id);
    to_return->mesh = mesh;
    return to_return;
}

glm::uvec2 Model::GetTextureByHeight(unsigned int height){
    if (height > 25) {
        return glm::uvec2(0.0f, 0.0f);
    }
    return glm::uvec2(0.0f + 1.0f / 16, 0.0f);
}

void Model::Draw(ShaderProgram& shader) {

    // Einheitsmatrix
    auto mx_model = glm::identity<glm::mat4>();
    // Move object
    mx_model = glm::translate(mx_model, position);
    // Scale object (scale in all three dimensions must be the same in this "engine")
    mx_model = glm::scale(mx_model, glm::vec3(scale));
    // Initial rotation (should be set only once when creating the Model)
    mx_model = glm::rotate(mx_model, glm::radians(rotation.w), glm::vec3(rotation.x, rotation.y, rotation.z));
    // Additional rotation
    mx_model = glm::rotate(mx_model, glm::radians(rotation.w), glm::vec3(rotation.x, rotation.y, rotation.z));

    // call Draw() on all meshes
    //for (auto const& mesh : meshes) {
    //    mesh.Draw(shader, mx_model);
    //}
    mesh.Draw(shader, mx_model);
}

void Model::FillFileLines(const std::filesystem::path& file_name)
{
    file_lines.clear();
    std::ifstream file_reader(file_name);
    while (getline(file_reader, file_line)) {
        file_lines.push_back(file_line);
    }
    file_reader.close();
}

void Model::LoadOBJFile(const std::filesystem::path& file_name, std::vector<Vertex>& out_vertices, std::vector<GLuint>& out_vertex_indices)
{
    FillFileLines(file_name);

    out_vertices.clear();
    out_vertex_indices.clear();

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texture_coordinates;
    std::vector<glm::vec3> vertex_normals;
    std::vector<GLuint> indices_vertex, indices_texture_coordinate, indices_vertex_normal;

    std::string first_two_chars, first_three_chars;
    glm::vec2 uv;
    glm::vec3 vertex_or_normal;

    bool line_success;

    for (const std::string& line : file_lines) {
        if (!line.empty()) {
            line_success = true;
            first_two_chars = line.substr(0, 2);
            first_three_chars = line.substr(0, 3);
            // v -1.183220029 4.784470081 47.4618988
            if (first_two_chars == "v ") {
                vertex_or_normal = {};
                (void)sscanf_s(line.c_str(), "v %f %f %f", &vertex_or_normal.x, &vertex_or_normal.y, &vertex_or_normal.z);
                vertices.push_back(vertex_or_normal);
            }
            // vt 0.5000 0.7500
            else if (first_three_chars == "vt ") {
                uv = {};
                (void)sscanf_s(line.c_str(), "vt %f %f", &uv.y, &uv.x);
                texture_coordinates.push_back(uv);
            }
            // vn 0.7235898972 -0.6894102097 -0.03363365307
            else if (first_three_chars == "vn ") {
                vertex_or_normal = {};
                (void)sscanf_s(line.c_str(), "vn %f %f %f", &vertex_or_normal.x, &vertex_or_normal.y, &vertex_or_normal.z);
                vertex_normals.push_back(vertex_or_normal);
            }
            else if (first_two_chars == "f ") {
                auto n = std::count(line.begin(), line.end(), '/');
                // f 1 2 3
                if (n == 0) {
                    unsigned int indices_temp[3]{};
                    (void)sscanf_s(line.c_str(), "f %d %d %d", &indices_temp[0], &indices_temp[1], &indices_temp[2]);
                    indices_vertex.insert(indices_vertex.end(), { indices_temp[0], indices_temp[1], indices_temp[2] });
                }
                // f 3/1 4/2 5/3
                else if (n == 3) {
                    unsigned int indices_temp[6]{};
                    (void)sscanf_s(line.c_str(), "f %d/%d %d/%d %d/%d", &indices_temp[0], &indices_temp[3], &indices_temp[1], &indices_temp[4], &indices_temp[2], &indices_temp[5]);
                    indices_vertex.insert(indices_vertex.end(), { indices_temp[0], indices_temp[1], indices_temp[2] });
                    indices_texture_coordinate.insert(indices_texture_coordinate.end(), { indices_temp[3], indices_temp[4], indices_temp[5] });
                }
                else if (n == 6) {
                    // f 7//1 8//2 9//3
                    if (line.find("//") != std::string::npos) {
                        unsigned int indices_temp[6]{};
                        (void)sscanf_s(line.c_str(), "f %d//%d %d//%d %d//%d", &indices_temp[0], &indices_temp[3], &indices_temp[1], &indices_temp[4], &indices_temp[2], &indices_temp[5]);
                        indices_vertex.insert(indices_vertex.end(), { indices_temp[0], indices_temp[1], indices_temp[2] });
                        indices_vertex_normal.insert(indices_vertex_normal.end(), { indices_temp[3], indices_temp[4], indices_temp[5] });
                    }                
                    // f 6/4/1 3/5/3 7/6/5
                    else {
                        unsigned int indices_temp[9]{};
                        (void)sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &indices_temp[0], &indices_temp[3], &indices_temp[6], &indices_temp[1], &indices_temp[4], &indices_temp[7], &indices_temp[2], &indices_temp[5], &indices_temp[8]);
                        indices_vertex.insert(indices_vertex.end(), { indices_temp[0], indices_temp[1], indices_temp[2] });
                        indices_texture_coordinate.insert(indices_texture_coordinate.end(), { indices_temp[3], indices_temp[4], indices_temp[5] });
                        indices_vertex_normal.insert(indices_vertex_normal.end(), { indices_temp[6], indices_temp[7], indices_temp[8] });
                    }
                }
                // f 1/1/1 2/2/2 22/23/3 21/22/4
                else if (n == 8) {
                    unsigned int v[4]{};
                    unsigned int vt[4]{};
                    unsigned int vn[4]{};
                    (void)sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v[0], &vt[0], &vn[0], &v[1], &vt[1], &vn[1], &v[2], &vt[2], &vn[2], &v[3], &vt[3], &vn[3]);
                    indices_vertex.insert(indices_vertex.end(), { v[0], v[1], v[2], v[0], v[2], v[3] });
                    indices_texture_coordinate.insert(indices_texture_coordinate.end(), { vt[0], vt[1], vt[2], vt[0], vt[2], vt[3] });
                    indices_vertex_normal.insert(indices_vertex_normal.end(), { vn[0], vn[1], vn[2], vn[0], vn[2], vn[3] });
                }
                else {
                    line_success = false;
                }
            }
            else {
                line_success = false;
            }

            if (!line_success && first_two_chars != "# ") {
                print("LoadOBJFile: Ignoring line '" << line << "' in file" << file_name);
            }
        }
    }

    // Set output vertices
    auto n_indirect_vertices = vertices.size();
    auto n_indirect_uvs = texture_coordinates.size();
    auto n_indirect_normals = vertex_normals.size();
    for (unsigned int u = 0; u < n_indirect_vertices; u++) {
        Vertex vertex{};
        vertex.position = vertices[u];
        if (u < n_indirect_uvs) vertex.tex_coords = texture_coordinates[u];
        if (u < n_indirect_normals) vertex.normal = vertex_normals[u];
        out_vertices.push_back(vertex);
    }

    bool use_direct = true;
    if (use_direct) {
        // RETARDED DRAW
        std::vector<Vertex> temp_vertices(out_vertices);
        out_vertices.clear();
        for (unsigned int u = 0; u < indices_vertex.size(); u++) {
            out_vertices.push_back(temp_vertices[indices_vertex[u] - 1]);
            out_vertex_indices.push_back(u);
        }
    }
    else {
        // INDIRECT DRAW
        out_vertex_indices = indices_vertex;
    }

    // Print done
    print("LoadOBJFile: Loaded OBJ file " << file_name);
}