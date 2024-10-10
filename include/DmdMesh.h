#ifndef DMD_MESH_H
#define DMD_MESH_H

#include <fstream>
#include <string_view>
#include <vector>

#include "Vertex.h"

class DmdMesh
{
public:
    DmdMesh(std::string_view full_dmd_mesh_path);

    std::ifstream open_dmd_mesh(std::string_view full_dmd_mesh_path) const;
    void parse_dmd_mesh(std::ifstream&& dmd_mesh);

private:
    std::vector<Vertex> vertices {};
    std::vector<int> indices {};
};

#endif // DMD_MESH_H
