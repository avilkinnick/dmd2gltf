#include "DmdMesh.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string_view>

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

void DmdMesh::parse_dmd_mesh(std::ifstream&& dmd_mesh)
{

}
