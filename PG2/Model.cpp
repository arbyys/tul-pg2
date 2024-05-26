#include <iostream>
#include <fstream>
#include <string>

//#include "OBJLoader.hpp"
#include "Model.hpp"

#include "Texture.hpp"


#define print(x) std::cout << x << "\n"

Model::Model(glm::vec3 position, float scale, glm::vec4 initial_rotation):
    position(position),
    scale(scale),
    initial_rotation(initial_rotation)
{}

Model::Model(const std::filesystem::path& obj_file_path, const std::filesystem::path& texture_file_path, glm::vec3 position, float scale, glm::vec4 initial_rotation) :
    position(position),
    scale(scale),
    initial_rotation(initial_rotation)
{

    LoadOBJFile(obj_file_path, vertices, vertex_indices);

    GLuint texture_id = TextureInit(texture_file_path.string().c_str());
    mesh = Mesh(GL_TRIANGLES, vertices, vertex_indices, texture_id);
}

Model* Model::CreateTerrain(glm::vec3 position, float scale, glm::vec4 rotation, std::map<std::pair<unsigned int, unsigned int>, unsigned int>* heights) {
    Model* to_return = new Model(position, scale, rotation);

    glm::vec3 normalA{};
    glm::vec3 normalB{};
    glm::vec3 normal{};
    unsigned int indices_counter = 0;
    
    cv::Mat map = cv::imread("./resources/heightmap/map.png", cv::IMREAD_GRAYSCALE);

    unsigned int step_size = 10;
    unsigned int max_height = 0;

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
            if (avarangeHeight > max_height) { max_height = avarangeHeight; }
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
            to_return->vertex_indices.emplace_back(indices_counter + 2);
            to_return->vertex_indices.emplace_back(indices_counter + 1);
            to_return->vertex_indices.emplace_back(indices_counter);
            to_return->vertex_indices.emplace_back(indices_counter + 3);
            to_return->vertex_indices.emplace_back(indices_counter + 2);

            indices_counter += 4;

            heights->insert({ std::pair<unsigned int, unsigned int>{x, z} ,(unsigned int)(map.at<uchar>(cv::Point(x, z))) });
        }
    }

    //height for collision 
    unsigned int remainderx = map.cols % step_size;
    unsigned int remaindery = map.rows % step_size;
    /*heights[{ map.cols - remainderx - step_size, map.rows - remaindery}] = map.at<uchar>(cv::Point(map.cols - remainderx - step_size, map.rows - remaindery));
    heights[{ map.cols - remainderx ,map.rows - remaindery}] = map.at<uchar>(cv::Point(map.cols - remainderx, map.rows - remaindery));
    heights[{ map.cols - remainderx, map.rows - remaindery - step_size}] = map.at<uchar>(cv::Point(map.cols - remainderx, map.rows - remaindery - step_size));*/
    heights->insert({ std::pair<unsigned int, unsigned int>{ map.cols - remainderx - step_size, map.rows - remaindery} ,(unsigned int)(map.at<uchar>(cv::Point(map.cols - remainderx - step_size, map.rows - remaindery)))});
    heights->insert({ std::pair<unsigned int, unsigned int>{ map.cols - remainderx, map.rows - remaindery} , (unsigned int)(map.at<uchar>(cv::Point(map.cols - remainderx , map.rows - remaindery)))});
    heights->insert({ std::pair<unsigned int, unsigned int>{ map.cols - remainderx, map.rows - remaindery - step_size} , (unsigned int)(map.at<uchar>(cv::Point(map.cols - remainderx , map.rows - remaindery - step_size)))});

    std::filesystem::path texture_path = "./resources/textures/atlas.png";
    GLuint texture_id = TextureInit(texture_path.string().c_str());
    Mesh mesh = Mesh(GL_TRIANGLES, to_return->vertices, to_return->vertex_indices, texture_id);
    to_return->mesh = mesh;
    return to_return;
}

glm::vec2 Model::GetTextureByHeight(unsigned int height){
    if (height < 90) {
        return glm::vec2(0.0f, 0.0f);
    }
    if (height < 170) {
        return glm::vec2(1.0f / 16, 0.0f);
    }
    return glm::vec2(2.0f / 16, 0.0f);
    
}

