#include "DmdMesh.h"

#include <cmath>
#include <fstream>
#include <glm/ext/vector_float2.hpp>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string_view>
#include <glm/ext/vector_float3.hpp>
#include <utility>
#include <vector>

DmdMesh::DmdMesh(std::string_view full_dmd_mesh_path)
{
    parse_dmd_mesh(open_dmd_mesh(full_dmd_mesh_path));
}

std::ifstream DmdMesh::open_dmd_mesh(std::string_view full_dmd_mesh_path) const
{
    std::ifstream dmd_mesh { full_dmd_mesh_path.data() };
    if (!dmd_mesh)
    {
        throw std::runtime_error { "Failed to open" };
    }

    return dmd_mesh;
}

// TODO: split on several functions
void DmdMesh::parse_dmd_mesh(std::ifstream&& dmd_mesh)
{
    std::string buffer {};
    while (buffer != "TriMesh()")
    {
        dmd_mesh >> buffer;
        if (dmd_mesh.fail() || dmd_mesh.eof())
        {
            throw std::runtime_error { "Failed to find TriMesh() in" };
        }
    }

    dmd_mesh >> buffer >> buffer;

    int position_count {};
    int position_face_count {};
    dmd_mesh >> position_count >> position_face_count;

    int index_count { position_face_count * 3 };

    dmd_mesh >> buffer >> buffer;

    std::vector<glm::vec3> positions(position_count);
    for (auto& position : positions)
    {
        for (int i { 0 }; i < 3; ++i)
        {
            dmd_mesh >> position[i];
            if (dmd_mesh.fail() || !std::isfinite(position[i]))
            {
                throw std::runtime_error { "Wrong position value in" };
            }
        }
    }

    dmd_mesh >> buffer >> buffer >> buffer >> buffer;

    std::vector<int> position_indices(index_count);
    for (auto& index : position_indices)
    {
        dmd_mesh >> index;
        --index;
    }

    while (buffer != "Texture:")
    {
        dmd_mesh >> buffer;
        if (dmd_mesh.fail() || dmd_mesh.eof())
        {
            throw std::runtime_error { "Failed to find \"Texture:\" in" };
        }
    }

    dmd_mesh >> buffer >> buffer;

    int tex_coord_count {};
    int tex_coord_face_count {};
    dmd_mesh >> tex_coord_count >> tex_coord_face_count;

    if (position_face_count != tex_coord_face_count)
    {
        throw std::runtime_error { "Different position and tex coord face count in" };
    }

    dmd_mesh >> buffer >> buffer;

    std::vector<glm::vec2> tex_coords(tex_coord_count);
    for (auto& tex_coord : tex_coords)
    {
        for (int i { 0 }; i < 2; ++i)
        {
            dmd_mesh >> tex_coord[i];
            if (dmd_mesh.fail() || !std::isfinite(tex_coord[i]))
            {
                throw std::runtime_error { "Wrong tex coord value in" };
            }
        }

        dmd_mesh >> buffer;
    }

    dmd_mesh >> buffer >> buffer >> buffer >> buffer >> buffer;

    std::vector<int> tex_coord_indices(index_count);
    for (auto& index : tex_coord_indices)
    {
        dmd_mesh >> index;
        --index;
    }

    vertices.reserve(index_count);
    indices.resize(index_count);

    std::map<std::pair<int, int>, int> unique_indices {};
    for (int i { 0 }; i < index_count; ++i)
    {
        const int position_index { position_indices[i] };
        const int tex_coord_index { tex_coord_indices[i] };

        const auto found_it { unique_indices.find(std::make_pair(position_index, tex_coord_index)) };
        if (found_it == unique_indices.end())
        {
            vertices.emplace_back(Vertex { positions[position_index], tex_coords[tex_coord_index] });
            indices[i] = vertices.size() - 1;
            unique_indices.emplace(std::make_pair(position_index, tex_coord_index), vertices.size() - 1);
        }
        else
        {
            indices[i] = found_it->second;
        }
    }

    vertices.shrink_to_fit();
}