void Model::Draw(ShaderProgram& shader) {

    // called to draw each object
    auto mx_model = glm::identity<glm::mat4>();
    mx_model = glm::translate(mx_model, position);
    mx_model = glm::scale(mx_model, glm::vec3(scale));
    mx_model = glm::rotate(mx_model, glm::radians(initial_rotation.w), glm::vec3(initial_rotation.x, initial_rotation.y, initial_rotation.z));
    mx_model = glm::rotate(mx_model, glm::radians(rotation.w), glm::vec3(rotation.x, rotation.y, rotation.z));

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
    out_vertices.clear();
    out_vertex_indices.clear();

    // file read, vector load
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texture_coordinates;
    std::vector<glm::vec3> vertex_normals;
    std::vector<GLuint> indices_vertex, indices_texture_coordinate, indices_vertex_normal;

    std::string first_two_chars, first_three_chars;
    glm::vec2 uv;
    glm::vec3 vertex_or_normal;

    bool line_success;

    std::string line;
    std::ifstream file_reader(file_name);
    while (getline(file_reader, line)) {
        if (!line.empty()) {
            line_success = true;
            first_two_chars = line.substr(0, 2);
            first_three_chars = line.substr(0, 3);
            if (first_two_chars == "v ") {
                vertex_or_normal = {};
                (void)sscanf_s(line.c_str(), "v %f %f %f", &vertex_or_normal.x, &vertex_or_normal.y, &vertex_or_normal.z);
                vertices.push_back(vertex_or_normal);
            }
            else if (first_three_chars == "vt ") {
                uv = {};
                (void)sscanf_s(line.c_str(), "vt %f %f", &uv.x, &uv.y);
                uv.y = -uv.y; // DDS textures are inverted
                texture_coordinates.push_back(uv);
            }
            else if (first_three_chars == "vn ") {
                vertex_or_normal = {};
                (void)sscanf_s(line.c_str(), "vn %f %f %f", &vertex_or_normal.x, &vertex_or_normal.y, &vertex_or_normal.z);
                vertex_normals.push_back(vertex_or_normal);
            }
            else if (first_two_chars == "f ") {
                auto n = std::count(line.begin(), line.end(), '/');
                if (n == 0) {
                    unsigned int indices_temp[3]{};
                    (void)sscanf_s(line.c_str(), "f %d %d %d", &indices_temp[0], &indices_temp[1], &indices_temp[2]);
                    indices_vertex.insert(indices_vertex.end(), { indices_temp[0], indices_temp[1], indices_temp[2] });
                }
                else if (n == 3) {
                    unsigned int indices_temp[6]{};
                    (void)sscanf_s(line.c_str(), "f %d/%d %d/%d %d/%d", &indices_temp[0], &indices_temp[3], &indices_temp[1], &indices_temp[4], &indices_temp[2], &indices_temp[5]);
                    indices_vertex.insert(indices_vertex.end(), { indices_temp[0], indices_temp[1], indices_temp[2] });
                    indices_texture_coordinate.insert(indices_texture_coordinate.end(), { indices_temp[3], indices_temp[4], indices_temp[5] });
                }
                else if (n == 6) {
                    if (line.find("//") != std::string::npos) {
                        unsigned int indices_temp[6]{};
                        (void)sscanf_s(line.c_str(), "f %d//%d %d//%d %d//%d", &indices_temp[0], &indices_temp[3], &indices_temp[1], &indices_temp[4], &indices_temp[2], &indices_temp[5]);
                        indices_vertex.insert(indices_vertex.end(), { indices_temp[0], indices_temp[1], indices_temp[2] });
                        indices_vertex_normal.insert(indices_vertex_normal.end(), { indices_temp[3], indices_temp[4], indices_temp[5] });
                    }
                    else {
                        unsigned int indices_temp[9]{};
                        (void)sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &indices_temp[0], &indices_temp[3], &indices_temp[6], &indices_temp[1], &indices_temp[4], &indices_temp[7], &indices_temp[2], &indices_temp[5], &indices_temp[8]);
                        indices_vertex.insert(indices_vertex.end(), { indices_temp[0], indices_temp[1], indices_temp[2] });
                        indices_texture_coordinate.insert(indices_texture_coordinate.end(), { indices_temp[3], indices_temp[4], indices_temp[5] });
                        indices_vertex_normal.insert(indices_vertex_normal.end(), { indices_temp[6], indices_temp[7], indices_temp[8] });
                    }
                }
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
                print("OBJLoader is ignoring line '" << line << "' in file '" << file_name << "'");
            }
        }
    }
    file_reader.close();

    // indirect -> direct
    std::vector<glm::vec3> vertices_direct;
    std::vector<glm::vec2> texture_coordinates_direct;
    std::vector<glm::vec3> vertex_normals_direct;

    for (unsigned int u = 0; u < indices_vertex.size(); u++) {
        vertices_direct.push_back(vertices[indices_vertex[u] - 1]);
    }
    for (unsigned int u = 0; u < indices_texture_coordinate.size(); u++) {
        texture_coordinates_direct.push_back(texture_coordinates[indices_texture_coordinate[u] - 1]);
    }
    for (unsigned int u = 0; u < indices_vertex_normal.size(); u++) {
        vertex_normals_direct.push_back(vertex_normals[indices_vertex_normal[u] - 1]);
    }

    auto n_direct_uvs = texture_coordinates_direct.size();
    auto n_direct_normals = vertex_normals_direct.size();

    // vectors to vertex vector
    for (unsigned int u = 0; u < vertices_direct.size(); u++) {
        Vertex vertex{};
        vertex.position = vertices_direct[u];
        if (u < n_direct_uvs) vertex.tex_coords = texture_coordinates_direct[u];
        if (u < n_direct_normals) vertex.normal = vertex_normals_direct[u];
        out_vertices.push_back(vertex);
        out_vertex_indices.push_back(u);
    }
    print("Succesfully loaded OBJ " << file_name << "\n");
}